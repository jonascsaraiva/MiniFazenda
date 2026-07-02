#include "Aplicacao/Servicos/InicializadorDaFazenda.hpp"
#include "Aplicacao/Servicos/ServicoDeFerramentas.hpp"
#include "Aplicacao/Servicos/ServicoDeTempo.hpp"
#include "Apresentacao/Animacao/AnimadorDoPersonagem.hpp"
#include "Apresentacao/Camera/CameraDoJogo.hpp"
#include "Apresentacao/ConfiguracoesDoLayout.hpp"
#include "Apresentacao/Interface/BarraDeFerramentas/BarraDeFerramentas.hpp"
#include "Apresentacao/Interface/EstadoDaCenaFazenda.hpp"
#include "Apresentacao/Isometria/Isometrico.hpp"
#include "Compartilhado/Constantes.hpp"
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

void validarListasDoMapa(const Mapa::MapaDaFazenda& mapa) {
    VERIFICAR(mapa.entidades().size() == mapa.quantidadeDeEntidades());
    VERIFICAR(mapa.indiceDeOcupacao().quantidadeDeOcupacoes() == mapa.quantidadeDeEntidades());

    for (const Mapa::EntidadeDoMapa& entidade : mapa.entidades()) {
        VERIFICAR(entidade.identificador() != Ocupacao::IDENTIFICADOR_INVALIDO_DE_ENTIDADE_DE_MAPA);
        VERIFICAR(mapa.obterEntidade(entidade.identificador()) != nullptr);

        if (entidade.ehCanteiroAgricola()) {
            VERIFICAR(entidade.posicaoDoCanteiroNoMapa().has_value());
            VERIFICAR(entidade.canteiroAgricola() != nullptr);
            const Mapa::EntidadeDoMapa* entidadeNoMapa =
                mapa.entidadeEmCanteiro(*entidade.posicaoDoCanteiroNoMapa());
            VERIFICAR(entidadeNoMapa != nullptr);
            VERIFICAR(entidadeNoMapa->identificador() == entidade.identificador());
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

        VERIFICAR(entidade.posicaoDoCanteiroNoMapa().has_value());
        const Geometria::AreaNaGradeDeOcupacao area =
            Ocupacao::calcularAreaDeOcupacaoDoCanteiro(*entidade.posicaoDoCanteiroNoMapa());
        VERIFICAR(Geometria::areasDaGradeDeOcupacaoSaoIguais(entidade.areaDeOcupacao(), area));

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

Geometria::PosicaoDeCanteiroNoMapa posicaoLivreProximaDoNucleo() {
    return Geometria::PosicaoDeCanteiroNoMapa{
        Constantes::COLUNA_INICIAL_DO_NUCLEO_INICIAL + 3,
        Constantes::LINHA_INICIAL_DO_NUCLEO_INICIAL
    };
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

    const auto verificarBotao = [&botoes](
        const Interface::AreaDeInteracao& botao,
        Ferramentas::TipoDeFerramenta ferramentaEsperada
    ) {
        Ferramentas::TipoDeFerramenta ferramentaSelecionada = Ferramentas::TipoDeFerramenta::Cursor;
        Interface::EstadoDaCenaFazenda estadoDaCena;
        const int centroX = botao.posicaoBotaoHorizontal + botao.tamanhoBotaoLargura / 2;
        const int centroY = botao.posicaoBotaoVertical + botao.tamanhoBotaoAltura / 2;

        VERIFICAR(BarraFerramentas::processarCliqueNaInterface(
            centroX,
            centroY,
            botoes,
            ferramentaSelecionada,
            estadoDaCena
        ));
        VERIFICAR(ferramentaSelecionada == ferramentaEsperada);
    };

    verificarBotao(botoes.cursor, Ferramentas::TipoDeFerramenta::Cursor);
    verificarBotao(botoes.enxada, Ferramentas::TipoDeFerramenta::Enxada);
    verificarBotao(botoes.removerTerra, Ferramentas::TipoDeFerramenta::RemoverTerra);
    verificarBotao(botoes.semente, Ferramentas::TipoDeFerramenta::Semente);
    verificarBotao(botoes.loja, Ferramentas::TipoDeFerramenta::Loja);
}

void validarFluxoDeCliqueDaLoja() {
    const Plantas::FabricaDePlantas fabrica;
    const auto especiesDaLoja = fabrica.todasAsEspecies();
    const BarraFerramentas::BotoesDaInterface botoes = BarraFerramentas::criarBotoesDaInterface();
    const BarraFerramentas::PainelDaLoja painel = BarraFerramentas::criarPainelDaLoja(botoes, especiesDaLoja);

    Ferramentas::TipoDeFerramenta ferramentaSelecionada = Ferramentas::TipoDeFerramenta::Cursor;
    Interface::EstadoDaCenaFazenda estadoDaCena;
    const int xDaLoja = botoes.loja.posicaoBotaoHorizontal + botoes.loja.tamanhoBotaoLargura / 2;
    const int yDaLoja = botoes.loja.posicaoBotaoVertical + botoes.loja.tamanhoBotaoAltura / 2;
    const bool clicouNaLoja =
        BarraFerramentas::processarCliqueNaInterface(xDaLoja, yDaLoja, botoes, ferramentaSelecionada, estadoDaCena);

    VERIFICAR(clicouNaLoja);
    VERIFICAR(estadoDaCena.painelDaLojaAberto());
    VERIFICAR(ferramentaSelecionada == Ferramentas::TipoDeFerramenta::Loja);
    VERIFICAR(!painel.opcoes.empty());

    const BarraFerramentas::OpcaoDeSementeDaLoja& opcaoMirtilo = painel.opcoes.front();
    const int xDaSemente = opcaoMirtilo.area.posicaoBotaoHorizontal + opcaoMirtilo.area.tamanhoBotaoLargura / 2;
    const int yDaSemente = opcaoMirtilo.area.posicaoBotaoVertical + opcaoMirtilo.area.tamanhoBotaoAltura / 2;
    const auto sementeClicada = BarraFerramentas::sementeClicadaNoPainelDaLoja(xDaSemente, yDaSemente, painel);

    VERIFICAR(sementeClicada.has_value());
    VERIFICAR(*sementeClicada == Especies::PlantaMirtilo::IDENTIFICADOR_DA_SEMENTE);
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

    VERIFICAR(!estadoDaCena.painelDaLojaAberto());
    estadoDaCena.alternarPainelDaLoja();
    VERIFICAR(estadoDaCena.painelDaLojaAberto());
    estadoDaCena.fecharPainelDaLoja();
    VERIFICAR(!estadoDaCena.painelDaLojaAberto());
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

void validarAnimacaoIdleDoPersonagem() {
    auto jogo = AppServicos::criarEstadoInicialDoJogo();
    const auto& configuracaoIdle = ConfigPersonagem::configuracaoParaAnimacao(
        ConfigPersonagem::AnimacaoVisualDoPersonagem::Idle
    );
    Animacao::EstadoVisualDoPersonagem estadoVisualDoPersonagem;
    const Geometria::PosicaoNaGrade posicaoInicial = jogo.personagem().posicaoNaGrade();
    const Geometria::PosicaoDeCanteiroNoMapa canteiroDoPersonagem =
        Geometria::converterPosicaoNaGradeParaCanteiroNoMapa(posicaoInicial);
    VERIFICAR(jogo.mapa().obterCanteiroAgricola(canteiroDoPersonagem) != nullptr);

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
    VERIFICAR(Geometria::posicoesDaGradeSaoIguais(posicaoInicial, jogo.personagem().posicaoNaGrade()));
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
    const Geometria::PosicaoNaGrade posicaoInicial = jogo.personagem().posicaoNaGrade();
    const Geometria::PosicaoNaGrade destino{
        posicaoInicial.indiceColuna + 2,
        posicaoInicial.indiceLinha + 1
    };

    jogo.personagem().caminharAte(destino);
    VERIFICAR(jogo.personagem().estadoAtual() == Personagem::EstadoDoPersonagem::Andando);
    VERIFICAR(jogo.personagem().direcaoAtual() == Personagem::DirecaoIsometrica::BaixoDireita);
    VERIFICAR(Animacao::animacaoVisualParaEstadoDoPersonagem(jogo.personagem()) ==
           ConfigPersonagem::AnimacaoVisualDoPersonagem::WalkBaixoDireita);

    AppServicos::avancarTempoDoJogo(
        jogo,
        (2.0f + 0.01f) / Constantes::VELOCIDADE_PERSONAGEM_EM_CELULAS_POR_SEGUNDO
    );
    VERIFICAR(jogo.personagem().estadoAtual() == Personagem::EstadoDoPersonagem::Andando);
    VERIFICAR(jogo.personagem().direcaoAtual() == Personagem::DirecaoIsometrica::BaixoEsquerda);
    VERIFICAR(Animacao::animacaoVisualParaEstadoDoPersonagem(jogo.personagem()) ==
           ConfigPersonagem::AnimacaoVisualDoPersonagem::WalkBaixoEsquerda);

    AppServicos::avancarTempoDoJogo(jogo, 2.0f);
    VERIFICAR(jogo.personagem().estadoAtual() == Personagem::EstadoDoPersonagem::Parado);
    VERIFICAR(Animacao::animacaoVisualParaEstadoDoPersonagem(jogo.personagem()) ==
           ConfigPersonagem::AnimacaoVisualDoPersonagem::Idle);
    VERIFICAR(Geometria::posicoesDaGradeSaoIguais(destino, jogo.personagem().posicaoNaGrade()));

    jogo.personagem().caminharAte(posicaoInicial);
    VERIFICAR(jogo.personagem().direcaoAtual() == Personagem::DirecaoIsometrica::CimaEsquerda);
    AppServicos::avancarTempoDoJogo(jogo, 2.0f);
    VERIFICAR(Geometria::posicoesDaGradeSaoIguais(posicaoInicial, jogo.personagem().posicaoNaGrade()));
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

} // namespace

int main() {
    validarGridDeOcupacaoBasico();
    validarConversaoDeCanteiroParaOcupacao();
    validarAnimacaoIdleDoPersonagem();
    validarSequenciasDaAnimacaoIdleDoPersonagem();
    validarMovimentoIsometricoDoPersonagem();
    validarEstadoDaCenaFazenda();
    validarEstadosVisuaisDePlantaParaDesenho();
    validarContratoSemiabertoDasAreasDeInteracao();
    validarBotoesDaBarraRespondemAoCliqueCentral();
    validarFluxoDeCliqueDaLoja();
    validarHitTestIsometricoDoCanteiro();
    validarHitTestIsometricoGlobalComCamera();

    auto jogo = AppServicos::criarEstadoInicialDoJogo();
    VERIFICAR(!jogo.identificadorDaSementeSelecionada().has_value());
    VERIFICAR(jogo.tamanhoAtualDoGrid() == Constantes::TAMANHO_INICIAL_GRID);
    VERIFICAR(jogo.mapa().quantidadeDeCanteiros() == 4);
    VERIFICAR(jogo.mapa().quantidadeDeCanteirosEmCrescimento() == 0);
    validarListasDoMapa(jogo.mapa());
    validarIndiceDeOcupacaoDoMapa(jogo.mapa());

    const Geometria::PosicaoDeCanteiroNoMapa posicaoNova = posicaoLivreProximaDoNucleo();
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
    VERIFICAR(jogo.mapa().obterCanteiroAgricola(posicaoNova)->identificadorDaSemente() ==
              Especies::PlantaMirtilo::IDENTIFICADOR_DA_SEMENTE);
    validarListasDoMapa(jogo.mapa());

    AppServicos::avancarTempoDoJogo(jogo, static_cast<float>(mirtilo.tempoParaCrescer()));
    const Canteiros::Canteiro* canteiro = jogo.mapa().obterCanteiroAgricola(posicaoNova);
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

    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, Geometria::PosicaoDeCanteiroNoMapa{-1, -1});
    VERIFICAR(!resultado.houveMudanca());

    const Geometria::PosicaoDeCanteiroNoMapa foraDaGradeJogavel{
        Mapa::MapaDaFazenda::calcularColunaInicialDaAreaJogavel(jogo.tamanhoAtualDoGrid()) - 1,
        Mapa::MapaDaFazenda::calcularLinhaInicialDaAreaJogavel(jogo.tamanhoAtualDoGrid())
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
