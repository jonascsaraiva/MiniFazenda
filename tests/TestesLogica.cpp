#include "Aplicacao/Servicos/InicializadorDaFazenda.hpp"
#include "Aplicacao/Servicos/ServicoDeFerramentas.hpp"
#include "Aplicacao/Servicos/ServicoDeTempo.hpp"
#include "Apresentacao/Camera/CameraDoJogo.hpp"
#include "Apresentacao/ConfiguracoesDoLayout.hpp"
#include "Apresentacao/Interface/BarraDeFerramentas/BarraDeFerramentas.hpp"
#include "Apresentacao/Isometria/Isometrico.hpp"
#include "Compartilhado/Constantes.hpp"
#include "Compartilhado/Geometria/Posicoes.hpp"
#include "Dominio/Animacao/AnimacaoIdle.hpp"
#include "Dominio/Canteiros/EstadoDoCanteiro.hpp"
#include "Dominio/Ferramentas/ResultadoDaFerramenta.hpp"
#include "Dominio/Ferramentas/TipoDeFerramenta.hpp"
#include "Dominio/Grade/GradeGlobalDeCanteiros.hpp"
#include "Dominio/Personagem/Personagem.hpp"
#include "Dominio/Plantas/FabricaDePlantas.hpp"
#include "Dominio/Plantas/Especies/PlantaMirtilo.hpp"
#include "Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp"

#include <cassert>
#include <cstddef>

namespace {

namespace AppServicos = MiniFazenda::Aplicacao::Servicos;
namespace Animacao = MiniFazenda::Dominio::Animacao;
namespace BarraFerramentas = MiniFazenda::Apresentacao::Interface::BarraDeFerramentas;
namespace Camera = MiniFazenda::Apresentacao::Camera;
namespace Canteiros = MiniFazenda::Dominio::Canteiros;
namespace ConfigPersonagem = MiniFazenda::Infraestrutura::Assets::ConfigVisualDoPersonagem;
namespace Constantes = MiniFazenda::Compartilhado::Constantes;
namespace Especies = MiniFazenda::Dominio::Plantas::Especies;
namespace Ferramentas = MiniFazenda::Dominio::Ferramentas;
namespace Geometria = MiniFazenda::Compartilhado::Geometria;
namespace Grade = MiniFazenda::Dominio::Grade;
namespace Isometria = MiniFazenda::Apresentacao::Isometria;
namespace Personagem = MiniFazenda::Dominio::Personagem;
namespace Plantas = MiniFazenda::Dominio::Plantas;

void validarListasDaGrade(const Grade::GradeGlobalDeCanteiros& grade) {
    assert(grade.posicoesDeTilesExistentes().size() == grade.quantidadeDeTilesExistentes());
    assert(grade.posicoesDeCanteirosEmCrescimento().size() == grade.quantidadeDeCanteirosEmCrescimento());

    for (const Geometria::PosicaoNaGrade posicao : grade.posicoesDeTilesExistentes()) {
        const Grade::TileDeTerra* tile = grade.obterTile(posicao);
        assert(tile != nullptr);
        assert(tile->existeNoMapa());
    }

    for (const Geometria::PosicaoNaGrade posicao : grade.posicoesDeCanteirosEmCrescimento()) {
        const Grade::TileDeTerra* tile = grade.obterTile(posicao);
        assert(tile != nullptr);
        assert(tile->existeNoMapa());
        assert(tile->canteiro().precisaAvancarCrescimento());
    }
}

Geometria::PosicaoNaGrade posicaoLivreProximaDoNucleo() {
    return Geometria::PosicaoNaGrade{
        Constantes::COLUNA_INICIAL_DO_NUCLEO_INICIAL + 3,
        Constantes::LINHA_INICIAL_DO_NUCLEO_INICIAL
    };
}

void ararEPlantar(MiniFazenda::Aplicacao::Estado::EstadoDoJogo& jogo, Geometria::PosicaoNaGrade posicao) {
    jogo.selecionarFerramenta(Ferramentas::TipoDeFerramenta::Enxada);
    Ferramentas::ResultadoDaFerramenta resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicao);
    assert(resultado.acao == Ferramentas::AcaoDaFerramenta::ArarTerra);

