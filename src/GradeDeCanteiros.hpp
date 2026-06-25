#pragma once

#include "Constantes.hpp"
#include "Tipos.hpp"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <vector>

constexpr std::size_t CAPACIDADE_INICIAL_DAS_LISTAS_DA_GRADE = 128;

struct TileDeTerra {
    bool existeNoMapa = false;
    DadosDoCanteiro canteiro;
    std::size_t indiceNaListaDeTilesExistentes = std::numeric_limits<std::size_t>::max();
    std::size_t indiceNaListaDeCrescimento = std::numeric_limits<std::size_t>::max();
};

struct GradeGlobalDeCanteiros {
    std::vector<TileDeTerra> tiles;
    std::vector<PosicaoNaGrade> posicoesDeTilesExistentes;
    std::vector<PosicaoNaGrade> posicoesDeCanteirosEmCrescimento;

    GradeGlobalDeCanteiros()
        : tiles(static_cast<std::size_t>(Constantes::TOTAL_DE_TILES_DA_GRADE_GLOBAL)) {
        posicoesDeTilesExistentes.reserve(CAPACIDADE_INICIAL_DAS_LISTAS_DA_GRADE);
        posicoesDeCanteirosEmCrescimento.reserve(CAPACIDADE_INICIAL_DAS_LISTAS_DA_GRADE);
    }
};

