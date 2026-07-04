#include "Aplicacao/Servicos/InicializadorDaFazenda.hpp"
#include "Aplicacao/Servicos/ServicoDeFerramentas.hpp"
#include "Aplicacao/Servicos/ServicoDeTempo.hpp"
#include "Apresentacao/Animacao/AnimadorDoPersonagem.hpp"
#include "Apresentacao/Camera/CameraDoJogo.hpp"
#include "Apresentacao/ConfiguracoesDoLayout.hpp"
#include "Apresentacao/Interface/BarraDeFerramentas/BarraDeFerramentas.hpp"
#include "Apresentacao/Interface/EstadoDaCenaFazenda.hpp"
#include "Apresentacao/Interface/Loja/ControladorDaLoja.hpp"
#include "Apresentacao/Isometria/Isometrico.hpp"
#include "Compartilhado/ConstantesDaCamera.hpp"
#include "Compartilhado/ConstantesDaJanela.hpp"
#include "Compartilhado/ConstantesDoJogo.hpp"
#include "Compartilhado/Geometria/Posicoes.hpp"
#include "Dominio/Canteiros/EstadoDoCanteiro.hpp"
#include "Dominio/Ferramentas/ResultadoDaFerramenta.hpp"
#include "Dominio/Ferramentas/TipoDeFerramenta.hpp"
#include "Dominio/Mapa/MapaDaFazenda.hpp"
#include "Dominio/Ocupacao/GridDeOcupacao.hpp"
#include "Dominio/Personagem/Personagem.hpp"
#include "Dominio/Plantas/FabricaDePlantas.hpp"
#include "Dominio/Plantas/Especies/PlantaMirtilo.hpp"
#include "Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp"
#include "Infraestrutura/Assets/EstadoVisualDaPlanta.hpp"

#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <iostream>