    jogo.selecionarSemente(Especies::PlantaMirtilo::IDENTIFICADOR_DA_SEMENTE);
    jogo.selecionarFerramenta(Ferramentas::TipoDeFerramenta::Semente);
    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicao);
    assert(resultado.acao == Ferramentas::AcaoDaFerramenta::Plantar);
}

void validarFluxoDeCliqueDaLoja() {
    const Plantas::FabricaDePlantas fabrica;
    const auto especiesDaLoja = fabrica.todasAsEspecies();
    const BarraFerramentas::BotoesDaInterface botoes = BarraFerramentas::criarBotoesDaInterface();
    const BarraFerramentas::PainelDaLoja painel = BarraFerramentas::criarPainelDaLoja(botoes, especiesDaLoja);

    Ferramentas::TipoDeFerramenta ferramentaSelecionada = Ferramentas::TipoDeFerramenta::Cursor;
    bool lojaAberta = false;
    const int xDaLoja = botoes.loja.posicaoBotaoHorizontal + botoes.loja.tamanhoBotaoLargura / 2;
    const int yDaLoja = botoes.loja.posicaoBotaoVertical + botoes.loja.tamanhoBotaoAltura / 2;
    const bool clicouNaLoja =
        BarraFerramentas::processarCliqueNaInterface(xDaLoja, yDaLoja, botoes, ferramentaSelecionada, lojaAberta);

    assert(clicouNaLoja);
    assert(lojaAberta);
    assert(ferramentaSelecionada == Ferramentas::TipoDeFerramenta::Loja);
    assert(!painel.opcoes.empty());

    const BarraFerramentas::OpcaoDeSementeDaLoja& opcaoMirtilo = painel.opcoes.front();
    const int xDaSemente = opcaoMirtilo.area.posicaoBotaoHorizontal + opcaoMirtilo.area.tamanhoBotaoLargura / 2;
    const int yDaSemente = opcaoMirtilo.area.posicaoBotaoVertical + opcaoMirtilo.area.tamanhoBotaoAltura / 2;
    const auto sementeClicada = BarraFerramentas::sementeClicadaNoPainelDaLoja(xDaSemente, yDaSemente, painel);

    assert(sementeClicada.has_value());
    assert(*sementeClicada == Especies::PlantaMirtilo::IDENTIFICADOR_DA_SEMENTE);
}

