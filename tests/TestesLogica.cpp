#include "SistemasDoJogo.hpp"

#include <cassert>
#include <cstddef>

namespace {

void validarIndicesDaGrade(const GradeGlobalDeCanteiros& grade) {
    for (std::size_t indice = 0; indice < grade.posicoesDeTilesExistentes.size(); ++indice) {
        const PosicaoNaGrade posicao = grade.posicoesDeTilesExistentes[indice];
        const TileDeTerra* tile = Grade::obterTile(grade, posicao);
        assert(tile != nullptr);
        assert(tile->existeNoMapa);
        assert(tile->indiceNaListaDeTilesExistentes == indice);
    }

    for (std::size_t indice = 0; indice < grade.posicoesDeCanteirosEmCrescimento.size(); ++indice) {
        const PosicaoNaGrade posicao = grade.posicoesDeCanteirosEmCrescimento[indice];
        const TileDeTerra* tile = Grade::obterTile(grade, posicao);
        assert(tile != nullptr);
        assert(tile->existeNoMapa);
        assert(tile->indiceNaListaDeCrescimento == indice);
        assert(Grade::estadoPrecisaAvancarCrescimento(tile->canteiro.estadoVisualAtual));
    }
}

PosicaoNaGrade posicaoLivreProximaDoNucleo() {
    return PosicaoNaGrade{
        Constantes::COLUNA_INICIAL_DO_NUCLEO_INICIAL + 3,
        Constantes::LINHA_INICIAL_DO_NUCLEO_INICIAL
    };
}

void ararEPlantar(EstadoDoJogo& jogo, PosicaoNaGrade posicao) {
    jogo.ferramentaSelecionada = FERRAMENTA_ENXADA;
    ResultadoDaFerramenta resultado = aplicarFerramentaNoJogo(jogo, posicao);
    assert(resultado.acao == ACAO_ARAR_TERRA);

    jogo.ferramentaSelecionada = FERRAMENTA_SEMENTE;
    resultado = aplicarFerramentaNoJogo(jogo, posicao);
    assert(resultado.acao == ACAO_PLANTAR);
}

} // namespace