namespace Grade {

constexpr std::size_t INDICE_INVALIDO = std::numeric_limits<std::size_t>::max();

inline bool posicaoEstaDentroDaGradeGlobal(PosicaoNaGrade posicao) {
    return posicao.indiceColuna >= 0 &&
           posicao.indiceColuna < Constantes::QUANTIDADE_DE_COLUNAS_DA_GRADE_GLOBAL &&
           posicao.indiceLinha >= 0 &&
           posicao.indiceLinha < Constantes::QUANTIDADE_DE_LINHAS_DA_GRADE_GLOBAL;
}

inline int normalizarTamanhoDaGradeAtual(int tamanhoAtualDoGrid) {
    const int tamanhoLimitado = std::clamp(
        tamanhoAtualDoGrid,
        Constantes::TAMANHO_INICIAL_GRID,
        Constantes::TAMANHO_MAXIMO_GRID
    );

    if ((tamanhoLimitado - Constantes::TAMANHO_INICIAL_GRID) % Constantes::INCREMENTO_TAMANHO_GRID == 0) {
        return tamanhoLimitado;
    }

    const int deslocamento = tamanhoLimitado - Constantes::TAMANHO_INICIAL_GRID;
    return Constantes::TAMANHO_INICIAL_GRID +
           (deslocamento / Constantes::INCREMENTO_TAMANHO_GRID) * Constantes::INCREMENTO_TAMANHO_GRID;
}

inline int calcularColunaInicialDaGradeAtual(int tamanhoAtualDoGrid) {
    const int tamanhoNormalizado = normalizarTamanhoDaGradeAtual(tamanhoAtualDoGrid);
    return Constantes::COLUNA_CENTRAL_DA_GRADE_GLOBAL - tamanhoNormalizado / 2;
}

inline int calcularLinhaInicialDaGradeAtual(int tamanhoAtualDoGrid) {
    const int tamanhoNormalizado = normalizarTamanhoDaGradeAtual(tamanhoAtualDoGrid);
    return Constantes::LINHA_CENTRAL_DA_GRADE_GLOBAL - tamanhoNormalizado / 2;
}

inline bool posicaoEstaDentroDaGradeAtual(PosicaoNaGrade posicao, int tamanhoAtualDoGrid) {
    const int tamanhoNormalizado = normalizarTamanhoDaGradeAtual(tamanhoAtualDoGrid);
    const int colunaInicial = calcularColunaInicialDaGradeAtual(tamanhoNormalizado);
    const int linhaInicial = calcularLinhaInicialDaGradeAtual(tamanhoNormalizado);

    return posicao.indiceColuna >= colunaInicial &&
           posicao.indiceColuna < colunaInicial + tamanhoNormalizado &&
           posicao.indiceLinha >= linhaInicial &&
           posicao.indiceLinha < linhaInicial + tamanhoNormalizado;
}

inline std::size_t calcularIndiceLinearDoTile(PosicaoNaGrade posicao) {
    return static_cast<std::size_t>(posicao.indiceLinha) *
           static_cast<std::size_t>(Constantes::QUANTIDADE_DE_COLUNAS_DA_GRADE_GLOBAL) +
           static_cast<std::size_t>(posicao.indiceColuna);
}

inline TileDeTerra* obterTile(GradeGlobalDeCanteiros& grade, PosicaoNaGrade posicao) {
    if (!posicaoEstaDentroDaGradeGlobal(posicao)) {
        return nullptr;
    }

    return &grade.tiles[calcularIndiceLinearDoTile(posicao)];
}

inline const TileDeTerra* obterTile(const GradeGlobalDeCanteiros& grade, PosicaoNaGrade posicao) {
    if (!posicaoEstaDentroDaGradeGlobal(posicao)) {
        return nullptr;
    }

    return &grade.tiles[calcularIndiceLinearDoTile(posicao)];
}

inline bool estadoPrecisaAvancarCrescimento(EstadoDoCanteiro estado) {
    return estado == ESTADO_SEMENTE_PLANTADA ||
           estado == ESTADO_PLANTA_CRESCENDO ||
           estado == ESTADO_PLANTA_MADURA;
}

inline void removerCanteiroDaListaDeCrescimento(GradeGlobalDeCanteiros& grade, PosicaoNaGrade posicao) {
    TileDeTerra* tile = obterTile(grade, posicao);
    if (tile == nullptr || tile->indiceNaListaDeCrescimento == INDICE_INVALIDO) {
        return;
    }

    const std::size_t indiceRemovido = tile->indiceNaListaDeCrescimento;
    if (indiceRemovido >= grade.posicoesDeCanteirosEmCrescimento.size()) {
        tile->indiceNaListaDeCrescimento = INDICE_INVALIDO;
        return;
    }

    const std::size_t ultimoIndice = grade.posicoesDeCanteirosEmCrescimento.size() - 1;

    if (indiceRemovido != ultimoIndice) {
        const PosicaoNaGrade posicaoMovida = grade.posicoesDeCanteirosEmCrescimento[ultimoIndice];
        grade.posicoesDeCanteirosEmCrescimento[indiceRemovido] = posicaoMovida;

        TileDeTerra* tileMovido = obterTile(grade, posicaoMovida);
        if (tileMovido != nullptr) {
            tileMovido->indiceNaListaDeCrescimento = indiceRemovido;
        }
    }

    grade.posicoesDeCanteirosEmCrescimento.pop_back();
    tile->indiceNaListaDeCrescimento = INDICE_INVALIDO;
}

inline void registrarCanteiroEmCrescimento(GradeGlobalDeCanteiros& grade, PosicaoNaGrade posicao) {
    TileDeTerra* tile = obterTile(grade, posicao);
    if (tile == nullptr ||
        !tile->existeNoMapa ||
        tile->indiceNaListaDeCrescimento != INDICE_INVALIDO ||
        !estadoPrecisaAvancarCrescimento(tile->canteiro.estadoVisualAtual)) {
        return;
    }

    tile->indiceNaListaDeCrescimento = grade.posicoesDeCanteirosEmCrescimento.size();
    grade.posicoesDeCanteirosEmCrescimento.push_back(posicao);
}

inline void sincronizarCrescimentoDoCanteiro(GradeGlobalDeCanteiros& grade, PosicaoNaGrade posicao) {
    const TileDeTerra* tile = obterTile(grade, posicao);
    if (tile != nullptr && tile->existeNoMapa && estadoPrecisaAvancarCrescimento(tile->canteiro.estadoVisualAtual)) {
        registrarCanteiroEmCrescimento(grade, posicao);
        return;
    }

    removerCanteiroDaListaDeCrescimento(grade, posicao);
}

inline TileDeTerra* ativarTile(GradeGlobalDeCanteiros& grade, PosicaoNaGrade posicao) {
    TileDeTerra* tile = obterTile(grade, posicao);
    if (tile == nullptr) {
        return nullptr;
    }

    if (tile->existeNoMapa) {
        return tile;
    }

    tile->existeNoMapa = true;
    tile->canteiro = DadosDoCanteiro{};
    tile->indiceNaListaDeCrescimento = INDICE_INVALIDO;
    tile->indiceNaListaDeTilesExistentes = grade.posicoesDeTilesExistentes.size();
    grade.posicoesDeTilesExistentes.push_back(posicao);

    return tile;
}

inline void removerTile(GradeGlobalDeCanteiros& grade, PosicaoNaGrade posicao) {
    TileDeTerra* tile = obterTile(grade, posicao);
    if (tile == nullptr || !tile->existeNoMapa) {
        return;
    }

    removerCanteiroDaListaDeCrescimento(grade, posicao);

    const std::size_t indiceRemovido = tile->indiceNaListaDeTilesExistentes;
    if (indiceRemovido >= grade.posicoesDeTilesExistentes.size()) {
        tile->existeNoMapa = false;
        tile->canteiro = DadosDoCanteiro{};
        tile->indiceNaListaDeTilesExistentes = INDICE_INVALIDO;
        tile->indiceNaListaDeCrescimento = INDICE_INVALIDO;
        return;
    }

    const std::size_t ultimoIndice = grade.posicoesDeTilesExistentes.size() - 1;

    if (indiceRemovido != ultimoIndice) {
        const PosicaoNaGrade posicaoMovida = grade.posicoesDeTilesExistentes[ultimoIndice];
        grade.posicoesDeTilesExistentes[indiceRemovido] = posicaoMovida;

        TileDeTerra* tileMovido = obterTile(grade, posicaoMovida);
        if (tileMovido != nullptr) {
            tileMovido->indiceNaListaDeTilesExistentes = indiceRemovido;
        }
    }

    grade.posicoesDeTilesExistentes.pop_back();
    tile->existeNoMapa = false;
    tile->canteiro = DadosDoCanteiro{};
    tile->indiceNaListaDeTilesExistentes = INDICE_INVALIDO;
    tile->indiceNaListaDeCrescimento = INDICE_INVALIDO;
}

} // namespace Grade