void validarAnimacaoIdleDoPersonagem() {
    auto jogo = AppServicos::criarEstadoInicialDoJogo();
    const auto& configuracaoIdle = ConfigPersonagem::configuracaoParaAnimacao(
        Personagem::AnimacaoVisualDoPersonagem::Idle
    );
    const Geometria::PosicaoNaGrade posicaoInicial = jogo.personagem().posicaoNaGrade();
    const Grade::TileDeTerra* tileDoPersonagem = jogo.grade().obterTile(posicaoInicial);
    assert(tileDoPersonagem != nullptr);
    assert(tileDoPersonagem->existeNoMapa());

    assert(configuracaoIdle.quantidadeFrames == 5);
    assert(configuracaoIdle.frameOrigemX == 0);
    assert(configuracaoIdle.frameOrigemY == 0);
    assert(configuracaoIdle.frameLargura == 250);
    assert(configuracaoIdle.frameAltura == 250);
    assert(configuracaoIdle.frameEspacamentoX == 0);
    assert(configuracaoIdle.destinoLargura == 63);
    assert(configuracaoIdle.destinoAltura == 96);
    assert(configuracaoIdle.pontoDosPesX == 32);
    assert(configuracaoIdle.pontoDosPesY == 96);

    for (int frame = 0; frame < configuracaoIdle.quantidadeFrames; ++frame) {
        const auto origem = ConfigPersonagem::calcularRetanguloDeOrigem(configuracaoIdle, frame);
        assert(origem.x == ConfigPersonagem::frameOrigemX + frame * (
            ConfigPersonagem::frameLargura + ConfigPersonagem::frameEspacamentoX
        ));
        assert(origem.y == ConfigPersonagem::frameOrigemY);
        assert(origem.w == ConfigPersonagem::frameLargura);
        assert(origem.h == ConfigPersonagem::frameAltura);
    }

    const auto origemAposUltimoFrame =
        ConfigPersonagem::calcularRetanguloDeOrigem(configuracaoIdle, configuracaoIdle.quantidadeFrames);
    assert(origemAposUltimoFrame.x == ConfigPersonagem::frameOrigemX);

    const Geometria::PosicaoNaTela centroDoCanteiro{64, 32};
    const auto destino = ConfigPersonagem::calcularRetanguloDeDestino(configuracaoIdle, centroDoCanteiro);
    assert(destino.x == 32);
    assert(destino.y == -64);
    assert(destino.w == 63);
    assert(destino.h == 96);
    assert(destino.x + configuracaoIdle.pontoDosPesX == centroDoCanteiro.coordenadaHorizontal);
    assert(destino.y + configuracaoIdle.pontoDosPesY == centroDoCanteiro.coordenadaVertical);
    assert(destino.y + destino.h == centroDoCanteiro.coordenadaVertical);

    assert(jogo.personagem().indiceFrameDaAnimacaoVisualAtual() == 0);
    AppServicos::avancarTempoDoJogo(jogo, 2.49f);
    assert(jogo.personagem().indiceFrameDaAnimacaoVisualAtual() == 0);
    assert(Geometria::posicoesDaGradeSaoIguais(posicaoInicial, jogo.personagem().posicaoNaGrade()));
}

void validarSequenciasDaAnimacaoIdleDoPersonagem() {
    Animacao::AnimacaoIdle animacao{7};
    assert(animacao.indiceFrameAtual() == 0);

    animacao.iniciarPiscadaRapida(0.06f);
    assert(animacao.indiceFrameAtual() == 0);
    animacao.avancar(0.06f);
    assert(animacao.indiceFrameAtual() == 1);
    animacao.avancar(0.06f);
    assert(animacao.indiceFrameAtual() == 2);
    animacao.avancar(0.06f);
    assert(animacao.indiceFrameAtual() == 3);
    animacao.avancar(0.06f);
    assert(animacao.indiceFrameAtual() == 4);
    animacao.avancar(0.06f);
    assert(animacao.indiceFrameAtual() == 0);

    animacao.iniciarPiscadaDupla(0.06f);
    assert(animacao.indiceFrameAtual() == 0);
    animacao.avancar(0.06f);
    assert(animacao.indiceFrameAtual() == 1);
    animacao.avancar(0.06f);
    assert(animacao.indiceFrameAtual() == 2);
    animacao.avancar(0.06f);
    assert(animacao.indiceFrameAtual() == 3);
    animacao.avancar(0.06f);
    assert(animacao.indiceFrameAtual() == 0);
    animacao.avancar(0.06f);
    assert(animacao.indiceFrameAtual() == 1);
    animacao.avancar(0.06f);
    assert(animacao.indiceFrameAtual() == 2);
    animacao.avancar(0.06f);
    assert(animacao.indiceFrameAtual() == 3);
    animacao.avancar(0.06f);
    assert(animacao.indiceFrameAtual() == 4);
    animacao.avancar(0.06f);
    assert(animacao.indiceFrameAtual() == 0);

    animacao.iniciarCansaco(0.06f, 1.6f);
    assert(animacao.indiceFrameAtual() == 0);
    animacao.avancar(0.06f);
    assert(animacao.indiceFrameAtual() == 1);
    animacao.avancar(1.59f);
    assert(animacao.indiceFrameAtual() == 1);
    animacao.avancar(0.01f);
    assert(animacao.indiceFrameAtual() == 4);
    animacao.avancar(0.06f);
    assert(animacao.indiceFrameAtual() == 0);
}

