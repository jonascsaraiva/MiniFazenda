#include "Aplicacao/Servicos/InicializadorDaFazenda.hpp"
#include "Aplicacao/Servicos/ServicoDeFerramentas.hpp"
#include "Aplicacao/Servicos/ServicoDeTempo.hpp"
#include "Apresentacao/Camera/CameraDoJogo.hpp"
#include "Apresentacao/ConfiguracoesDoLayout.hpp"
#include "Apresentacao/Interface/BarraDeFerramentas/BarraDeFerramentas.hpp"
#include "Compartilhado/Constantes.hpp"
#include "Compartilhado/Geometria/Posicoes.hpp"
#include "Dominio/Canteiros/EstadoDoCanteiro.hpp"
#include "Dominio/Ferramentas/ResultadoDaFerramenta.hpp"
#include "Dominio/Ferramentas/TipoDeFerramenta.hpp"
#include "Dominio/Grade/GradeGlobalDeCanteiros.hpp"
#include "Dominio/Plantas/FabricaDePlantas.hpp"
#include "Dominio/Plantas/Especies/PlantaMirtilo.hpp"

#include <cassert>
#include <cstddef>

namespace {

namespace AppServicos = MiniFazenda::Aplicacao::Servicos;
namespace BarraFerramentas = MiniFazenda::Apresentacao::Interface::BarraDeFerramentas;
namespace Camera = MiniFazenda::Apresentacao::Camera;
namespace Canteiros = MiniFazenda::Dominio::Canteiros;
namespace Constantes = MiniFazenda::Compartilhado::Constantes;
namespace Especies = MiniFazenda::Dominio::Plantas::Especies;
namespace Ferramentas = MiniFazenda::Dominio::Ferramentas;
namespace Geometria = MiniFazenda::Compartilhado::Geometria;
namespace Grade = MiniFazenda::Dominio::Grade;
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

} // namespace

int main() {
    validarFluxoDeCliqueDaLoja();

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
    assert(jogo.jogador().moedas() == 50);
    assert(jogo.grade().quantidadeDeCanteirosEmCrescimento() == 0);

    jogo.selecionarSemente(Especies::PlantaMirtilo::IDENTIFICADOR_DA_SEMENTE);
    assert(jogo.identificadorDaSementeSelecionada().has_value());
    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicaoNova);
    assert(resultado.acao == Ferramentas::AcaoDaFerramenta::Plantar);
    assert(jogo.jogador().moedas() == 48);
    assert(jogo.grade().quantidadeDeCanteirosEmCrescimento() == 1);
    assert(jogo.grade().obterTile(posicaoNova)->canteiro().identificadorDaSemente() ==
           Especies::PlantaMirtilo::IDENTIFICADOR_DA_SEMENTE);
    validarListasDaGrade(jogo.grade());

    const Especies::PlantaMirtilo mirtilo;

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
    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicaoNova);
    assert(resultado.acao == Ferramentas::AcaoDaFerramenta::Colher);
    assert(jogo.jogador().moedas() == 56);
    assert(jogo.jogador().experiencia() == 5);
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