int main() {
    EstadoDoJogo jogo = criarEstadoInicialDoJogo();
    assert(jogo.tamanhoAtualDoGrid == Constantes::TAMANHO_INICIAL_GRID);
    assert(jogo.grade.posicoesDeTilesExistentes.size() == 4);
    assert(jogo.grade.posicoesDeCanteirosEmCrescimento.empty());
    validarIndicesDaGrade(jogo.grade);

    const PosicaoNaGrade posicaoNova = posicaoLivreProximaDoNucleo();
    jogo.ferramentaSelecionada = FERRAMENTA_ENXADA;

    ResultadoDaFerramenta resultado = aplicarFerramentaNoJogo(jogo, posicaoNova);
    assert(resultado.acao == ACAO_CRIAR_TERRA);
    assert(jogo.grade.posicoesDeTilesExistentes.size() == 5);
    validarIndicesDaGrade(jogo.grade);

    resultado = aplicarFerramentaNoJogo(jogo, posicaoNova);
    assert(resultado.acao == ACAO_ARAR_TERRA);

    jogo.ferramentaSelecionada = FERRAMENTA_SEMENTE;
    resultado = aplicarFerramentaNoJogo(jogo, posicaoNova);
    assert(resultado.acao == ACAO_PLANTAR);
    assert(jogo.moedas == 48);
    assert(jogo.grade.posicoesDeCanteirosEmCrescimento.size() == 1);
    validarIndicesDaGrade(jogo.grade);

    avancarTempoDoJogo(jogo, static_cast<float>(Constantes::TEMPO_PARA_CRESCER));
    const TileDeTerra* tile = Grade::obterTile(jogo.grade, posicaoNova);
    assert(tile != nullptr);
    assert(tile->canteiro.estadoVisualAtual == ESTADO_PLANTA_CRESCENDO);

    avancarTempoDoJogo(
        jogo,
        static_cast<float>(Constantes::TEMPO_PARA_MADURAR - Constantes::TEMPO_PARA_CRESCER)
    );
    assert(tile->canteiro.estadoVisualAtual == ESTADO_PLANTA_MADURA);

    jogo.ferramentaSelecionada = FERRAMENTA_CURSOR;
    resultado = aplicarFerramentaNoJogo(jogo, posicaoNova);
    assert(resultado.acao == ACAO_COLHER);
    assert(jogo.moedas == 56);
    assert(jogo.experiencia == 5);
    assert(jogo.grade.posicoesDeCanteirosEmCrescimento.empty());
    assert(tile->canteiro.estadoVisualAtual == ESTADO_TERRA_VAZIA);
    validarIndicesDaGrade(jogo.grade);

    ararEPlantar(jogo, posicaoNova);
    avancarTempoDoJogo(jogo, static_cast<float>(Constantes::TEMPO_PARA_MORRER));
    assert(tile->canteiro.estadoVisualAtual == ESTADO_PLANTA_MORTA);
    assert(jogo.grade.posicoesDeCanteirosEmCrescimento.empty());
    validarIndicesDaGrade(jogo.grade);

    const std::size_t quantidadeAntesDeRemover = jogo.grade.posicoesDeTilesExistentes.size();
    jogo.ferramentaSelecionada = FERRAMENTA_REMOVER_TERRA;
    resultado = aplicarFerramentaNoJogo(jogo, posicaoNova);
    assert(resultado.acao == ACAO_REMOVER_TERRA);
    assert(jogo.grade.posicoesDeTilesExistentes.size() == quantidadeAntesDeRemover - 1);
    assert(tile->existeNoMapa == false);
    validarIndicesDaGrade(jogo.grade);

    resultado = aplicarFerramentaNoJogo(jogo, PosicaoNaGrade{-1, -1});
    assert(!resultado.houveMudanca());

    const PosicaoNaGrade foraDaGradeJogavel{
        Grade::calcularColunaInicialDaGradeAtual(jogo.tamanhoAtualDoGrid) - 1,
        Grade::calcularLinhaInicialDaGradeAtual(jogo.tamanhoAtualDoGrid)
    };
    jogo.ferramentaSelecionada = FERRAMENTA_ENXADA;
    resultado = aplicarFerramentaNoJogo(jogo, foraDaGradeJogavel);
    assert(!resultado.houveMudanca());

    ConfiguracoesDoLayout configuracoes;
    aplicarOrigemCentradaDaGrade(configuracoes, jogo.tamanhoAtualDoGrid);
    EstadoDaCamera camera;
    RetanguloDeGradeRenderizada retangulo = calcularRetanguloDaGradeRenderizada(
        configuracoes,
        camera,
        jogo.tamanhoAtualDoGrid
    );
    assert(retangulo.x + retangulo.largura / 2 == configuracoes.centroVisualBackgroundX);
    assert(retangulo.y + retangulo.altura / 2 == configuracoes.centroVisualBackgroundY);

    aplicarOrigemCentradaDaGrade(configuracoes, Constantes::TAMANHO_MAXIMO_GRID);
    retangulo = calcularRetanguloDaGradeRenderizada(
        configuracoes,
        camera,
        Constantes::TAMANHO_MAXIMO_GRID
    );
    assert(retangulo.x + retangulo.largura / 2 == configuracoes.centroVisualBackgroundX);
    assert(retangulo.y + retangulo.altura / 2 == configuracoes.centroVisualBackgroundY);

    const bool zoomAplicado = aplicarZoomNoPonto(
        camera,
        configuracoes,
        jogo.tamanhoAtualDoGrid,
        Constantes::LARGURA_DA_JANELA / 2,
        Constantes::ALTURA_DA_JANELA / 2,
        1
    );
    assert(zoomAplicado);
    assert(camera.zoomAtual > Constantes::ZOOM_INICIAL);
    centralizarCamera(camera);
    assert(camera.zoomAtual == Constantes::ZOOM_INICIAL);
    assert(camera.offsetHorizontal == 0);
    assert(camera.offsetVertical == 0);

    return 0;
}