void validarMovimentoIsometricoDoPersonagem() {
    auto jogo = AppServicos::criarEstadoInicialDoJogo();
    const Geometria::PosicaoNaGrade posicaoInicial = jogo.personagem().posicaoNaGrade();
    const Geometria::PosicaoNaGrade destino{
        posicaoInicial.indiceColuna + 2,
        posicaoInicial.indiceLinha + 1
    };

    jogo.personagem().caminharAte(destino);
    assert(jogo.personagem().estadoAtual() == Personagem::EstadoDoPersonagem::Andando);
    assert(jogo.personagem().direcaoAtual() == Personagem::DirecaoIsometrica::BaixoDireita);
    assert(jogo.personagem().animacaoVisualAtual() == Personagem::AnimacaoVisualDoPersonagem::WalkBaixoDireita);

    AppServicos::avancarTempoDoJogo(
        jogo,
        (2.0f + 0.01f) / Constantes::VELOCIDADE_PERSONAGEM_EM_CELULAS_POR_SEGUNDO
    );
    assert(jogo.personagem().estadoAtual() == Personagem::EstadoDoPersonagem::Andando);
    assert(jogo.personagem().direcaoAtual() == Personagem::DirecaoIsometrica::BaixoEsquerda);
    assert(jogo.personagem().animacaoVisualAtual() == Personagem::AnimacaoVisualDoPersonagem::WalkBaixoEsquerda);

    AppServicos::avancarTempoDoJogo(jogo, 2.0f);
    assert(jogo.personagem().estadoAtual() == Personagem::EstadoDoPersonagem::Parado);
    assert(jogo.personagem().animacaoVisualAtual() == Personagem::AnimacaoVisualDoPersonagem::Idle);
    assert(Geometria::posicoesDaGradeSaoIguais(destino, jogo.personagem().posicaoNaGrade()));

    jogo.personagem().caminharAte(posicaoInicial);
    assert(jogo.personagem().direcaoAtual() == Personagem::DirecaoIsometrica::CimaEsquerda);
    AppServicos::avancarTempoDoJogo(jogo, 2.0f);
    assert(Geometria::posicoesDaGradeSaoIguais(posicaoInicial, jogo.personagem().posicaoNaGrade()));
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
        assert(metadeDaLargura > 0);
        assert(metadeDaAltura > 0);

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
            assert(Geometria::posicoesDaGradeSaoIguais(topo, posicao));

            const Geometria::PosicaoNaGrade centro = Isometria::converterTelaParaGrade(
                destino.coordenadaHorizontal + metadeDaLargura,
                destino.coordenadaVertical + metadeDaAltura,
                dimensoes.largura,
                dimensoes.altura,
                origemX,
                origemY
            );
            assert(Geometria::posicoesDaGradeSaoIguais(centro, posicao));

            const Geometria::PosicaoNaGrade direitaInterior = Isometria::converterTelaParaGrade(
                destino.coordenadaHorizontal + 2 * metadeDaLargura - 1,
                destino.coordenadaVertical + metadeDaAltura,
                dimensoes.largura,
                dimensoes.altura,
                origemX,
                origemY
            );
            assert(Geometria::posicoesDaGradeSaoIguais(direitaInterior, posicao));

            const Geometria::PosicaoNaGrade baixoInterior = Isometria::converterTelaParaGrade(
                destino.coordenadaHorizontal + metadeDaLargura,
                destino.coordenadaVertical + 2 * metadeDaAltura - 1,
                dimensoes.largura,
                dimensoes.altura,
                origemX,
                origemY
            );
            assert(Geometria::posicoesDaGradeSaoIguais(baixoInterior, posicao));

            const Geometria::PosicaoNaGrade esquerdaInterior = Isometria::converterTelaParaGrade(
                destino.coordenadaHorizontal + 1,
                destino.coordenadaVertical + metadeDaAltura,
                dimensoes.largura,
                dimensoes.altura,
                origemX,
                origemY
            );
            assert(Geometria::posicoesDaGradeSaoIguais(esquerdaInterior, posicao));
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

    assert(Geometria::posicoesDaGradeSaoIguais(posicaoConvertida, posicao));
}

} // namespace

