#include "Aplicacao/Servicos/InicializadorDaFazenda.hpp"
#include "Aplicacao/Servicos/ServicoDeFerramentas.hpp"
#include "Aplicacao/Servicos/ServicoDeTempo.hpp"
#include "Apresentacao/Camera/CameraDoJogo.hpp"
#include "Apresentacao/ConfiguracoesDoLayout.hpp"
#include "Compartilhado/Constantes.hpp"
#include "Compartilhado/Geometria/Posicoes.hpp"
#include "Dominio/Canteiros/EstadoDoCanteiro.hpp"
#include "Dominio/Ferramentas/ResultadoDaFerramenta.hpp"
#include "Dominio/Ferramentas/TipoDeFerramenta.hpp"
#include "Dominio/Grade/GradeGlobalDeCanteiros.hpp"

#include <cassert>
#include <cstddef>

namespace {

namespace AppServicos = MiniFazenda::Aplicacao::Servicos;
namespace Camera = MiniFazenda::Apresentacao::Camera;
namespace Canteiros = MiniFazenda::Dominio::Canteiros;
namespace Constantes = MiniFazenda::Compartilhado::Constantes;
namespace Ferramentas = MiniFazenda::Dominio::Ferramentas;
namespace Geometria = MiniFazenda::Compartilhado::Geometria;
namespace Grade = MiniFazenda::Dominio::Grade;

void validarIndicesDaGrade(const Grade::GradeGlobalDeCanteiros& grade) {
    for (std::size_t indice = 0; indice < grade.posicoesDeTilesExistentes().size(); ++indice) {
        const Geometria::PosicaoNaGrade posicao = grade.posicoesDeTilesExistentes()[indice];
        const Grade::TileDeTerra* tile = grade.obterTile(posicao);
        assert(tile != nullptr);
        assert(tile->existeNoMapa());
        assert(tile->indiceNaListaDeTilesExistentes() == indice);
    }

    for (std::size_t indice = 0; indice < grade.posicoesDeCanteirosEmCrescimento().size(); ++indice) {
        const Geometria::PosicaoNaGrade posicao = grade.posicoesDeCanteirosEmCrescimento()[indice];
        const Grade::TileDeTerra* tile = grade.obterTile(posicao);
        assert(tile != nullptr);
        assert(tile->existeNoMapa());
        assert(tile->indiceNaListaDeCrescimento() == indice);
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

    jogo.selecionarFerramenta(Ferramentas::TipoDeFerramenta::Semente);
    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicao);
    assert(resultado.acao == Ferramentas::AcaoDaFerramenta::Plantar);
}

} // namespace

int main() {
    auto jogo = AppServicos::criarEstadoInicialDoJogo();
    assert(jogo.tamanhoAtualDoGrid() == Constantes::TAMANHO_INICIAL_GRID);
    assert(jogo.grade().quantidadeDeTilesExistentes() == 4);
    assert(jogo.grade().quantidadeDeCanteirosEmCrescimento() == 0);
    validarIndicesDaGrade(jogo.grade());

    const Geometria::PosicaoNaGrade posicaoNova = posicaoLivreProximaDoNucleo();
    jogo.selecionarFerramenta(Ferramentas::TipoDeFerramenta::Enxada);

    Ferramentas::ResultadoDaFerramenta resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicaoNova);
    assert(resultado.acao == Ferramentas::AcaoDaFerramenta::CriarTerra);
    assert(jogo.grade().quantidadeDeTilesExistentes() == 5);
    validarIndicesDaGrade(jogo.grade());

    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicaoNova);
    assert(resultado.acao == Ferramentas::AcaoDaFerramenta::ArarTerra);

    jogo.selecionarFerramenta(Ferramentas::TipoDeFerramenta::Semente);
    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicaoNova);
    assert(resultado.acao == Ferramentas::AcaoDaFerramenta::Plantar);
    assert(jogo.jogador().moedas() == 48);
    assert(jogo.grade().quantidadeDeCanteirosEmCrescimento() == 1);
    validarIndicesDaGrade(jogo.grade());

    AppServicos::avancarTempoDoJogo(jogo, static_cast<float>(Constantes::TEMPO_PARA_CRESCER));
    const Grade::TileDeTerra* tile = jogo.grade().obterTile(posicaoNova);
    assert(tile != nullptr);
    assert(tile->canteiro().estadoVisualAtual() == Canteiros::EstadoVisualDoCanteiro::PlantaCrescendo);

    AppServicos::avancarTempoDoJogo(
        jogo,
        static_cast<float>(Constantes::TEMPO_PARA_MADURAR - Constantes::TEMPO_PARA_CRESCER)
    );
    assert(tile->canteiro().estadoVisualAtual() == Canteiros::EstadoVisualDoCanteiro::PlantaMadura);

    jogo.selecionarFerramenta(Ferramentas::TipoDeFerramenta::Cursor);
    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicaoNova);
    assert(resultado.acao == Ferramentas::AcaoDaFerramenta::Colher);
    assert(jogo.jogador().moedas() == 56);
    assert(jogo.jogador().experiencia() == 5);
    assert(jogo.grade().quantidadeDeCanteirosEmCrescimento() == 0);
    assert(tile->canteiro().estadoVisualAtual() == Canteiros::EstadoVisualDoCanteiro::TerraVazia);
    validarIndicesDaGrade(jogo.grade());

    ararEPlantar(jogo, posicaoNova);
    AppServicos::avancarTempoDoJogo(jogo, static_cast<float>(Constantes::TEMPO_PARA_MORRER));
    assert(tile->canteiro().estadoVisualAtual() == Canteiros::EstadoVisualDoCanteiro::PlantaMorta);
    assert(jogo.grade().quantidadeDeCanteirosEmCrescimento() == 0);
    validarIndicesDaGrade(jogo.grade());

    const std::size_t quantidadeAntesDeRemover = jogo.grade().quantidadeDeTilesExistentes();
    jogo.selecionarFerramenta(Ferramentas::TipoDeFerramenta::RemoverTerra);
    resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicaoNova);
    assert(resultado.acao == Ferramentas::AcaoDaFerramenta::RemoverTerra);
    assert(jogo.grade().quantidadeDeTilesExistentes() == quantidadeAntesDeRemover - 1);
    assert(tile->existeNoMapa() == false);
    validarIndicesDaGrade(jogo.grade());

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
