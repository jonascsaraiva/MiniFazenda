#pragma once

#include "Constantes.hpp"

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

enum EstadoDoCanteiro {
    ESTADO_TERRA_VAZIA,
    ESTADO_TERRA_ARADA,
    ESTADO_SEMENTE_PLANTADA,
    ESTADO_PLANTA_CRESCENDO,
    ESTADO_PLANTA_MADURA,
    ESTADO_PLANTA_MORTA
};

enum FerramentaSelecionada {
    FERRAMENTA_CURSOR,
    FERRAMENTA_ENXADA,
    FERRAMENTA_REMOVER_TERRA,
    FERRAMENTA_SEMENTE,
    FERRAMENTA_PRESENTE
};

struct PosicaoNaTela {
    int coordenadaHorizontal = 0;
    int coordenadaVertical = 0;
};

struct PosicaoNaGrade {
    int indiceColuna = -1;
    int indiceLinha = -1;
};

struct DadosDoCanteiro {
    EstadoDoCanteiro estadoVisualAtual = ESTADO_TERRA_VAZIA;
    int tempoDePlantioEmSegundos = 0;
    int identificadorDaSemente = -1;
};

struct TileDeTerra {
    bool existeNoMapa = false;
    DadosDoCanteiro canteiro;
};

struct GradeGlobalDeCanteiros {
    std::vector<TileDeTerra> tiles;
    std::vector<PosicaoNaGrade> posicoesDeTilesExistentes;

    GradeGlobalDeCanteiros()
        : tiles(static_cast<std::size_t>(Constantes::TOTAL_DE_TILES_DA_GRADE_GLOBAL)) {
    }
};

struct ConfiguracoesDoLayout {
    int deslocamentoGradeHorizontal = 576;
    int deslocamentoGradeVertical = 296;

    std::string arquivoBackgroundPrincipal = "background.png";
};

struct AreaDeInteracao {
    int posicaoBotaoHorizontal = 0;
    int posicaoBotaoVertical = 0;
    int tamanhoBotaoLargura = 0;
    int tamanhoBotaoAltura = 0;
};

inline bool verificarCliqueNoBotao(int cliqueMouseHorizontal, int cliqueMouseVertical, AreaDeInteracao limitesDoBotao) {
    const bool colidiuHorizontalmente =
        cliqueMouseHorizontal >= limitesDoBotao.posicaoBotaoHorizontal &&
        cliqueMouseHorizontal <= (limitesDoBotao.posicaoBotaoHorizontal + limitesDoBotao.tamanhoBotaoLargura);

    const bool colidiuVerticalmente =
        cliqueMouseVertical >= limitesDoBotao.posicaoBotaoVertical &&
        cliqueMouseVertical <= (limitesDoBotao.posicaoBotaoVertical + limitesDoBotao.tamanhoBotaoAltura);

    return colidiuHorizontalmente && colidiuVerticalmente;
}

inline bool posicoesDaGradeSaoIguais(PosicaoNaGrade primeira, PosicaoNaGrade segunda) {
    return primeira.indiceColuna == segunda.indiceColuna &&
           primeira.indiceLinha == segunda.indiceLinha;
}

inline bool posicaoEstaDentroDaGradeGlobal(PosicaoNaGrade posicao) {
    return posicao.indiceColuna >= 0 &&
           posicao.indiceColuna < Constantes::QUANTIDADE_DE_COLUNAS_DA_GRADE_GLOBAL &&
           posicao.indiceLinha >= 0 &&
           posicao.indiceLinha < Constantes::QUANTIDADE_DE_LINHAS_DA_GRADE_GLOBAL;
}

inline std::size_t calcularIndiceLinearDoTile(PosicaoNaGrade posicao) {
    return static_cast<std::size_t>(posicao.indiceLinha) *
           static_cast<std::size_t>(Constantes::QUANTIDADE_DE_COLUNAS_DA_GRADE_GLOBAL) +
           static_cast<std::size_t>(posicao.indiceColuna);
}

inline TileDeTerra* obterTileDaGradeGlobal(GradeGlobalDeCanteiros& grade, PosicaoNaGrade posicao) {
    if (!posicaoEstaDentroDaGradeGlobal(posicao)) {
        return nullptr;
    }

    return &grade.tiles[calcularIndiceLinearDoTile(posicao)];
}

inline const TileDeTerra* obterTileDaGradeGlobal(const GradeGlobalDeCanteiros& grade, PosicaoNaGrade posicao) {
    if (!posicaoEstaDentroDaGradeGlobal(posicao)) {
        return nullptr;
    }

    return &grade.tiles[calcularIndiceLinearDoTile(posicao)];
}

inline bool gradeGlobalJaTemTileRegistrado(const GradeGlobalDeCanteiros& grade, PosicaoNaGrade posicao) {
    return std::any_of(
        grade.posicoesDeTilesExistentes.begin(),
        grade.posicoesDeTilesExistentes.end(),
        [posicao](PosicaoNaGrade posicaoRegistrada) {
            return posicoesDaGradeSaoIguais(posicaoRegistrada, posicao);
        }
    );
}

inline TileDeTerra* ativarTileNaGradeGlobal(GradeGlobalDeCanteiros& grade, PosicaoNaGrade posicao) {
    TileDeTerra* tile = obterTileDaGradeGlobal(grade, posicao);
    if (tile == nullptr) {
        return nullptr;
    }

    if (!tile->existeNoMapa) {
        tile->existeNoMapa = true;
        tile->canteiro = DadosDoCanteiro{};
    }

    if (!gradeGlobalJaTemTileRegistrado(grade, posicao)) {
        grade.posicoesDeTilesExistentes.push_back(posicao);
    }

    return tile;
}

inline void removerTileDaGradeGlobal(GradeGlobalDeCanteiros& grade, PosicaoNaGrade posicao) {
    TileDeTerra* tile = obterTileDaGradeGlobal(grade, posicao);
    if (tile == nullptr || !tile->existeNoMapa) {
        return;
    }

    tile->existeNoMapa = false;
    tile->canteiro = DadosDoCanteiro{};

    grade.posicoesDeTilesExistentes.erase(
        std::remove_if(
            grade.posicoesDeTilesExistentes.begin(),
            grade.posicoesDeTilesExistentes.end(),
            [posicao](PosicaoNaGrade posicaoRegistrada) {
                return posicoesDaGradeSaoIguais(posicaoRegistrada, posicao);
            }
        ),
        grade.posicoesDeTilesExistentes.end()
    );
}