int main() {
    validarAnimacaoIdleDoPersonagem();
    validarSequenciasDaAnimacaoIdleDoPersonagem();
    validarMovimentoIsometricoDoPersonagem();
    validarFluxoDeCliqueDaLoja();
    validarHitTestIsometricoDoCanteiro();
    validarHitTestIsometricoGlobalComCamera();

    auto jogo = AppServicos::criarEstadoInicialDoJogo();
    assert(!jogo.identificadorDaSementeSelecionada().has_value());
    assert(jogo.tamanhoAtualDoGrid() == Constantes::TAMANHO_INICIAL_GRID);
    assert(jogo.grade().quantidadeDeTilesExistentes() == 4);
    assert(jogo.grade().quantidadeDeCanteirosEmCrescimento() == 0);
    validarListasDaGrade(jogo.grade());

    const Geometria::PosicaoNaGrade posicaoNova = posicaoLivreProximaDoNucleo();
    jogo.selecionarFerramenta(Ferramentas::TipoDeFerramenta::Enxada);

    Ferramentas::ResultadoDaFerramenta resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicaoNova);
    assert(resultado.acao == Ferramentas::AcaoDaFerramenta::CriarTerra);
    assert(jogo.grade().quantidadeDeTilesExistentes() == 5);
    validarListasDaGrade(jogo.grade());

    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicaoNova);
    assert(resultado.acao == Ferramentas::AcaoDaFerramenta::ArarTerra);

    jogo.selecionarFerramenta(Ferramentas::TipoDeFerramenta::Semente);
    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicaoNova);
    assert(!resultado.houveMudanca());
    assert(jogo.jogador().moedas() == 200);
    assert(jogo.grade().quantidadeDeCanteirosEmCrescimento() == 0);

    const Especies::PlantaMirtilo mirtilo;
    const int moedasAntesDoPlantio = jogo.jogador().moedas();
    jogo.selecionarSemente(Especies::PlantaMirtilo::IDENTIFICADOR_DA_SEMENTE);
    assert(jogo.identificadorDaSementeSelecionada().has_value());
    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicaoNova);
    assert(resultado.acao == Ferramentas::AcaoDaFerramenta::Plantar);
    assert(jogo.jogador().moedas() == moedasAntesDoPlantio - mirtilo.custoEmMoedas());
    assert(jogo.grade().quantidadeDeCanteirosEmCrescimento() == 1);
    assert(jogo.grade().obterTile(posicaoNova)->canteiro().identificadorDaSemente() ==
           Especies::PlantaMirtilo::IDENTIFICADOR_DA_SEMENTE);
    validarListasDaGrade(jogo.grade());

    AppServicos::avancarTempoDoJogo(jogo, static_cast<float>(mirtilo.tempoParaCrescer()));
    const Grade::TileDeTerra* tile = jogo.grade().obterTile(posicaoNova);
    assert(tile != nullptr);
    assert(tile->canteiro().estadoVisualAtual() == Canteiros::EstadoVisualDoCanteiro::PlantaCrescendo);

    AppServicos::avancarTempoDoJogo(
        jogo,
        static_cast<float>(mirtilo.tempoParaFicarJovem() - mirtilo.tempoParaCrescer())
    );
    assert(tile->canteiro().estadoVisualAtual() == Canteiros::EstadoVisualDoCanteiro::PlantaJovem);

    AppServicos::avancarTempoDoJogo(
        jogo,
        static_cast<float>(mirtilo.tempoParaMaturar() - mirtilo.tempoParaFicarJovem())
    );
    assert(tile->canteiro().estadoVisualAtual() == Canteiros::EstadoVisualDoCanteiro::PlantaMadura);

    jogo.selecionarFerramenta(Ferramentas::TipoDeFerramenta::Cursor);
    const int moedasAntesDaColheita = jogo.jogador().moedas();
    const Plantas::RecompensaDaColheita recompensaDaColheita = mirtilo.recompensaDaColheita();
    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicaoNova);
    assert(resultado.acao == Ferramentas::AcaoDaFerramenta::Colher);
    assert(jogo.jogador().moedas() == moedasAntesDaColheita + recompensaDaColheita.moedas);
    assert(jogo.jogador().experiencia() == recompensaDaColheita.experiencia);
    assert(jogo.grade().quantidadeDeCanteirosEmCrescimento() == 0);
    assert(tile->canteiro().estadoVisualAtual() == Canteiros::EstadoVisualDoCanteiro::TerraVazia);
    validarListasDaGrade(jogo.grade());

    ararEPlantar(jogo, posicaoNova);
    AppServicos::avancarTempoDoJogo(jogo, static_cast<float>(mirtilo.tempoParaMorrer()));
    assert(tile->canteiro().estadoVisualAtual() == Canteiros::EstadoVisualDoCanteiro::PlantaMorta);
    assert(jogo.grade().quantidadeDeCanteirosEmCrescimento() == 0);
    validarListasDaGrade(jogo.grade());

    const std::size_t quantidadeAntesDeRemover = jogo.grade().quantidadeDeTilesExistentes();
    jogo.selecionarFerramenta(Ferramentas::TipoDeFerramenta::RemoverTerra);
    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicaoNova);
    assert(resultado.acao == Ferramentas::AcaoDaFerramenta::RemoverTerra);
    assert(jogo.grade().quantidadeDeTilesExistentes() == quantidadeAntesDeRemover - 1);
    assert(tile->existeNoMapa() == false);
    validarListasDaGrade(jogo.grade());

    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, Geometria::PosicaoNaGrade{-1, -1});
    assert(!resultado.houveMudanca());

    const Geometria::PosicaoNaGrade foraDaGradeJogavel{
        Grade::GradeGlobalDeCanteiros::calcularColunaInicialDaGradeAtual(jogo.tamanhoAtualDoGrid()) - 1,
        Grade::GradeGlobalDeCanteiros::calcularLinhaInicialDaGradeAtual(jogo.tamanhoAtualDoGrid())
    };
    jogo.selecionarFerramenta(Ferramentas::TipoDeFerramenta::Enxada);
    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, foraDaGradeJogavel);
    assert(!resultado.houveMudanca());

    MiniFazenda::Apresentacao::ConfiguracoesDoLayout configuracoes;
    Camera::aplicarOrigemCentradaDaGrade(configuracoes, jogo.tamanhoAtualDoGrid());
    Camera::EstadoDaCamera camera;
    Camera::RetanguloDeGradeRenderizada retangulo = Camera::calcularRetanguloDaGradeRenderizada(
        configuracoes,
        camera,
        jogo.tamanhoAtualDoGrid()
    );
    assert(retangulo.x + retangulo.largura / 2 == configuracoes.centroVisualBackgroundX);
    assert(retangulo.y + retangulo.altura / 2 == configuracoes.centroVisualBackgroundY);

    Camera::aplicarOrigemCentradaDaGrade(configuracoes, Constantes::TAMANHO_MAXIMO_GRID);
    retangulo = Camera::calcularRetanguloDaGradeRenderizada(
        configuracoes,
        camera,
        Constantes::TAMANHO_MAXIMO_GRID
    );
    assert(retangulo.x + retangulo.largura / 2 == configuracoes.centroVisualBackgroundX);
    assert(retangulo.y + retangulo.altura / 2 == configuracoes.centroVisualBackgroundY);

    const bool zoomAplicado = Camera::aplicarZoomNoPonto(
        camera,
        configuracoes,
        jogo.tamanhoAtualDoGrid(),
        Constantes::LARGURA_DA_JANELA / 2,
        Constantes::ALTURA_DA_JANELA / 2,
        1
    );
    assert(zoomAplicado);
    assert(camera.zoomAtual > Constantes::ZOOM_INICIAL);
    Camera::centralizarCamera(camera);
    assert(camera.zoomAtual == Constantes::ZOOM_INICIAL);
    assert(camera.offsetHorizontal == 0);
    assert(camera.offsetVertical == 0);

    return 0;
}