namespace {

namespace AppServicos = MiniFazenda::Aplicacao::Servicos;
namespace Animacao = MiniFazenda::Apresentacao::Animacao;
namespace Assets = MiniFazenda::Infraestrutura::Assets;
namespace BarraFerramentas = MiniFazenda::Apresentacao::Interface::BarraDeFerramentas;
namespace Camera = MiniFazenda::Apresentacao::Camera;
namespace Canteiros = MiniFazenda::Dominio::Canteiros;
namespace ConfigPersonagem = MiniFazenda::Infraestrutura::Assets::ConfigVisualDoPersonagem;
namespace Constantes = MiniFazenda::Compartilhado::Constantes;
namespace Especies = MiniFazenda::Dominio::Plantas::Especies;
namespace Ferramentas = MiniFazenda::Dominio::Ferramentas;
namespace Geometria = MiniFazenda::Compartilhado::Geometria;
namespace Interface = MiniFazenda::Apresentacao::Interface;
namespace Isometria = MiniFazenda::Apresentacao::Isometria;
namespace Loja = MiniFazenda::Apresentacao::Interface::Loja;
namespace Mapa = MiniFazenda::Dominio::Mapa;
namespace Ocupacao = MiniFazenda::Dominio::Ocupacao;
namespace Personagem = MiniFazenda::Dominio::Personagem;
namespace Plantas = MiniFazenda::Dominio::Plantas;

[[noreturn]] void falharTeste(const char* arquivo, int linha, const char* condicao) {
    std::cerr << arquivo << ':' << linha << ": falha no teste: esperado `" << condicao << "`\n";
    std::exit(EXIT_FAILURE);
}

#define VERIFICAR(condicao) \
    do { \
        if (!(condicao)) { \
            falharTeste(__FILE__, __LINE__, #condicao); \
        } \
    } while (false)

bool posicaoDecimalDeOcupacaoEstaProxima(
    Geometria::PosicaoDecimalNaGradeDeOcupacao atual,
    Geometria::PosicaoNaGradeDeOcupacao esperada,
    float tolerancia = 0.01f
) {
    return std::fabs(atual.indiceColuna - static_cast<float>(esperada.indiceColuna)) <= tolerancia &&
           std::fabs(atual.indiceLinha - static_cast<float>(esperada.indiceLinha)) <= tolerancia;
}

void validarListasDoMapa(const Mapa::MapaDaFazenda& mapa) {
    VERIFICAR(mapa.entidades().size() == mapa.quantidadeDeEntidades());
    VERIFICAR(mapa.indiceDeOcupacao().quantidadeDeOcupacoes() == mapa.quantidadeDeEntidades());

    for (const Mapa::EntidadeDoMapa& entidade : mapa.entidades()) {
        VERIFICAR(entidade.identificador() != Ocupacao::IDENTIFICADOR_INVALIDO_DE_ENTIDADE_DE_MAPA);
        VERIFICAR(mapa.obterEntidade(entidade.identificador()) != nullptr);

        if (entidade.ehCanteiroAgricola()) {
            VERIFICAR(entidade.canteiroAgricola() != nullptr);

            const Geometria::AreaNaGradeDeOcupacao area = entidade.areaDeOcupacao();
            for (int linha = area.indiceLinha; linha < area.indiceLinha + area.altura; ++linha) {
                for (int coluna = area.indiceColuna; coluna < area.indiceColuna + area.largura; ++coluna) {
                    const Mapa::EntidadeDoMapa* entidadeNoMapa =
                        mapa.entidadeEm(Geometria::PosicaoNaGradeDeOcupacao{coluna, linha});
                    VERIFICAR(entidadeNoMapa != nullptr);
                    VERIFICAR(entidadeNoMapa->identificador() == entidade.identificador());
                }
            }
        }
    }

    VERIFICAR(
        mapa.identificadoresDeCanteirosEmCrescimento().size() ==
        mapa.quantidadeDeCanteirosEmCrescimento()
    );

    for (const Ocupacao::IdentificadorDeEntidadeDeMapa identificador :
         mapa.identificadoresDeCanteirosEmCrescimento()) {
        const Mapa::EntidadeDoMapa* entidade = mapa.obterEntidade(identificador);
        VERIFICAR(entidade != nullptr);
        VERIFICAR(entidade->ehCanteiroAgricola());
        VERIFICAR(entidade->canteiroAgricola() != nullptr);
        VERIFICAR(entidade->canteiroAgricola()->precisaAvancarCrescimento());
    }
}

void validarGridDeOcupacaoBasico() {
    Ocupacao::GridDeOcupacao grid{8, 8};
    const Geometria::AreaNaGradeDeOcupacao areaDaArvore{2, 2, 1, 1};
    constexpr Ocupacao::IdentificadorDeEntidadeDeMapa idDaArvore = 1;

    VERIFICAR(grid.registrarOcupacao(idDaArvore, areaDaArvore));
    VERIFICAR(grid.quantidadeDeOcupacoes() == 1);
    VERIFICAR(!grid.areaEstaLivre(areaDaArvore));

    const Ocupacao::RegistroDeOcupacao* arvore = grid.ocupacaoEm(Geometria::PosicaoNaGradeDeOcupacao{2, 2});
    VERIFICAR(arvore != nullptr);
    VERIFICAR(arvore->identificador == idDaArvore);
    VERIFICAR(arvore->ocupa(Geometria::PosicaoNaGradeDeOcupacao{2, 2}));
    VERIFICAR(!arvore->ocupa(Geometria::PosicaoNaGradeDeOcupacao{3, 2}));
    VERIFICAR(!grid.registrarOcupacao(2, areaDaArvore));

    const Geometria::AreaNaGradeDeOcupacao areaDaCasa{4, 4, 3, 2};
    constexpr Ocupacao::IdentificadorDeEntidadeDeMapa idDaCasa = 3;
    VERIFICAR(grid.registrarOcupacao(idDaCasa, areaDaCasa));
    VERIFICAR(grid.quantidadeDeOcupacoes() == 2);

    const Ocupacao::RegistroDeOcupacao* casa = grid.ocupacaoPorIdentificador(idDaCasa);
    VERIFICAR(casa != nullptr);
    VERIFICAR(casa->profundidadeDaBase() == 11);
    VERIFICAR(grid.ocupacaoEm(Geometria::PosicaoNaGradeDeOcupacao{6, 5}) == casa);
    VERIFICAR(!grid.areaEstaLivre(Geometria::AreaNaGradeDeOcupacao{7, 7, 2, 2}));

    VERIFICAR(grid.removerOcupacao(idDaArvore));
    VERIFICAR(grid.quantidadeDeOcupacoes() == 1);
    VERIFICAR(grid.ocupacaoEm(Geometria::PosicaoNaGradeDeOcupacao{2, 2}) == nullptr);
}

void validarConversaoDeCanteiroParaOcupacao() {
    const Geometria::PosicaoDeCanteiroNoMapa posicaoDoCanteiro{127, 128};
    const Geometria::PosicaoNaGradeDeOcupacao origem =
        Ocupacao::converterCanteiroParaOcupacao(posicaoDoCanteiro);
    const Geometria::AreaNaGradeDeOcupacao area =
        Ocupacao::calcularAreaDeOcupacaoDoCanteiro(posicaoDoCanteiro);

    VERIFICAR(Constantes::UNIDADES_DE_OCUPACAO_POR_CANTEIRO == 2);
    VERIFICAR(Constantes::LARGURA_DA_UNIDADE_DE_OCUPACAO == 64);
    VERIFICAR(Constantes::ALTURA_DA_UNIDADE_DE_OCUPACAO == 32);
    VERIFICAR(origem.indiceColuna == posicaoDoCanteiro.indiceColuna * 2);
    VERIFICAR(origem.indiceLinha == posicaoDoCanteiro.indiceLinha * 2);
    VERIFICAR(area.indiceColuna == origem.indiceColuna);
    VERIFICAR(area.indiceLinha == origem.indiceLinha);
    VERIFICAR(area.largura == 2);
    VERIFICAR(area.altura == 2);
    VERIFICAR(Ocupacao::calcularProfundidadeDaBase(area) ==
              area.indiceColuna + area.largura - 1 + area.indiceLinha + area.altura - 1);
}

void validarIndiceDeOcupacaoDoMapa(const Mapa::MapaDaFazenda& mapa) {
    VERIFICAR(mapa.indiceDeOcupacao().quantidadeDeOcupacoes() == mapa.quantidadeDeEntidades());

    for (const Mapa::EntidadeDoMapa& entidade : mapa.entidades()) {
        if (!entidade.ehCanteiroAgricola()) {
            continue;
        }

        const Geometria::AreaNaGradeDeOcupacao area = entidade.areaDeOcupacao();
        VERIFICAR(area.largura == Constantes::UNIDADES_DE_OCUPACAO_POR_CANTEIRO);
        VERIFICAR(area.altura == Constantes::UNIDADES_DE_OCUPACAO_POR_CANTEIRO);

        for (int linha = area.indiceLinha; linha < area.indiceLinha + area.altura; ++linha) {
            for (int coluna = area.indiceColuna; coluna < area.indiceColuna + area.largura; ++coluna) {
                const Ocupacao::RegistroDeOcupacao* ocupacao =
                    mapa.indiceDeOcupacao().ocupacaoEm(Geometria::PosicaoNaGradeDeOcupacao{coluna, linha});
                VERIFICAR(ocupacao != nullptr);
                VERIFICAR(ocupacao->identificador == entidade.identificador());
                VERIFICAR(Geometria::areasDaGradeDeOcupacaoSaoIguais(ocupacao->area, area));
            }
        }
    }
}

void validarCanteiroPodeNascerEmOrigemParEImparDeOcupacao() {
    Mapa::MapaDaFazenda mapa;
    const int tamanho = Constantes::TAMANHO_INICIAL_GRID;
    const int colunaInicial = Mapa::MapaDaFazenda::calcularColunaInicialDaAreaJogavelEmOcupacao(tamanho);
    const int linhaInicial = Mapa::MapaDaFazenda::calcularLinhaInicialDaAreaJogavelEmOcupacao(tamanho);
    const Geometria::PosicaoNaGradeDeOcupacao origemPar{colunaInicial, linhaInicial};
    const Geometria::PosicaoNaGradeDeOcupacao origemImpar{colunaInicial + 3, linhaInicial + 1};

    const auto idPar = mapa.criarCanteiroEm(origemPar, tamanho);
    const auto idImpar = mapa.criarCanteiroEm(origemImpar, tamanho);

    VERIFICAR(idPar.has_value());
    VERIFICAR(idImpar.has_value());
    VERIFICAR(mapa.quantidadeDeCanteiros() == 2);
    VERIFICAR(mapa.obterEntidade(*idPar)->posicaoDoCanteiroNoMapa().has_value());
    VERIFICAR(!mapa.obterEntidade(*idImpar)->posicaoDoCanteiroNoMapa().has_value());
    VERIFICAR(Geometria::areasDaGradeDeOcupacaoSaoIguais(
        mapa.obterEntidade(*idPar)->areaDeOcupacao(),
        Ocupacao::calcularAreaDeOcupacaoDoCanteiro(origemPar)
    ));
    VERIFICAR(Geometria::areasDaGradeDeOcupacaoSaoIguais(
        mapa.obterEntidade(*idImpar)->areaDeOcupacao(),
        Ocupacao::calcularAreaDeOcupacaoDoCanteiro(origemImpar)
    ));
}

void validarConsultaDeEntidadePorCelulaDoCanteiro() {
    Mapa::MapaDaFazenda mapa;
    const int tamanho = Constantes::TAMANHO_INICIAL_GRID;
    const int colunaInicial = Mapa::MapaDaFazenda::calcularColunaInicialDaAreaJogavelEmOcupacao(tamanho);
    const int linhaInicial = Mapa::MapaDaFazenda::calcularLinhaInicialDaAreaJogavelEmOcupacao(tamanho);
    const Geometria::PosicaoNaGradeDeOcupacao origemAlinhada{colunaInicial, linhaInicial};
    const Geometria::PosicaoNaGradeDeOcupacao origemDesalinhada{colunaInicial + 3, linhaInicial + 1};

    const auto idAlinhado = mapa.criarCanteiroEm(origemAlinhada, tamanho);
    const auto idDesalinhado = mapa.criarCanteiroEm(origemDesalinhada, tamanho);

    VERIFICAR(idAlinhado.has_value());
    VERIFICAR(idDesalinhado.has_value());

    const auto verificarFootprint = [&mapa](
        Ocupacao::IdentificadorDeEntidadeDeMapa identificador,
        Geometria::PosicaoNaGradeDeOcupacao origem
    ) {
        const Geometria::AreaNaGradeDeOcupacao area = Ocupacao::calcularAreaDeOcupacaoDoCanteiro(origem);
        for (int linha = area.indiceLinha; linha < area.indiceLinha + area.altura; ++linha) {
            for (int coluna = area.indiceColuna; coluna < area.indiceColuna + area.largura; ++coluna) {
                const Mapa::EntidadeDoMapa* entidade =
                    mapa.entidadeEm(Geometria::PosicaoNaGradeDeOcupacao{coluna, linha});
                VERIFICAR(entidade != nullptr);
                VERIFICAR(entidade->identificador() == identificador);
                VERIFICAR(entidade->ehCanteiroAgricola());
            }
        }
    };

    verificarFootprint(*idAlinhado, origemAlinhada);
    verificarFootprint(*idDesalinhado, origemDesalinhada);
}

void validarSobreposicaoEContatoDeCanteirosPorOcupacao() {
    Mapa::MapaDaFazenda mapa;
    const int tamanho = Constantes::TAMANHO_INICIAL_GRID;
    const int colunaInicial = Mapa::MapaDaFazenda::calcularColunaInicialDaAreaJogavelEmOcupacao(tamanho);
    const int linhaInicial = Mapa::MapaDaFazenda::calcularLinhaInicialDaAreaJogavelEmOcupacao(tamanho);
    const Geometria::PosicaoNaGradeDeOcupacao origemBase{colunaInicial, linhaInicial};
    const Geometria::PosicaoNaGradeDeOcupacao origemSobreposta{colunaInicial + 1, linhaInicial};
    const Geometria::PosicaoNaGradeDeOcupacao origemEncostada{colunaInicial + 2, linhaInicial};

    VERIFICAR(mapa.criarCanteiroEm(origemBase, tamanho).has_value());
    VERIFICAR(!mapa.criarCanteiroEm(origemSobreposta, tamanho).has_value());
    VERIFICAR(mapa.criarCanteiroEm(origemEncostada, tamanho).has_value());
    VERIFICAR(mapa.quantidadeDeCanteiros() == 2);
}

void validarRemocaoDeCanteiroDesalinhadoLiberaAreaExata() {
    Mapa::MapaDaFazenda mapa;
    const int tamanho = Constantes::TAMANHO_INICIAL_GRID;
    const int colunaInicial = Mapa::MapaDaFazenda::calcularColunaInicialDaAreaJogavelEmOcupacao(tamanho);
    const int linhaInicial = Mapa::MapaDaFazenda::calcularLinhaInicialDaAreaJogavelEmOcupacao(tamanho);
    const Geometria::PosicaoNaGradeDeOcupacao origem{colunaInicial + 1, linhaInicial + 1};
    const Geometria::AreaNaGradeDeOcupacao area = Ocupacao::calcularAreaDeOcupacaoDoCanteiro(origem);
    const auto id = mapa.criarCanteiroEm(origem, tamanho);

    VERIFICAR(id.has_value());
    for (int linha = area.indiceLinha; linha < area.indiceLinha + area.altura; ++linha) {
        for (int coluna = area.indiceColuna; coluna < area.indiceColuna + area.largura; ++coluna) {
            const Geometria::PosicaoNaGradeDeOcupacao posicao{coluna, linha};
            const Mapa::EntidadeDoMapa* entidade = mapa.entidadeEm(posicao);
            VERIFICAR(entidade != nullptr);
            VERIFICAR(entidade->identificador() == *id);
            VERIFICAR(mapa.obterCanteiroAgricolaEm(posicao) != nullptr);
        }
    }

    VERIFICAR(mapa.removerCanteiroEm(Geometria::PosicaoNaGradeDeOcupacao{
        area.indiceColuna + 1,
        area.indiceLinha + 1
    }));

    for (int linha = area.indiceLinha; linha < area.indiceLinha + area.altura; ++linha) {
        for (int coluna = area.indiceColuna; coluna < area.indiceColuna + area.largura; ++coluna) {
            VERIFICAR(mapa.entidadeEm(Geometria::PosicaoNaGradeDeOcupacao{coluna, linha}) == nullptr);
        }
    }

    VERIFICAR(mapa.indiceDeOcupacao().quantidadeDeOcupacoes() == 0);
    VERIFICAR(mapa.quantidadeDeEntidades() == 0);
}

void validarProfundidadePorBaseDaAreaDeOcupacao() {
    VERIFICAR(Ocupacao::calcularProfundidadeDaBase(Geometria::AreaNaGradeDeOcupacao{10, 20, 1, 1}) == 30);
    VERIFICAR(Ocupacao::calcularProfundidadeDaBase(Geometria::AreaNaGradeDeOcupacao{10, 20, 2, 2}) == 32);
    VERIFICAR(Ocupacao::calcularProfundidadeDaBase(Geometria::AreaNaGradeDeOcupacao{10, 20, 4, 3}) == 35);
}

Geometria::PosicaoNaGradeDeOcupacao origemDeOcupacaoLivreProximaDoNucleo() {
    return Geometria::PosicaoNaGradeDeOcupacao{
        Constantes::COLUNA_INICIAL_DO_NUCLEO_INICIAL * Constantes::UNIDADES_DE_OCUPACAO_POR_CANTEIRO + 5,
        Constantes::LINHA_INICIAL_DO_NUCLEO_INICIAL * Constantes::UNIDADES_DE_OCUPACAO_POR_CANTEIRO
    };
}

int centroHorizontalDaArea(const Interface::AreaDeInteracao& area) {
    return area.posicaoBotaoHorizontal + area.tamanhoBotaoLargura / 2;
}

int centroVerticalDaArea(const Interface::AreaDeInteracao& area) {
    return area.posicaoBotaoVertical + area.tamanhoBotaoAltura / 2;
}

void validarContratoSemiabertoDasAreasDeInteracao() {
    const Interface::AreaDeInteracao esquerda{10, 20, 30, 40};
    const Interface::AreaDeInteracao direita{40, 20, 30, 40};

    VERIFICAR(Interface::pontoEstaNaAreaDeInteracao(10, 20, esquerda));
    VERIFICAR(Interface::pontoEstaNaAreaDeInteracao(39, 59, esquerda));
    VERIFICAR(!Interface::pontoEstaNaAreaDeInteracao(40, 20, esquerda));
    VERIFICAR(!Interface::pontoEstaNaAreaDeInteracao(10, 60, esquerda));
    VERIFICAR(!Interface::pontoEstaNaAreaDeInteracao(9, 20, esquerda));
    VERIFICAR(!Interface::pontoEstaNaAreaDeInteracao(10, 19, esquerda));

    VERIFICAR(!BarraFerramentas::verificarCliqueNoBotao(40, 20, esquerda));
    VERIFICAR(BarraFerramentas::verificarCliqueNoBotao(40, 20, direita));
    VERIFICAR(!(BarraFerramentas::verificarCliqueNoBotao(40, 20, esquerda) &&
               BarraFerramentas::verificarCliqueNoBotao(40, 20, direita)));
}

void validarBotoesDaBarraRespondemAoCliqueCentral() {
    const BarraFerramentas::BotoesDaInterface botoes = BarraFerramentas::criarBotoesDaInterface();
    using AcaoDaInterface = BarraFerramentas::AcaoDoCliqueNaInterface;

    VERIFICAR(
        botoes.areaTotalDoHud.posicaoBotaoHorizontal + botoes.areaTotalDoHud.tamanhoBotaoLargura ==
        Constantes::LARGURA_DA_JANELA - Constantes::MARGEM_DIREITA_DO_HUD_FERRAMENTAS
    );
    VERIFICAR(
        botoes.areaTotalDoHud.posicaoBotaoVertical + botoes.areaTotalDoHud.tamanhoBotaoAltura ==
        Constantes::ALTURA_DA_JANELA - Constantes::MARGEM_INFERIOR_DO_HUD_FERRAMENTAS
    );
    VERIFICAR(botoes.loja.tamanhoBotaoLargura == Constantes::LARGURA_DO_BOTAO_DA_LOJA);
    VERIFICAR(botoes.loja.tamanhoBotaoAltura == Constantes::ALTURA_DO_BOTAO_DA_LOJA);
    VERIFICAR(botoes.cursor.posicaoBotaoHorizontal ==
              botoes.loja.posicaoBotaoHorizontal + botoes.loja.tamanhoBotaoLargura +
              Constantes::ESPACAMENTO_DOS_BOTOES);
    VERIFICAR(botoes.enxada.posicaoBotaoHorizontal ==
              botoes.cursor.posicaoBotaoHorizontal + botoes.cursor.tamanhoBotaoLargura +
              Constantes::ESPACAMENTO_DOS_BOTOES);
    VERIFICAR(botoes.removerTerra.posicaoBotaoHorizontal ==
              botoes.enxada.posicaoBotaoHorizontal + botoes.enxada.tamanhoBotaoLargura +
              Constantes::ESPACAMENTO_DOS_BOTOES);
    VERIFICAR(botoes.estrela.posicaoBotaoVertical ==
              botoes.cursor.posicaoBotaoVertical + botoes.cursor.tamanhoBotaoAltura +
              Constantes::ESPACAMENTO_DOS_BOTOES);
    VERIFICAR(botoes.ajuda.posicaoBotaoVertical < botoes.cursor.posicaoBotaoVertical);
    VERIFICAR(botoes.zoomMais.posicaoBotaoVertical == botoes.ajuda.posicaoBotaoVertical);
    VERIFICAR(botoes.zoomMenos.posicaoBotaoVertical == botoes.ajuda.posicaoBotaoVertical);

    const auto verificarBotao = [&botoes](
        const Interface::AreaDeInteracao& botao,
        Ferramentas::TipoDeFerramenta ferramentaEsperada
    ) {
        Ferramentas::TipoDeFerramenta ferramentaSelecionada = Ferramentas::TipoDeFerramenta::Cursor;
        const int centroX = centroHorizontalDaArea(botao);
        const int centroY = centroVerticalDaArea(botao);

        const BarraFerramentas::ResultadoDoCliqueNaInterface resultado =
            BarraFerramentas::processarCliqueNaInterface(
                centroX,
                centroY,
                botoes,
                ferramentaSelecionada
            );
        VERIFICAR(resultado.cliqueConsumido);
        VERIFICAR(resultado.acao == AcaoDaInterface::SelecionarFerramenta);
        VERIFICAR(ferramentaSelecionada == ferramentaEsperada);
    };

    verificarBotao(botoes.cursor, Ferramentas::TipoDeFerramenta::Cursor);
    verificarBotao(botoes.enxada, Ferramentas::TipoDeFerramenta::Enxada);
    verificarBotao(botoes.removerTerra, Ferramentas::TipoDeFerramenta::RemoverTerra);

    Ferramentas::TipoDeFerramenta ferramentaSelecionada = Ferramentas::TipoDeFerramenta::Enxada;

    const auto verificarPlaceholder = [&botoes, &ferramentaSelecionada](
        const Interface::AreaDeInteracao& botao
    ) {
        const BarraFerramentas::ResultadoDoCliqueNaInterface resultado =
            BarraFerramentas::processarCliqueNaInterface(
                centroHorizontalDaArea(botao),
                centroVerticalDaArea(botao),
                botoes,
                ferramentaSelecionada
            );

        VERIFICAR(resultado.cliqueConsumido);
        VERIFICAR(resultado.acao == AcaoDaInterface::ConsumirClique);
        VERIFICAR(ferramentaSelecionada == Ferramentas::TipoDeFerramenta::Enxada);
    };

    verificarPlaceholder(botoes.ajuda);
    verificarPlaceholder(botoes.estrela);
    verificarPlaceholder(botoes.presente);
    verificarPlaceholder(botoes.foto);

    BarraFerramentas::ResultadoDoCliqueNaInterface resultado =
        BarraFerramentas::processarCliqueNaInterface(
            centroHorizontalDaArea(botoes.zoomMais),
            centroVerticalDaArea(botoes.zoomMais),
            botoes,
            ferramentaSelecionada
        );
    VERIFICAR(resultado.cliqueConsumido);
    VERIFICAR(resultado.acao == AcaoDaInterface::AumentarZoom);
    VERIFICAR(ferramentaSelecionada == Ferramentas::TipoDeFerramenta::Enxada);

    resultado = BarraFerramentas::processarCliqueNaInterface(
        centroHorizontalDaArea(botoes.zoomMenos),
        centroVerticalDaArea(botoes.zoomMenos),
        botoes,
        ferramentaSelecionada
    );
    VERIFICAR(resultado.cliqueConsumido);
    VERIFICAR(resultado.acao == AcaoDaInterface::DiminuirZoom);
    VERIFICAR(ferramentaSelecionada == Ferramentas::TipoDeFerramenta::Enxada);

    resultado = BarraFerramentas::processarCliqueNaInterface(
        botoes.loja.posicaoBotaoHorizontal + botoes.loja.tamanhoBotaoLargura +
            Constantes::ESPACAMENTO_DOS_BOTOES / 2,
        botoes.loja.posicaoBotaoVertical + 8,
        botoes,
        ferramentaSelecionada
    );
    VERIFICAR(resultado.cliqueConsumido);
    VERIFICAR(resultado.acao == AcaoDaInterface::ConsumirClique);

    resultado = BarraFerramentas::processarCliqueNaInterface(
        botoes.areaTotalDoHud.posicaoBotaoHorizontal - 1,
        botoes.areaTotalDoHud.posicaoBotaoVertical,
        botoes,
        ferramentaSelecionada
    );
    VERIFICAR(!resultado.cliqueConsumido);

    resultado = BarraFerramentas::processarCliqueNaInterface(
        centroHorizontalDaArea(botoes.loja),
        centroVerticalDaArea(botoes.loja),
        botoes,
        ferramentaSelecionada
    );
    VERIFICAR(resultado.cliqueConsumido);
    VERIFICAR(resultado.acao == AcaoDaInterface::AbrirLoja);
    VERIFICAR(ferramentaSelecionada == Ferramentas::TipoDeFerramenta::Loja);
}

void validarFluxoDeCliqueDaLoja() {
    const Plantas::FabricaDePlantas fabrica;
    const auto especiesDaLoja = fabrica.todasAsEspecies();
    const std::vector<Loja::ItemDeSementeDaLoja> itens = Loja::criarItensDeSementesDaLoja(especiesDaLoja);
    Loja::EstadoDaLoja estadoDaLoja;
    estadoDaLoja.abrir();

    Loja::LayoutCalculadoDaLoja layout = Loja::calcularLayoutDaLoja(estadoDaLoja, itens);
    VERIFICAR(estadoDaLoja.aberta());
    VERIFICAR(!itens.empty());
    VERIFICAR(!layout.cartoesDeSementes.empty());
    VERIFICAR(
        layout.painelPrincipal.posicaoBotaoHorizontal ==
        (Constantes::LARGURA_DA_JANELA - layout.painelPrincipal.tamanhoBotaoLargura) / 2
    );

    Loja::ResultadoDoCliqueDaLoja resultado = Loja::processarCliqueDaLoja(
        centroHorizontalDaArea(layout.abasPrincipais[1].area),
        centroVerticalDaArea(layout.abasPrincipais[1].area),
        estadoDaLoja,
        layout
    );
    VERIFICAR(resultado.cliqueConsumido);
    VERIFICAR(resultado.acao == Loja::AcaoDoCliqueDaLoja::TrocarAba);
    VERIFICAR(estadoDaLoja.abaPrincipalSelecionada() == Loja::AbaPrincipalDaLoja::Animais);
    VERIFICAR(estadoDaLoja.aberta());

    layout = Loja::calcularLayoutDaLoja(estadoDaLoja, itens);
    resultado = Loja::processarCliqueDaLoja(
        centroHorizontalDaArea(layout.abasPrincipais[2].area),
        centroVerticalDaArea(layout.abasPrincipais[2].area),
        estadoDaLoja,
        layout
    );
    VERIFICAR(resultado.cliqueConsumido);
    VERIFICAR(estadoDaLoja.abaPrincipalSelecionada() == Loja::AbaPrincipalDaLoja::Construcoes);

    layout = Loja::calcularLayoutDaLoja(estadoDaLoja, itens);
    resultado = Loja::processarCliqueDaLoja(
        centroHorizontalDaArea(layout.abasPrincipais[3].area),
        centroVerticalDaArea(layout.abasPrincipais[3].area),
        estadoDaLoja,
        layout
    );
    VERIFICAR(resultado.cliqueConsumido);
    VERIFICAR(estadoDaLoja.abaPrincipalSelecionada() == Loja::AbaPrincipalDaLoja::Decoracoes);

    layout = Loja::calcularLayoutDaLoja(estadoDaLoja, itens);
    resultado = Loja::processarCliqueDaLoja(
        centroHorizontalDaArea(layout.abasPrincipais[0].area),
        centroVerticalDaArea(layout.abasPrincipais[0].area),
        estadoDaLoja,
        layout
    );
    VERIFICAR(resultado.cliqueConsumido);
    VERIFICAR(estadoDaLoja.abaPrincipalSelecionada() == Loja::AbaPrincipalDaLoja::Sementes);

    for (std::size_t indice = 0; indice < layout.filtrosDeSementes.size(); ++indice) {
        layout = Loja::calcularLayoutDaLoja(estadoDaLoja, itens);
        resultado = Loja::processarCliqueDaLoja(
            centroHorizontalDaArea(layout.filtrosDeSementes[indice].area),
            centroVerticalDaArea(layout.filtrosDeSementes[indice].area),
            estadoDaLoja,
            layout
        );
        VERIFICAR(resultado.cliqueConsumido);
        VERIFICAR(resultado.acao == Loja::AcaoDoCliqueDaLoja::TrocarFiltro);
        VERIFICAR(estadoDaLoja.filtroDeSementesSelecionado() == layout.filtrosDeSementes[indice].filtro);
        VERIFICAR(estadoDaLoja.aberta());
    }

    estadoDaLoja.selecionarFiltroDeSementes(Loja::FiltroDeSementesDaLoja::Todos);
    layout = Loja::calcularLayoutDaLoja(estadoDaLoja, itens);
    const Loja::CartaoDeSementeDaLoja& cartaoMirtilo = layout.cartoesDeSementes.front();
    Loja::atualizarHoverDaLoja(
        centroHorizontalDaArea(cartaoMirtilo.area),
        centroVerticalDaArea(cartaoMirtilo.area),
        estadoDaLoja,
        layout
    );
    VERIFICAR(estadoDaLoja.identificadorDaSementeSobHover().has_value());
    VERIFICAR(*estadoDaLoja.identificadorDaSementeSobHover() == Especies::PlantaMirtilo::IDENTIFICADOR_DA_SEMENTE);

    resultado = Loja::processarCliqueDaLoja(
        centroHorizontalDaArea(cartaoMirtilo.botaoComprar),
        centroVerticalDaArea(cartaoMirtilo.botaoComprar),
        estadoDaLoja,
        layout
    );
    VERIFICAR(resultado.cliqueConsumido);
    VERIFICAR(resultado.acao == Loja::AcaoDoCliqueDaLoja::SelecionarSemente);
    VERIFICAR(resultado.identificadorDaSementeSelecionada.has_value());
    VERIFICAR(*resultado.identificadorDaSementeSelecionada == Especies::PlantaMirtilo::IDENTIFICADOR_DA_SEMENTE);
    VERIFICAR(!estadoDaLoja.aberta());

    auto jogo = AppServicos::criarEstadoInicialDoJogo();
    const Geometria::PosicaoNaGradeDeOcupacao posicaoInicialDoPersonagem =
        jogo.personagem().posicaoNaGradeDeOcupacao();
    jogo.selecionarSemente(*resultado.identificadorDaSementeSelecionada);
    jogo.selecionarFerramenta(Ferramentas::TipoDeFerramenta::Semente);
    VERIFICAR(jogo.identificadorDaSementeSelecionada().has_value());
    VERIFICAR(Geometria::posicoesDaGradeDeOcupacaoSaoIguais(
        posicaoInicialDoPersonagem,
        jogo.personagem().posicaoNaGradeDeOcupacao()
    ));

    estadoDaLoja.abrir();
    layout = Loja::calcularLayoutDaLoja(estadoDaLoja, itens);
    resultado = Loja::processarCliqueDaLoja(
        layout.botaoFechar.posicaoBotaoHorizontal,
        layout.botaoFechar.posicaoBotaoVertical,
        estadoDaLoja,
        layout
    );
    VERIFICAR(resultado.cliqueConsumido);
    VERIFICAR(resultado.acao == Loja::AcaoDoCliqueDaLoja::Fechar);
    VERIFICAR(!estadoDaLoja.aberta());

    estadoDaLoja.abrir();
    layout = Loja::calcularLayoutDaLoja(estadoDaLoja, itens);
    resultado = Loja::processarCliqueDaLoja(
        layout.painelPrincipal.posicaoBotaoHorizontal - 1,
        layout.painelPrincipal.posicaoBotaoVertical,
        estadoDaLoja,
        layout
    );
    VERIFICAR(resultado.cliqueConsumido);
    VERIFICAR(resultado.acao == Loja::AcaoDoCliqueDaLoja::Fechar);
    VERIFICAR(!estadoDaLoja.aberta());
}

void validarEstadoDaCenaFazenda() {
    Interface::EstadoDaCenaFazenda estadoDaCena;

    VERIFICAR(!estadoDaCena.painelConfiguracoesAberto());
    estadoDaCena.alternarPainelConfiguracoes();
    VERIFICAR(estadoDaCena.painelConfiguracoesAberto());
    estadoDaCena.fecharPainelConfiguracoes();
    VERIFICAR(!estadoDaCena.painelConfiguracoesAberto());

    VERIFICAR(!estadoDaCena.audioMutado());
    estadoDaCena.alternarAudioMutado();
    VERIFICAR(estadoDaCena.audioMutado());
    estadoDaCena.definirAudioMutado(false);
    VERIFICAR(!estadoDaCena.audioMutado());

}

void validarEstadoDaLoja() {
    Loja::EstadoDaLoja estadoDaLoja;

    VERIFICAR(!estadoDaLoja.aberta());
    estadoDaLoja.abrir();
    VERIFICAR(estadoDaLoja.aberta());
    VERIFICAR(estadoDaLoja.abaPrincipalSelecionada() == Loja::AbaPrincipalDaLoja::Sementes);
    VERIFICAR(estadoDaLoja.filtroDeSementesSelecionado() == Loja::FiltroDeSementesDaLoja::Todos);

    estadoDaLoja.selecionarAbaPrincipal(Loja::AbaPrincipalDaLoja::Animais);
    VERIFICAR(estadoDaLoja.abaPrincipalSelecionada() == Loja::AbaPrincipalDaLoja::Animais);

    estadoDaLoja.selecionarFiltroDeSementes(Loja::FiltroDeSementesDaLoja::Frutas);
    VERIFICAR(estadoDaLoja.filtroDeSementesSelecionado() == Loja::FiltroDeSementesDaLoja::Frutas);

    estadoDaLoja.definirSementeSobHover(Especies::PlantaMirtilo::IDENTIFICADOR_DA_SEMENTE);
    VERIFICAR(estadoDaLoja.identificadorDaSementeSobHover().has_value());
    estadoDaLoja.definirSementeSelecionadaVisualmente(Especies::PlantaMirtilo::IDENTIFICADOR_DA_SEMENTE);
    VERIFICAR(estadoDaLoja.identificadorDaSementeSelecionadaVisualmente().has_value());

    estadoDaLoja.fechar();
    VERIFICAR(!estadoDaLoja.aberta());
    VERIFICAR(!estadoDaLoja.identificadorDaSementeSobHover().has_value());
}

void validarEstadosVisuaisDePlantaParaDesenho() {
    VERIFICAR(!Assets::estadoVisualTemPlantaParaDesenho(Canteiros::EstadoVisualDoCanteiro::TerraVazia));
    VERIFICAR(!Assets::estadoVisualTemPlantaParaDesenho(Canteiros::EstadoVisualDoCanteiro::TerraArada));
    VERIFICAR(Assets::estadoVisualTemPlantaParaDesenho(Canteiros::EstadoVisualDoCanteiro::SementePlantada));
    VERIFICAR(Assets::estadoVisualTemPlantaParaDesenho(Canteiros::EstadoVisualDoCanteiro::PlantaCrescendo));
    VERIFICAR(Assets::estadoVisualTemPlantaParaDesenho(Canteiros::EstadoVisualDoCanteiro::PlantaJovem));
    VERIFICAR(Assets::estadoVisualTemPlantaParaDesenho(Canteiros::EstadoVisualDoCanteiro::PlantaMadura));
    VERIFICAR(Assets::estadoVisualTemPlantaParaDesenho(Canteiros::EstadoVisualDoCanteiro::PlantaMorta));
    VERIFICAR(!Assets::estadoVisualTemPlantaParaDesenho(Canteiros::EstadoVisualDoCanteiro::Restos));
}

void validarConsultasPurasDoCanteiroComPlanta() {
    const Especies::PlantaMirtilo mirtilo;
    Canteiros::Canteiro canteiro;

    VERIFICAR(!canteiro.possuiPlanta());
    VERIFICAR(!canteiro.nomeDaPlantaAtual().has_value());
    VERIFICAR(!canteiro.percentualDeCrescimentoAteColheita().has_value());
    VERIFICAR(canteiro.estadoLogicoDaPlantaAtual() == Canteiros::EstadoLogicoDaPlantaNoCanteiro::Ausente);

    VERIFICAR(canteiro.arar());
    VERIFICAR(!canteiro.possuiPlanta());
    VERIFICAR(canteiro.estadoLogicoDaPlantaAtual() == Canteiros::EstadoLogicoDaPlantaNoCanteiro::Ausente);

    VERIFICAR(canteiro.plantar(mirtilo.clonar()));
    VERIFICAR(canteiro.possuiPlanta());
    VERIFICAR(canteiro.nomeDaPlantaAtual().has_value());
    VERIFICAR(*canteiro.nomeDaPlantaAtual() == "Mirtilo");
    VERIFICAR(canteiro.percentualDeCrescimentoAteColheita().has_value());
    VERIFICAR(*canteiro.percentualDeCrescimentoAteColheita() == 0);
    VERIFICAR(canteiro.estadoLogicoDaPlantaAtual() == Canteiros::EstadoLogicoDaPlantaNoCanteiro::Crescendo);

    const int metadeDoTempoAteColheita = mirtilo.tempoParaMaturar() / 2;
    for (int segundo = 0; segundo < metadeDoTempoAteColheita; ++segundo) {
        VERIFICAR(canteiro.avancarUmSegundo());
    }
    VERIFICAR(*canteiro.percentualDeCrescimentoAteColheita() == 50);
    VERIFICAR(canteiro.estadoLogicoDaPlantaAtual() == Canteiros::EstadoLogicoDaPlantaNoCanteiro::Crescendo);

    for (int segundo = metadeDoTempoAteColheita; segundo < mirtilo.tempoParaMaturar(); ++segundo) {
        VERIFICAR(canteiro.avancarUmSegundo());
    }
    VERIFICAR(*canteiro.percentualDeCrescimentoAteColheita() == 100);
    VERIFICAR(canteiro.estadoLogicoDaPlantaAtual() ==
              Canteiros::EstadoLogicoDaPlantaNoCanteiro::ProntaParaColheita);

    Canteiros::Canteiro canteiroComPlantaMorta;
    VERIFICAR(canteiroComPlantaMorta.arar());
    VERIFICAR(canteiroComPlantaMorta.plantar(mirtilo.clonar()));
    for (int segundo = 0; segundo < mirtilo.tempoParaMorrer(); ++segundo) {
        VERIFICAR(canteiroComPlantaMorta.avancarUmSegundo());
    }
    VERIFICAR(canteiroComPlantaMorta.possuiPlanta());
    VERIFICAR(*canteiroComPlantaMorta.nomeDaPlantaAtual() == "Mirtilo");
    VERIFICAR(*canteiroComPlantaMorta.percentualDeCrescimentoAteColheita() == 100);
    VERIFICAR(canteiroComPlantaMorta.estadoLogicoDaPlantaAtual() ==
              Canteiros::EstadoLogicoDaPlantaNoCanteiro::Morta);

    VERIFICAR(canteiroComPlantaMorta.limparPlantaMorta());
    VERIFICAR(canteiroComPlantaMorta.estaComRestos());
    VERIFICAR(!canteiroComPlantaMorta.possuiPlanta());
    VERIFICAR(!canteiroComPlantaMorta.nomeDaPlantaAtual().has_value());
    VERIFICAR(!canteiroComPlantaMorta.percentualDeCrescimentoAteColheita().has_value());
    VERIFICAR(canteiroComPlantaMorta.estadoLogicoDaPlantaAtual() ==
              Canteiros::EstadoLogicoDaPlantaNoCanteiro::Ausente);
}

void validarAnimacaoIdleDoPersonagem() {
    auto jogo = AppServicos::criarEstadoInicialDoJogo();
    const auto& configuracaoIdle = ConfigPersonagem::configuracaoParaAnimacao(
        ConfigPersonagem::AnimacaoVisualDoPersonagem::Idle
    );
    Animacao::EstadoVisualDoPersonagem estadoVisualDoPersonagem;
    const Geometria::PosicaoNaGradeDeOcupacao posicaoInicial =
        jogo.personagem().posicaoNaGradeDeOcupacao();
    VERIFICAR(jogo.mapa().obterCanteiroAgricolaEm(posicaoInicial) != nullptr);

    VERIFICAR(configuracaoIdle.quantidadeFrames == 5);
    VERIFICAR(configuracaoIdle.frameOrigemX == 0);
    VERIFICAR(configuracaoIdle.frameOrigemY == 0);
    VERIFICAR(configuracaoIdle.frameLargura == 250);
    VERIFICAR(configuracaoIdle.frameAltura == 250);
    VERIFICAR(configuracaoIdle.frameEspacamentoX == 0);
    VERIFICAR(configuracaoIdle.destinoLargura == 63);
    VERIFICAR(configuracaoIdle.destinoAltura == 96);
    VERIFICAR(configuracaoIdle.pontoDosPesX == 32);
    VERIFICAR(configuracaoIdle.pontoDosPesY == 96);

    for (int frame = 0; frame < configuracaoIdle.quantidadeFrames; ++frame) {
        const auto origem = ConfigPersonagem::calcularRetanguloDeOrigem(configuracaoIdle, frame);
        VERIFICAR(origem.x == ConfigPersonagem::frameOrigemX + frame * (
            ConfigPersonagem::frameLargura + ConfigPersonagem::frameEspacamentoX
        ));
        VERIFICAR(origem.y == ConfigPersonagem::frameOrigemY);
        VERIFICAR(origem.w == ConfigPersonagem::frameLargura);
        VERIFICAR(origem.h == ConfigPersonagem::frameAltura);
    }

    const auto origemAposUltimoFrame =
        ConfigPersonagem::calcularRetanguloDeOrigem(configuracaoIdle, configuracaoIdle.quantidadeFrames);
    VERIFICAR(origemAposUltimoFrame.x == ConfigPersonagem::frameOrigemX);

    const Geometria::PosicaoNaTela centroDoCanteiro{64, 32};
    const auto destino = ConfigPersonagem::calcularRetanguloDeDestino(configuracaoIdle, centroDoCanteiro);
    VERIFICAR(destino.x == 32);
    VERIFICAR(destino.y == -64);
    VERIFICAR(destino.w == 63);
    VERIFICAR(destino.h == 96);
    VERIFICAR(destino.x + configuracaoIdle.pontoDosPesX == centroDoCanteiro.coordenadaHorizontal);
    VERIFICAR(destino.y + configuracaoIdle.pontoDosPesY == centroDoCanteiro.coordenadaVertical);
    VERIFICAR(destino.y + destino.h == centroDoCanteiro.coordenadaVertical);

    VERIFICAR(estadoVisualDoPersonagem.indiceFrameAtual == 0);
    AppServicos::avancarTempoDoJogo(jogo, 2.49f);
    Animacao::avancarAnimacaoDoPersonagem(estadoVisualDoPersonagem, jogo.personagem(), 2.49f);
    VERIFICAR(estadoVisualDoPersonagem.indiceFrameAtual == 0);
    VERIFICAR(Geometria::posicoesDaGradeDeOcupacaoSaoIguais(
        posicaoInicial,
        jogo.personagem().posicaoNaGradeDeOcupacao()
    ));
}

void validarSequenciasDaAnimacaoIdleDoPersonagem() {
    Animacao::AnimacaoIdleDoPersonagem animacao{7};
    VERIFICAR(animacao.indiceFrameAtual() == 0);

    animacao.iniciarPiscadaRapida(0.06f);
    VERIFICAR(animacao.indiceFrameAtual() == 0);
    animacao.avancar(0.06f);
    VERIFICAR(animacao.indiceFrameAtual() == 1);
    animacao.avancar(0.06f);
    VERIFICAR(animacao.indiceFrameAtual() == 2);
    animacao.avancar(0.06f);
    VERIFICAR(animacao.indiceFrameAtual() == 3);
    animacao.avancar(0.06f);
    VERIFICAR(animacao.indiceFrameAtual() == 4);
    animacao.avancar(0.06f);
    VERIFICAR(animacao.indiceFrameAtual() == 0);

    animacao.iniciarPiscadaDupla(0.06f);
    VERIFICAR(animacao.indiceFrameAtual() == 0);
    animacao.avancar(0.06f);
    VERIFICAR(animacao.indiceFrameAtual() == 1);
    animacao.avancar(0.06f);
    VERIFICAR(animacao.indiceFrameAtual() == 2);
    animacao.avancar(0.06f);
    VERIFICAR(animacao.indiceFrameAtual() == 3);
    animacao.avancar(0.06f);
    VERIFICAR(animacao.indiceFrameAtual() == 0);
    animacao.avancar(0.06f);
    VERIFICAR(animacao.indiceFrameAtual() == 1);
    animacao.avancar(0.06f);
    VERIFICAR(animacao.indiceFrameAtual() == 2);
    animacao.avancar(0.06f);
    VERIFICAR(animacao.indiceFrameAtual() == 3);
    animacao.avancar(0.06f);
    VERIFICAR(animacao.indiceFrameAtual() == 4);
    animacao.avancar(0.06f);
    VERIFICAR(animacao.indiceFrameAtual() == 0);

    animacao.iniciarCansaco(0.06f, 1.6f);
    VERIFICAR(animacao.indiceFrameAtual() == 0);
    animacao.avancar(0.06f);
    VERIFICAR(animacao.indiceFrameAtual() == 1);
    animacao.avancar(1.59f);
    VERIFICAR(animacao.indiceFrameAtual() == 1);
    animacao.avancar(0.01f);
    VERIFICAR(animacao.indiceFrameAtual() == 4);
    animacao.avancar(0.06f);
    VERIFICAR(animacao.indiceFrameAtual() == 0);
}

void validarMovimentoIsometricoDoPersonagem() {
    auto jogo = AppServicos::criarEstadoInicialDoJogo();
    const Geometria::PosicaoNaGradeDeOcupacao posicaoInicial =
        jogo.personagem().posicaoNaGradeDeOcupacao();
    const Geometria::PosicaoNaGradeDeOcupacao destinoPar{
        posicaoInicial.indiceColuna + 3,
        posicaoInicial.indiceLinha + 2
    };

    jogo.personagem().caminharAte(destinoPar);
    VERIFICAR(jogo.personagem().estadoAtual() == Personagem::EstadoDoPersonagem::Andando);
    VERIFICAR(jogo.personagem().direcaoAtual() == Personagem::DirecaoIsometrica::BaixoDireita);
    VERIFICAR(Animacao::animacaoVisualParaEstadoDoPersonagem(jogo.personagem()) ==
           ConfigPersonagem::AnimacaoVisualDoPersonagem::WalkBaixoDireita);

    AppServicos::avancarTempoDoJogo(
        jogo,
        (3.0f + 0.01f) / Constantes::VELOCIDADE_PERSONAGEM_EM_UNIDADES_DE_OCUPACAO_POR_SEGUNDO
    );
    VERIFICAR(jogo.personagem().estadoAtual() == Personagem::EstadoDoPersonagem::Andando);
    VERIFICAR(jogo.personagem().direcaoAtual() == Personagem::DirecaoIsometrica::BaixoEsquerda);
    VERIFICAR(Animacao::animacaoVisualParaEstadoDoPersonagem(jogo.personagem()) ==
           ConfigPersonagem::AnimacaoVisualDoPersonagem::WalkBaixoEsquerda);
    VERIFICAR(std::fabs(
        jogo.personagem().posicaoDosPesNaGradeDeOcupacao().indiceColuna -
        static_cast<float>(destinoPar.indiceColuna)
    ) <= 0.02f);
    VERIFICAR(jogo.personagem().posicaoDosPesNaGradeDeOcupacao().indiceLinha >
              static_cast<float>(posicaoInicial.indiceLinha));
    VERIFICAR(jogo.personagem().posicaoDosPesNaGradeDeOcupacao().indiceLinha <
              static_cast<float>(destinoPar.indiceLinha));

    AppServicos::avancarTempoDoJogo(jogo, 2.0f);
    VERIFICAR(jogo.personagem().estadoAtual() == Personagem::EstadoDoPersonagem::Parado);
    VERIFICAR(Animacao::animacaoVisualParaEstadoDoPersonagem(jogo.personagem()) ==
           ConfigPersonagem::AnimacaoVisualDoPersonagem::Idle);
    VERIFICAR(Geometria::posicoesDaGradeDeOcupacaoSaoIguais(
        destinoPar,
        jogo.personagem().posicaoNaGradeDeOcupacao()
    ));

    const Geometria::PosicaoNaGradeDeOcupacao destinoImpar{
        posicaoInicial.indiceColuna + 2,
        posicaoInicial.indiceLinha + 1
    };
    jogo.personagem().caminharAte(destinoImpar);
    VERIFICAR(jogo.personagem().direcaoAtual() == Personagem::DirecaoIsometrica::CimaDireita);
    AppServicos::avancarTempoDoJogo(jogo, 2.0f);
    VERIFICAR(Geometria::posicoesDaGradeDeOcupacaoSaoIguais(
        destinoImpar,
        jogo.personagem().posicaoNaGradeDeOcupacao()
    ));

    jogo.personagem().caminharAte(posicaoInicial);
    VERIFICAR(jogo.personagem().direcaoAtual() == Personagem::DirecaoIsometrica::CimaEsquerda);
    AppServicos::avancarTempoDoJogo(jogo, 2.0f);
    VERIFICAR(Geometria::posicoesDaGradeDeOcupacaoSaoIguais(
        posicaoInicial,
        jogo.personagem().posicaoNaGradeDeOcupacao()
    ));
}

void validarPosicaoInicialDoPersonagemNaOcupacao() {
    Personagem::Personagem personagem;
    const Geometria::PosicaoDeCanteiroNoMapa posicaoLegada{
        Constantes::COLUNA_CENTRAL_DA_GRADE_GLOBAL,
        Constantes::LINHA_CENTRAL_DA_GRADE_GLOBAL
    };
    const Geometria::PosicaoNaGradeDeOcupacao origemDoCanteiro =
        Ocupacao::converterCanteiroParaOcupacao(posicaoLegada);
    const Geometria::PosicaoNaGradeDeOcupacao posicaoEsperada{
        origemDoCanteiro.indiceColuna + 1,
        origemDoCanteiro.indiceLinha
    };

    VERIFICAR(Geometria::posicoesDaGradeDeOcupacaoSaoIguais(
        posicaoEsperada,
        personagem.posicaoNaGradeDeOcupacao()
    ));
    VERIFICAR(posicaoDecimalDeOcupacaoEstaProxima(
        personagem.posicaoDosPesNaGradeDeOcupacao(),
        posicaoEsperada
    ));

    MiniFazenda::Apresentacao::ConfiguracoesDoLayout configuracoes;
    Camera::aplicarOrigemCentradaDaGrade(configuracoes, Constantes::TAMANHO_INICIAL_GRID);
    Camera::EstadoDaCamera camera;
    const Camera::DimensoesDaUnidadeDeOcupacaoRenderizada unidade =
        Camera::calcularDimensoesDaUnidadeDeOcupacaoRenderizada(camera.zoomAtual);
    const Geometria::PosicaoNaTela pontoDosPesEmOcupacao =
        Isometria::converterCentroDaUnidadeDeOcupacaoGlobalParaTela(
            personagem.posicaoDosPesNaGradeDeOcupacao(),
            unidade.largura,
            unidade.altura,
            configuracoes.origemGradeHorizontal,
            configuracoes.origemGradeVertical,
            camera.offsetHorizontal,
            camera.offsetVertical
        );

    const Camera::DimensoesDoCanteiroRenderizado dimensoesDoCanteiro =
        Camera::calcularDimensoesDoCanteiroRenderizado(camera.zoomAtual);
    const Geometria::PosicaoNaTela topoDoCanteiroLegado =
        Isometria::converterGradeGlobalParaTela(
            Geometria::PosicaoNaGrade{posicaoLegada.indiceColuna, posicaoLegada.indiceLinha},
            dimensoesDoCanteiro.largura,
            dimensoesDoCanteiro.altura,
            configuracoes.origemGradeHorizontal,
            configuracoes.origemGradeVertical,
            camera.offsetHorizontal,
            camera.offsetVertical
        );
    const Geometria::PosicaoNaTela pontoDosPesLegado{
        topoDoCanteiroLegado.coordenadaHorizontal + dimensoesDoCanteiro.largura / 2,
        topoDoCanteiroLegado.coordenadaVertical + dimensoesDoCanteiro.altura / 2
    };

    VERIFICAR(pontoDosPesEmOcupacao.coordenadaHorizontal == pontoDosPesLegado.coordenadaHorizontal);
    VERIFICAR(pontoDosPesEmOcupacao.coordenadaVertical == pontoDosPesLegado.coordenadaVertical);
}

void validarCliqueNoMundoMovePersonagemParaOcupacao() {
    auto jogo = AppServicos::criarEstadoInicialDoJogo();
    MiniFazenda::Apresentacao::ConfiguracoesDoLayout configuracoes;
    Camera::aplicarOrigemCentradaDaGrade(configuracoes, jogo.tamanhoAtualDoGrid());

    Camera::EstadoDaCamera camera;
    camera.offsetHorizontal = 11;
    camera.offsetVertical = -7;
    camera.zoomAtual = 1.2f;

    const Geometria::PosicaoNaGradeDeOcupacao destino{
        Mapa::MapaDaFazenda::calcularColunaInicialDaAreaJogavelEmOcupacao(jogo.tamanhoAtualDoGrid()) + 5,
        Mapa::MapaDaFazenda::calcularLinhaInicialDaAreaJogavelEmOcupacao(jogo.tamanhoAtualDoGrid()) + 3
    };
    const Camera::DimensoesDaUnidadeDeOcupacaoRenderizada unidade =
        Camera::calcularDimensoesDaUnidadeDeOcupacaoRenderizada(camera.zoomAtual);
    const Geometria::PosicaoNaTela pontoDoClique =
        Isometria::converterCentroDaUnidadeDeOcupacaoGlobalParaTela(
            destino,
            unidade.largura,
            unidade.altura,
            configuracoes.origemGradeHorizontal,
            configuracoes.origemGradeVertical,
            camera.offsetHorizontal,
            camera.offsetVertical
        );
    const Geometria::PosicaoNaGradeDeOcupacao posicaoConvertida =
        Isometria::converterTelaParaOcupacaoGlobal(
            pontoDoClique.coordenadaHorizontal,
            pontoDoClique.coordenadaVertical,
            unidade.largura,
            unidade.altura,
            configuracoes.origemGradeHorizontal,
            configuracoes.origemGradeVertical,
            camera.offsetHorizontal,
            camera.offsetVertical
        );

    VERIFICAR(Geometria::posicoesDaGradeDeOcupacaoSaoIguais(posicaoConvertida, destino));
    VERIFICAR(Mapa::MapaDaFazenda::posicaoDeOcupacaoEstaDentroDaAreaJogavel(
        posicaoConvertida,
        jogo.tamanhoAtualDoGrid()
    ));

    jogo.personagem().caminharAte(posicaoConvertida);
    AppServicos::avancarTempoDoJogo(jogo, 10.0f);
    VERIFICAR(Geometria::posicoesDaGradeDeOcupacaoSaoIguais(
        destino,
        jogo.personagem().posicaoNaGradeDeOcupacao()
    ));
}

void validarCliqueEmInterfaceNaoMovePersonagemNoFluxo() {
    auto jogo = AppServicos::criarEstadoInicialDoJogo();
    const Geometria::PosicaoNaGradeDeOcupacao posicaoInicial =
        jogo.personagem().posicaoNaGradeDeOcupacao();
    const BarraFerramentas::BotoesDaInterface botoes = BarraFerramentas::criarBotoesDaInterface();
    Ferramentas::TipoDeFerramenta ferramentaSelecionada = jogo.ferramentaSelecionada();
    const int centroX = centroHorizontalDaArea(botoes.enxada);
    const int centroY = centroVerticalDaArea(botoes.enxada);

    const BarraFerramentas::ResultadoDoCliqueNaInterface resultado = BarraFerramentas::processarCliqueNaInterface(
        centroX,
        centroY,
        botoes,
        ferramentaSelecionada
    );

    VERIFICAR(resultado.cliqueConsumido);
    jogo.selecionarFerramenta(ferramentaSelecionada);
    VERIFICAR(Geometria::posicoesDaGradeDeOcupacaoSaoIguais(
        posicaoInicial,
        jogo.personagem().posicaoNaGradeDeOcupacao()
    ));
}

void validarHitTestIsometricoDoCanteiro() {
    constexpr int origemX = 37;
    constexpr int origemY = 91;
    const float zooms[] = {1.0f, 1.1f, 1.2f, 0.7f, 1.5f};
    const Geometria::PosicaoNaGrade posicoes[] = {
        Geometria::PosicaoNaGrade{0, 0},
        Geometria::PosicaoNaGrade{10, 0},
        Geometria::PosicaoNaGrade{-5, 7}
    };

    for (const float zoom : zooms) {
        const Camera::DimensoesDoCanteiroRenderizado dimensoes =
            Camera::calcularDimensoesDoCanteiroRenderizado(zoom);
        const int metadeDaLargura = dimensoes.largura / 2;
        const int metadeDaAltura = dimensoes.altura / 2;
        VERIFICAR(metadeDaLargura > 0);
        VERIFICAR(metadeDaAltura > 0);

        for (const Geometria::PosicaoNaGrade posicao : posicoes) {
            const Geometria::PosicaoNaTela destino = Isometria::converterGradeParaTela(
                posicao.indiceColuna,
                posicao.indiceLinha,
                dimensoes.largura,
                dimensoes.altura,
                origemX,
                origemY
            );

            const Geometria::PosicaoNaGrade topo = Isometria::converterTelaParaGrade(
                destino.coordenadaHorizontal + metadeDaLargura,
                destino.coordenadaVertical,
                dimensoes.largura,
                dimensoes.altura,
                origemX,
                origemY
            );
            VERIFICAR(Geometria::posicoesDaGradeSaoIguais(topo, posicao));

            const Geometria::PosicaoNaGrade centro = Isometria::converterTelaParaGrade(
                destino.coordenadaHorizontal + metadeDaLargura,
                destino.coordenadaVertical + metadeDaAltura,
                dimensoes.largura,
                dimensoes.altura,
                origemX,
                origemY
            );
            VERIFICAR(Geometria::posicoesDaGradeSaoIguais(centro, posicao));

            const Geometria::PosicaoNaGrade direitaInterior = Isometria::converterTelaParaGrade(
                destino.coordenadaHorizontal + 2 * metadeDaLargura - 1,
                destino.coordenadaVertical + metadeDaAltura,
                dimensoes.largura,
                dimensoes.altura,
                origemX,
                origemY
            );
            VERIFICAR(Geometria::posicoesDaGradeSaoIguais(direitaInterior, posicao));

            const Geometria::PosicaoNaGrade baixoInterior = Isometria::converterTelaParaGrade(
                destino.coordenadaHorizontal + metadeDaLargura,
                destino.coordenadaVertical + 2 * metadeDaAltura - 1,
                dimensoes.largura,
                dimensoes.altura,
                origemX,
                origemY
            );
            VERIFICAR(Geometria::posicoesDaGradeSaoIguais(baixoInterior, posicao));

            const Geometria::PosicaoNaGrade esquerdaInterior = Isometria::converterTelaParaGrade(
                destino.coordenadaHorizontal + 1,
                destino.coordenadaVertical + metadeDaAltura,
                dimensoes.largura,
                dimensoes.altura,
                origemX,
                origemY
            );
            VERIFICAR(Geometria::posicoesDaGradeSaoIguais(esquerdaInterior, posicao));
        }
    }
}

void validarHitTestIsometricoGlobalComCamera() {
    MiniFazenda::Apresentacao::ConfiguracoesDoLayout configuracoes;
    auto jogo = AppServicos::criarEstadoInicialDoJogo();
    Camera::aplicarOrigemCentradaDaGrade(configuracoes, jogo.tamanhoAtualDoGrid());

    Camera::EstadoDaCamera camera;
    camera.offsetHorizontal = 37;
    camera.offsetVertical = -19;
    camera.zoomAtual = 1.5f;

    const Camera::DimensoesDoCanteiroRenderizado dimensoes =
        Camera::calcularDimensoesDoCanteiroRenderizado(camera.zoomAtual);
    const Geometria::PosicaoNaGrade posicao{
        Constantes::COLUNA_INICIAL_DO_NUCLEO_INICIAL + 3,
        Constantes::LINHA_INICIAL_DO_NUCLEO_INICIAL + 4
    };
    const Geometria::PosicaoNaTela destino = Isometria::converterGradeGlobalParaTela(
        posicao,
        dimensoes.largura,
        dimensoes.altura,
        configuracoes.origemGradeHorizontal,
        configuracoes.origemGradeVertical,
        camera.offsetHorizontal,
        camera.offsetVertical
    );
    const Geometria::PosicaoNaGrade posicaoConvertida = Isometria::converterTelaParaGradeGlobal(
        destino.coordenadaHorizontal + dimensoes.largura / 2,
        destino.coordenadaVertical + dimensoes.altura / 2,
        dimensoes.largura,
        dimensoes.altura,
        configuracoes.origemGradeHorizontal,
        configuracoes.origemGradeVertical,
        camera.offsetHorizontal,
        camera.offsetVertical
    );

    VERIFICAR(Geometria::posicoesDaGradeSaoIguais(posicaoConvertida, posicao));
}

void validarHitTestIsometricoDeOcupacaoGlobalComCamera() {
    MiniFazenda::Apresentacao::ConfiguracoesDoLayout configuracoes;
    auto jogo = AppServicos::criarEstadoInicialDoJogo();
    Camera::aplicarOrigemCentradaDaGrade(configuracoes, jogo.tamanhoAtualDoGrid());

    Camera::EstadoDaCamera camera;
    camera.offsetHorizontal = -23;
    camera.offsetVertical = 41;
    camera.zoomAtual = 1.5f;

    const Camera::DimensoesDaUnidadeDeOcupacaoRenderizada unidade =
        Camera::calcularDimensoesDaUnidadeDeOcupacaoRenderizada(camera.zoomAtual);
    const Geometria::PosicaoNaGradeDeOcupacao posicaoAlinhada{
        (Constantes::COLUNA_INICIAL_DO_NUCLEO_INICIAL + 3) * Constantes::UNIDADES_DE_OCUPACAO_POR_CANTEIRO,
        Constantes::LINHA_INICIAL_DO_NUCLEO_INICIAL * Constantes::UNIDADES_DE_OCUPACAO_POR_CANTEIRO
    };
    const Geometria::PosicaoNaGradeDeOcupacao posicaoImpar{
        posicaoAlinhada.indiceColuna + 1,
        posicaoAlinhada.indiceLinha
    };

    const Geometria::PosicaoNaTela telaAlinhada = Isometria::converterOcupacaoGlobalParaTela(
        posicaoAlinhada,
        unidade.largura,
        unidade.altura,
        configuracoes.origemGradeHorizontal,
        configuracoes.origemGradeVertical,
        camera.offsetHorizontal,
        camera.offsetVertical
    );
    const Geometria::PosicaoNaTela telaImpar = Isometria::converterOcupacaoGlobalParaTela(
        posicaoImpar,
        unidade.largura,
        unidade.altura,
        configuracoes.origemGradeHorizontal,
        configuracoes.origemGradeVertical,
        camera.offsetHorizontal,
        camera.offsetVertical
    );

    VERIFICAR(telaImpar.coordenadaHorizontal == telaAlinhada.coordenadaHorizontal + unidade.largura / 2);
    VERIFICAR(telaImpar.coordenadaVertical == telaAlinhada.coordenadaVertical + unidade.altura / 2);

    const Geometria::PosicaoNaTela centroImpar =
        Isometria::converterCentroDaUnidadeDeOcupacaoGlobalParaTela(
            posicaoImpar,
            unidade.largura,
            unidade.altura,
            configuracoes.origemGradeHorizontal,
            configuracoes.origemGradeVertical,
            camera.offsetHorizontal,
            camera.offsetVertical
        );
    VERIFICAR(centroImpar.coordenadaHorizontal == telaImpar.coordenadaHorizontal + unidade.largura / 2);
    VERIFICAR(centroImpar.coordenadaVertical == telaImpar.coordenadaVertical + unidade.altura / 2);

    const Geometria::PosicaoNaGradeDeOcupacao posicaoConvertida = Isometria::converterTelaParaOcupacaoGlobal(
        telaImpar.coordenadaHorizontal + unidade.largura / 2,
        telaImpar.coordenadaVertical,
        unidade.largura,
        unidade.altura,
        configuracoes.origemGradeHorizontal,
        configuracoes.origemGradeVertical,
        camera.offsetHorizontal,
        camera.offsetVertical
    );
    VERIFICAR(Geometria::posicoesDaGradeDeOcupacaoSaoIguais(posicaoConvertida, posicaoImpar));

    const Geometria::PosicaoNaGradeDeOcupacao centroConvertido = Isometria::converterTelaParaOcupacaoGlobal(
        centroImpar.coordenadaHorizontal,
        centroImpar.coordenadaVertical,
        unidade.largura,
        unidade.altura,
        configuracoes.origemGradeHorizontal,
        configuracoes.origemGradeVertical,
        camera.offsetHorizontal,
        camera.offsetVertical
    );
    VERIFICAR(Geometria::posicoesDaGradeDeOcupacaoSaoIguais(centroConvertido, posicaoImpar));

    const Geometria::AreaNaGradeDeOcupacao areaDoCanteiro =
        Ocupacao::calcularAreaDeOcupacaoDoCanteiro(posicaoImpar);
    const Camera::DimensoesDoCanteiroRenderizado dimensoesDoCanteiro =
        Camera::calcularDimensoesDoCanteiroRenderizado(camera.zoomAtual);
    const Camera::DimensoesDaUnidadeDeOcupacaoRenderizada dimensoesDaArea =
        Camera::calcularDimensoesDaAreaDeOcupacaoRenderizada(areaDoCanteiro, camera.zoomAtual);
    VERIFICAR(dimensoesDaArea.largura == dimensoesDoCanteiro.largura);
    VERIFICAR(dimensoesDaArea.altura == dimensoesDoCanteiro.altura);
}

void validarConversaoDeOcupacaoComZoomsECamera() {
    MiniFazenda::Apresentacao::ConfiguracoesDoLayout configuracoes;
    auto jogo = AppServicos::criarEstadoInicialDoJogo();
    Camera::aplicarOrigemCentradaDaGrade(configuracoes, jogo.tamanhoAtualDoGrid());

    const float zooms[] = {0.5f, 0.6f, 0.7f, 1.0f, 1.1f, 1.3f, 1.7f, 1.9f, 2.0f};
    const Geometria::PosicaoNaTela offsets[] = {
        Geometria::PosicaoNaTela{0, 0},
        Geometria::PosicaoNaTela{37, -19},
        Geometria::PosicaoNaTela{-23, 41}
    };
    const Geometria::PosicaoNaGradeDeOcupacao posicoes[] = {
        Geometria::PosicaoNaGradeDeOcupacao{
            Mapa::MapaDaFazenda::calcularColunaInicialDaAreaJogavelEmOcupacao(jogo.tamanhoAtualDoGrid()) + 5,
            Mapa::MapaDaFazenda::calcularLinhaInicialDaAreaJogavelEmOcupacao(jogo.tamanhoAtualDoGrid()) + 3
        },
        Geometria::PosicaoNaGradeDeOcupacao{
            Mapa::MapaDaFazenda::calcularColunaInicialDaAreaJogavelEmOcupacao(jogo.tamanhoAtualDoGrid()) + 7,
            Mapa::MapaDaFazenda::calcularLinhaInicialDaAreaJogavelEmOcupacao(jogo.tamanhoAtualDoGrid()) + 8
        }
    };

    for (const float zoom : zooms) {
        for (const Geometria::PosicaoNaTela offset : offsets) {
            Camera::EstadoDaCamera camera;
            camera.zoomAtual = zoom;
            camera.offsetHorizontal = offset.coordenadaHorizontal;
            camera.offsetVertical = offset.coordenadaVertical;
            const Camera::DimensoesDaUnidadeDeOcupacaoRenderizada unidade =
                Camera::calcularDimensoesDaUnidadeDeOcupacaoRenderizada(camera.zoomAtual);
            const int metadeDaLargura = Isometria::calcularMetadeDaDimensaoIsometrica(unidade.largura);
            const int metadeDaAltura = Isometria::calcularMetadeDaDimensaoIsometrica(unidade.altura);

            for (const Geometria::PosicaoNaGradeDeOcupacao posicao : posicoes) {
                const Geometria::PosicaoNaTela origem =
                    Isometria::converterOcupacaoGlobalParaTela(
                        posicao,
                        unidade.largura,
                        unidade.altura,
                        configuracoes.origemGradeHorizontal,
                        configuracoes.origemGradeVertical,
                        camera.offsetHorizontal,
                        camera.offsetVertical
                    );
                const Geometria::PosicaoNaTela pontos[] = {
                    Geometria::PosicaoNaTela{origem.coordenadaHorizontal + metadeDaLargura,
                                             origem.coordenadaVertical},
                    Geometria::PosicaoNaTela{origem.coordenadaHorizontal + metadeDaLargura,
                                             origem.coordenadaVertical + metadeDaAltura},
                    Geometria::PosicaoNaTela{origem.coordenadaHorizontal + 1,
                                             origem.coordenadaVertical + metadeDaAltura},
                    Geometria::PosicaoNaTela{origem.coordenadaHorizontal + 2 * metadeDaLargura - 1,
                                             origem.coordenadaVertical + metadeDaAltura},
                    Geometria::PosicaoNaTela{origem.coordenadaHorizontal + metadeDaLargura,
                                             origem.coordenadaVertical + 2 * metadeDaAltura - 1}
                };

                for (const Geometria::PosicaoNaTela ponto : pontos) {
                    const Geometria::PosicaoNaGradeDeOcupacao convertida =
                        Isometria::converterTelaParaOcupacaoGlobal(
                            ponto.coordenadaHorizontal,
                            ponto.coordenadaVertical,
                            unidade.largura,
                            unidade.altura,
                            configuracoes.origemGradeHorizontal,
                            configuracoes.origemGradeVertical,
                            camera.offsetHorizontal,
                            camera.offsetVertical
                        );
                    VERIFICAR(Geometria::posicoesDaGradeDeOcupacaoSaoIguais(convertida, posicao));
                }
            }
        }
    }
}

void validarZoomNoCursorPreservaOcupacaoSobMouse() {
    MiniFazenda::Apresentacao::ConfiguracoesDoLayout configuracoes;
    auto jogo = AppServicos::criarEstadoInicialDoJogo();
    Camera::aplicarOrigemCentradaDaGrade(configuracoes, jogo.tamanhoAtualDoGrid());

    struct CasoDeZoomNoCursor {
        float zoomInicial;
        int passos;
        int offsetHorizontal;
        int offsetVertical;
    };

    const CasoDeZoomNoCursor casos[] = {
        CasoDeZoomNoCursor{0.5f, 1, 37, -19},
        CasoDeZoomNoCursor{0.6f, 1, 0, 0},
        CasoDeZoomNoCursor{0.7f, 1, -23, 41},
        CasoDeZoomNoCursor{1.0f, 1, 11, -7},
        CasoDeZoomNoCursor{1.1f, 1, 37, -19},
        CasoDeZoomNoCursor{1.3f, -1, -23, 41},
        CasoDeZoomNoCursor{1.7f, 1, 11, -7},
        CasoDeZoomNoCursor{1.9f, 1, 37, -19},
        CasoDeZoomNoCursor{2.0f, -1, -23, 41}
    };
    const Geometria::PosicaoNaGradeDeOcupacao posicaoSobCursor{
        Mapa::MapaDaFazenda::calcularColunaInicialDaAreaJogavelEmOcupacao(jogo.tamanhoAtualDoGrid()) + 10,
        Mapa::MapaDaFazenda::calcularLinhaInicialDaAreaJogavelEmOcupacao(jogo.tamanhoAtualDoGrid()) + 8
    };

    for (const CasoDeZoomNoCursor caso : casos) {
        Camera::EstadoDaCamera camera;
        camera.zoomAtual = caso.zoomInicial;
        camera.offsetHorizontal = caso.offsetHorizontal;
        camera.offsetVertical = caso.offsetVertical;

        const Camera::DimensoesDaUnidadeDeOcupacaoRenderizada unidadeAntes =
            Camera::calcularDimensoesDaUnidadeDeOcupacaoRenderizada(camera.zoomAtual);
        const Geometria::PosicaoNaTela pontoDoMouse =
            Isometria::converterCentroDaUnidadeDeOcupacaoGlobalParaTela(
                posicaoSobCursor,
                unidadeAntes.largura,
                unidadeAntes.altura,
                configuracoes.origemGradeHorizontal,
                configuracoes.origemGradeVertical,
                camera.offsetHorizontal,
                camera.offsetVertical
            );
        const Geometria::PosicaoNaGradeDeOcupacao antes =
            Isometria::converterTelaParaOcupacaoGlobal(
                pontoDoMouse.coordenadaHorizontal,
                pontoDoMouse.coordenadaVertical,
                unidadeAntes.largura,
                unidadeAntes.altura,
                configuracoes.origemGradeHorizontal,
                configuracoes.origemGradeVertical,
                camera.offsetHorizontal,
                camera.offsetVertical
            );

        VERIFICAR(Geometria::posicoesDaGradeDeOcupacaoSaoIguais(antes, posicaoSobCursor));
        VERIFICAR(Camera::aplicarZoomNoPonto(
            camera,
            configuracoes,
            jogo.tamanhoAtualDoGrid(),
            pontoDoMouse.coordenadaHorizontal,
            pontoDoMouse.coordenadaVertical,
            caso.passos
        ));

        const Camera::DimensoesDaUnidadeDeOcupacaoRenderizada unidadeDepois =
            Camera::calcularDimensoesDaUnidadeDeOcupacaoRenderizada(camera.zoomAtual);
        const Geometria::PosicaoNaGradeDeOcupacao depois =
            Isometria::converterTelaParaOcupacaoGlobal(
                pontoDoMouse.coordenadaHorizontal,
                pontoDoMouse.coordenadaVertical,
                unidadeDepois.largura,
                unidadeDepois.altura,
                configuracoes.origemGradeHorizontal,
                configuracoes.origemGradeVertical,
                camera.offsetHorizontal,
                camera.offsetVertical
            );
        VERIFICAR(Geometria::posicoesDaGradeDeOcupacaoSaoIguais(depois, posicaoSobCursor));
    }
}

void validarPreviewECriacaoDeCanteiroUsamMesmaOcupacaoComZoom() {
    auto jogo = AppServicos::criarEstadoInicialDoJogo();
    MiniFazenda::Apresentacao::ConfiguracoesDoLayout configuracoes;
    Camera::aplicarOrigemCentradaDaGrade(configuracoes, jogo.tamanhoAtualDoGrid());

    Camera::EstadoDaCamera camera;
    camera.zoomAtual = 1.3f;
    camera.offsetHorizontal = 37;
    camera.offsetVertical = -19;
    jogo.selecionarFerramenta(Ferramentas::TipoDeFerramenta::Enxada);

    const Geometria::PosicaoNaGradeDeOcupacao posicaoLivre = origemDeOcupacaoLivreProximaDoNucleo();
    const Camera::DimensoesDaUnidadeDeOcupacaoRenderizada unidade =
        Camera::calcularDimensoesDaUnidadeDeOcupacaoRenderizada(camera.zoomAtual);
    const Geometria::PosicaoNaTela pontoDoMouse =
        Isometria::converterCentroDaUnidadeDeOcupacaoGlobalParaTela(
            posicaoLivre,
            unidade.largura,
            unidade.altura,
            configuracoes.origemGradeHorizontal,
            configuracoes.origemGradeVertical,
            camera.offsetHorizontal,
            camera.offsetVertical
        );
    const Geometria::PosicaoNaGradeDeOcupacao posicaoDoPreview =
        Isometria::converterTelaParaOcupacaoGlobal(
            pontoDoMouse.coordenadaHorizontal,
            pontoDoMouse.coordenadaVertical,
            unidade.largura,
            unidade.altura,
            configuracoes.origemGradeHorizontal,
            configuracoes.origemGradeVertical,
            camera.offsetHorizontal,
            camera.offsetVertical
        );
    const Geometria::AreaNaGradeDeOcupacao areaDoPreview =
        Ocupacao::calcularAreaDeOcupacaoDoCanteiro(posicaoDoPreview);

    VERIFICAR(Geometria::posicoesDaGradeDeOcupacaoSaoIguais(posicaoDoPreview, posicaoLivre));
    VERIFICAR(jogo.mapa().areaEstaLivre(areaDoPreview));

    const Ferramentas::ResultadoDaFerramenta resultado =
        AppServicos::aplicarFerramentaNoJogo(jogo, posicaoDoPreview);
    const Mapa::EntidadeDoMapa* entidadeCriada = jogo.mapa().entidadeEm(posicaoDoPreview);

    VERIFICAR(resultado.acao == Ferramentas::AcaoDaFerramenta::CriarTerra);
    VERIFICAR(entidadeCriada != nullptr);
    VERIFICAR(Geometria::areasDaGradeDeOcupacaoSaoIguais(entidadeCriada->areaDeOcupacao(), areaDoPreview));
}

} // namespace

int main() {
    validarGridDeOcupacaoBasico();
    validarConversaoDeCanteiroParaOcupacao();
    validarCanteiroPodeNascerEmOrigemParEImparDeOcupacao();
    validarConsultaDeEntidadePorCelulaDoCanteiro();
    validarSobreposicaoEContatoDeCanteirosPorOcupacao();
    validarRemocaoDeCanteiroDesalinhadoLiberaAreaExata();
    validarProfundidadePorBaseDaAreaDeOcupacao();
    validarAnimacaoIdleDoPersonagem();
    validarSequenciasDaAnimacaoIdleDoPersonagem();
    validarPosicaoInicialDoPersonagemNaOcupacao();
    validarMovimentoIsometricoDoPersonagem();
    validarCliqueNoMundoMovePersonagemParaOcupacao();
    validarCliqueEmInterfaceNaoMovePersonagemNoFluxo();
    validarEstadoDaCenaFazenda();
    validarEstadoDaLoja();
    validarEstadosVisuaisDePlantaParaDesenho();
    validarConsultasPurasDoCanteiroComPlanta();
    validarContratoSemiabertoDasAreasDeInteracao();
    validarBotoesDaBarraRespondemAoCliqueCentral();
    validarFluxoDeCliqueDaLoja();
    validarHitTestIsometricoDoCanteiro();
    validarHitTestIsometricoGlobalComCamera();
    validarHitTestIsometricoDeOcupacaoGlobalComCamera();
    validarConversaoDeOcupacaoComZoomsECamera();
    validarZoomNoCursorPreservaOcupacaoSobMouse();
    validarPreviewECriacaoDeCanteiroUsamMesmaOcupacaoComZoom();

    auto jogo = AppServicos::criarEstadoInicialDoJogo();
    VERIFICAR(!jogo.identificadorDaSementeSelecionada().has_value());
    VERIFICAR(jogo.tamanhoAtualDoGrid() == Constantes::TAMANHO_INICIAL_GRID);
    VERIFICAR(jogo.mapa().quantidadeDeCanteiros() == 4);
    VERIFICAR(jogo.mapa().quantidadeDeCanteirosEmCrescimento() == 0);
    validarListasDoMapa(jogo.mapa());
    validarIndiceDeOcupacaoDoMapa(jogo.mapa());

    const Geometria::PosicaoNaGradeDeOcupacao posicaoNova = origemDeOcupacaoLivreProximaDoNucleo();
    jogo.selecionarFerramenta(Ferramentas::TipoDeFerramenta::Enxada);

    Ferramentas::ResultadoDaFerramenta resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicaoNova);
    VERIFICAR(resultado.acao == Ferramentas::AcaoDaFerramenta::CriarTerra);
    VERIFICAR(jogo.mapa().quantidadeDeCanteiros() == 5);
    validarListasDoMapa(jogo.mapa());
    validarIndiceDeOcupacaoDoMapa(jogo.mapa());

    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicaoNova);
    VERIFICAR(resultado.acao == Ferramentas::AcaoDaFerramenta::ArarTerra);

    jogo.selecionarFerramenta(Ferramentas::TipoDeFerramenta::Semente);
    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicaoNova);
    VERIFICAR(!resultado.houveMudanca());
    VERIFICAR(jogo.jogador().moedas() == 200);
    VERIFICAR(jogo.mapa().quantidadeDeCanteirosEmCrescimento() == 0);

    const Especies::PlantaMirtilo mirtilo;
    const int moedasAntesDoPlantio = jogo.jogador().moedas();
    jogo.selecionarSemente(Especies::PlantaMirtilo::IDENTIFICADOR_DA_SEMENTE);
    VERIFICAR(jogo.identificadorDaSementeSelecionada().has_value());
    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicaoNova);
    VERIFICAR(resultado.acao == Ferramentas::AcaoDaFerramenta::Plantar);
    VERIFICAR(jogo.jogador().moedas() == moedasAntesDoPlantio - mirtilo.custoEmMoedas());
    VERIFICAR(jogo.mapa().quantidadeDeCanteirosEmCrescimento() == 1);
    VERIFICAR(jogo.mapa().obterCanteiroAgricolaEm(posicaoNova)->identificadorDaSemente() ==
              Especies::PlantaMirtilo::IDENTIFICADOR_DA_SEMENTE);
    validarListasDoMapa(jogo.mapa());

    AppServicos::avancarTempoDoJogo(jogo, static_cast<float>(mirtilo.tempoParaCrescer()));
    const Canteiros::Canteiro* canteiro = jogo.mapa().obterCanteiroAgricolaEm(posicaoNova);
    VERIFICAR(canteiro != nullptr);
    VERIFICAR(canteiro->estadoVisualAtual() == Canteiros::EstadoVisualDoCanteiro::PlantaCrescendo);

    AppServicos::avancarTempoDoJogo(
        jogo,
        static_cast<float>(mirtilo.tempoParaFicarJovem() - mirtilo.tempoParaCrescer())
    );
    VERIFICAR(canteiro->estadoVisualAtual() == Canteiros::EstadoVisualDoCanteiro::PlantaJovem);

    AppServicos::avancarTempoDoJogo(
        jogo,
        static_cast<float>(mirtilo.tempoParaMaturar() - mirtilo.tempoParaFicarJovem())
    );
    VERIFICAR(canteiro->estadoVisualAtual() == Canteiros::EstadoVisualDoCanteiro::PlantaMadura);

    jogo.selecionarFerramenta(Ferramentas::TipoDeFerramenta::Cursor);
    const int moedasAntesDaColheita = jogo.jogador().moedas();
    const Plantas::RecompensaDaColheita recompensaDaColheita = mirtilo.recompensaDaColheita();
    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicaoNova);
    VERIFICAR(resultado.acao == Ferramentas::AcaoDaFerramenta::Colher);
    VERIFICAR(jogo.jogador().moedas() == moedasAntesDaColheita + recompensaDaColheita.moedas);
    VERIFICAR(jogo.jogador().experiencia() == recompensaDaColheita.experiencia);
    VERIFICAR(jogo.mapa().quantidadeDeCanteirosEmCrescimento() == 0);
    VERIFICAR(canteiro->estadoVisualAtual() == Canteiros::EstadoVisualDoCanteiro::Restos);
    validarListasDoMapa(jogo.mapa());

    jogo.selecionarFerramenta(Ferramentas::TipoDeFerramenta::Semente);
    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicaoNova);
    VERIFICAR(!resultado.houveMudanca());
    VERIFICAR(canteiro->estadoVisualAtual() == Canteiros::EstadoVisualDoCanteiro::Restos);

    jogo.selecionarFerramenta(Ferramentas::TipoDeFerramenta::Enxada);
    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicaoNova);
    VERIFICAR(resultado.acao == Ferramentas::AcaoDaFerramenta::LimparCanteiro);
    VERIFICAR(canteiro->estadoVisualAtual() == Canteiros::EstadoVisualDoCanteiro::TerraVazia);

    jogo.selecionarFerramenta(Ferramentas::TipoDeFerramenta::Semente);
    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicaoNova);
    VERIFICAR(!resultado.houveMudanca());
    VERIFICAR(canteiro->estadoVisualAtual() == Canteiros::EstadoVisualDoCanteiro::TerraVazia);

    jogo.selecionarFerramenta(Ferramentas::TipoDeFerramenta::Enxada);
    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicaoNova);
    VERIFICAR(resultado.acao == Ferramentas::AcaoDaFerramenta::ArarTerra);
    VERIFICAR(canteiro->estadoVisualAtual() == Canteiros::EstadoVisualDoCanteiro::TerraArada);

    jogo.selecionarFerramenta(Ferramentas::TipoDeFerramenta::Semente);
    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicaoNova);
    VERIFICAR(resultado.acao == Ferramentas::AcaoDaFerramenta::Plantar);
    AppServicos::avancarTempoDoJogo(jogo, static_cast<float>(mirtilo.tempoParaMorrer()));
    VERIFICAR(canteiro->estadoVisualAtual() == Canteiros::EstadoVisualDoCanteiro::PlantaMorta);
    VERIFICAR(jogo.mapa().quantidadeDeCanteirosEmCrescimento() == 0);
    validarListasDoMapa(jogo.mapa());

    const int moedasAntesDeLimparPlantaMorta = jogo.jogador().moedas();
    const int experienciaAntesDeLimparPlantaMorta = jogo.jogador().experiencia();
    jogo.selecionarFerramenta(Ferramentas::TipoDeFerramenta::Enxada);
    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicaoNova);
    VERIFICAR(resultado.acao == Ferramentas::AcaoDaFerramenta::LimparCanteiro);
    VERIFICAR(jogo.jogador().moedas() == moedasAntesDeLimparPlantaMorta);
    VERIFICAR(jogo.jogador().experiencia() == experienciaAntesDeLimparPlantaMorta);
    VERIFICAR(canteiro->estadoVisualAtual() == Canteiros::EstadoVisualDoCanteiro::Restos);
    validarListasDoMapa(jogo.mapa());

    jogo.selecionarFerramenta(Ferramentas::TipoDeFerramenta::Semente);
    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicaoNova);
    VERIFICAR(!resultado.houveMudanca());
    VERIFICAR(canteiro->estadoVisualAtual() == Canteiros::EstadoVisualDoCanteiro::Restos);

    jogo.selecionarFerramenta(Ferramentas::TipoDeFerramenta::Enxada);
    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicaoNova);
    VERIFICAR(resultado.acao == Ferramentas::AcaoDaFerramenta::LimparCanteiro);
    VERIFICAR(canteiro->estadoVisualAtual() == Canteiros::EstadoVisualDoCanteiro::TerraVazia);

    jogo.selecionarFerramenta(Ferramentas::TipoDeFerramenta::Semente);
    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicaoNova);
    VERIFICAR(!resultado.houveMudanca());
    VERIFICAR(canteiro->estadoVisualAtual() == Canteiros::EstadoVisualDoCanteiro::TerraVazia);

    jogo.selecionarFerramenta(Ferramentas::TipoDeFerramenta::Enxada);
    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicaoNova);
    VERIFICAR(resultado.acao == Ferramentas::AcaoDaFerramenta::ArarTerra);
    VERIFICAR(canteiro->estadoVisualAtual() == Canteiros::EstadoVisualDoCanteiro::TerraArada);

    const std::size_t quantidadeAntesDeRemover = jogo.mapa().quantidadeDeCanteiros();
    jogo.selecionarFerramenta(Ferramentas::TipoDeFerramenta::RemoverTerra);
    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicaoNova);
    VERIFICAR(resultado.acao == Ferramentas::AcaoDaFerramenta::RemoverTerra);
    VERIFICAR(jogo.mapa().quantidadeDeCanteiros() == quantidadeAntesDeRemover - 1);
    VERIFICAR(jogo.mapa().indiceDeOcupacao().quantidadeDeOcupacoes() == jogo.mapa().quantidadeDeEntidades());
    VERIFICAR(!jogo.mapa().existeCanteiroEm(posicaoNova));
    validarListasDoMapa(jogo.mapa());
    validarIndiceDeOcupacaoDoMapa(jogo.mapa());

    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, Geometria::PosicaoNaGradeDeOcupacao{-1, -1});
    VERIFICAR(!resultado.houveMudanca());

    const Geometria::PosicaoNaGradeDeOcupacao foraDaGradeJogavel{
        Mapa::MapaDaFazenda::calcularColunaInicialDaAreaJogavelEmOcupacao(jogo.tamanhoAtualDoGrid()) - 1,
        Mapa::MapaDaFazenda::calcularLinhaInicialDaAreaJogavelEmOcupacao(jogo.tamanhoAtualDoGrid())
    };
    jogo.selecionarFerramenta(Ferramentas::TipoDeFerramenta::Enxada);
    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, foraDaGradeJogavel);
    VERIFICAR(!resultado.houveMudanca());

    MiniFazenda::Apresentacao::ConfiguracoesDoLayout configuracoes;
    Camera::aplicarOrigemCentradaDaGrade(configuracoes, jogo.tamanhoAtualDoGrid());
    Camera::EstadoDaCamera camera;
    Camera::RetanguloDeGradeRenderizada retangulo = Camera::calcularRetanguloDaGradeRenderizada(
        configuracoes,
        camera,
        jogo.tamanhoAtualDoGrid()
    );
    VERIFICAR(retangulo.x + retangulo.largura / 2 == configuracoes.centroVisualBackgroundX);
    VERIFICAR(retangulo.y + retangulo.altura / 2 == configuracoes.centroVisualBackgroundY);

    Camera::aplicarOrigemCentradaDaGrade(configuracoes, Constantes::TAMANHO_MAXIMO_GRID);
    retangulo = Camera::calcularRetanguloDaGradeRenderizada(
        configuracoes,
        camera,
        Constantes::TAMANHO_MAXIMO_GRID
    );
    VERIFICAR(retangulo.x + retangulo.largura / 2 == configuracoes.centroVisualBackgroundX);
    VERIFICAR(retangulo.y + retangulo.altura / 2 == configuracoes.centroVisualBackgroundY);

    const bool zoomAplicado = Camera::aplicarZoomNoPonto(
        camera,
        configuracoes,
        jogo.tamanhoAtualDoGrid(),
        Constantes::LARGURA_DA_JANELA / 2,
        Constantes::ALTURA_DA_JANELA / 2,
        1
    );
    VERIFICAR(zoomAplicado);
    VERIFICAR(camera.zoomAtual > Constantes::ZOOM_INICIAL);
    Camera::centralizarCamera(camera);
    VERIFICAR(camera.zoomAtual == Constantes::ZOOM_INICIAL);
    VERIFICAR(camera.offsetHorizontal == 0);
    VERIFICAR(camera.offsetVertical == 0);

    return 0;
}
