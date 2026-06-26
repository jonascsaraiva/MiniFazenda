#pragma once

#include "Compartilhado/Constantes.hpp"
#include "Compartilhado/Geometria/Posicoes.hpp"
#include "Dominio/Grade/TileDeTerra.hpp"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace MiniFazenda::Dominio::Grade {

using Compartilhado::Geometria::PosicaoNaGrade;

constexpr std::size_t CAPACIDADE_INICIAL_DAS_LISTAS_DA_GRADE = 128;

class GradeGlobalDeCanteiros {
public:
    GradeGlobalDeCanteiros()
        : tiles_(static_cast<std::size_t>(Compartilhado::Constantes::TOTAL_DE_TILES_DA_GRADE_GLOBAL)) {
        posicoesDeTilesExistentes_.reserve(CAPACIDADE_INICIAL_DAS_LISTAS_DA_GRADE);
        posicoesDeCanteirosEmCrescimento_.reserve(CAPACIDADE_INICIAL_DAS_LISTAS_DA_GRADE);
    }

    static bool posicaoEstaDentroDaGradeGlobal(PosicaoNaGrade posicao) {
        return posicao.indiceColuna >= 0 &&
               posicao.indiceColuna < Compartilhado::Constantes::QUANTIDADE_DE_COLUNAS_DA_GRADE_GLOBAL &&
               posicao.indiceLinha >= 0 &&
               posicao.indiceLinha < Compartilhado::Constantes::QUANTIDADE_DE_LINHAS_DA_GRADE_GLOBAL;
    }

    static int normalizarTamanhoDaGradeAtual(int tamanhoAtualDoGrid) {
        const int tamanhoLimitado = std::clamp(
            tamanhoAtualDoGrid,
            Compartilhado::Constantes::TAMANHO_INICIAL_GRID,
            Compartilhado::Constantes::TAMANHO_MAXIMO_GRID
        );

        if ((tamanhoLimitado - Compartilhado::Constantes::TAMANHO_INICIAL_GRID) %
                Compartilhado::Constantes::INCREMENTO_TAMANHO_GRID ==
            0) {
            return tamanhoLimitado;
        }

        const int deslocamento = tamanhoLimitado - Compartilhado::Constantes::TAMANHO_INICIAL_GRID;
        return Compartilhado::Constantes::TAMANHO_INICIAL_GRID +
               (deslocamento / Compartilhado::Constantes::INCREMENTO_TAMANHO_GRID) *
                   Compartilhado::Constantes::INCREMENTO_TAMANHO_GRID;
    }

    static int calcularColunaInicialDaGradeAtual(int tamanhoAtualDoGrid) {
        const int tamanhoNormalizado = normalizarTamanhoDaGradeAtual(tamanhoAtualDoGrid);
        return Compartilhado::Constantes::COLUNA_CENTRAL_DA_GRADE_GLOBAL - tamanhoNormalizado / 2;
    }

    static int calcularLinhaInicialDaGradeAtual(int tamanhoAtualDoGrid) {
        const int tamanhoNormalizado = normalizarTamanhoDaGradeAtual(tamanhoAtualDoGrid);
        return Compartilhado::Constantes::LINHA_CENTRAL_DA_GRADE_GLOBAL - tamanhoNormalizado / 2;
    }

    static bool posicaoEstaDentroDaGradeAtual(PosicaoNaGrade posicao, int tamanhoAtualDoGrid) {
        const int tamanhoNormalizado = normalizarTamanhoDaGradeAtual(tamanhoAtualDoGrid);
        const int colunaInicial = calcularColunaInicialDaGradeAtual(tamanhoNormalizado);
        const int linhaInicial = calcularLinhaInicialDaGradeAtual(tamanhoNormalizado);

        return posicao.indiceColuna >= colunaInicial &&
               posicao.indiceColuna < colunaInicial + tamanhoNormalizado &&
               posicao.indiceLinha >= linhaInicial &&
               posicao.indiceLinha < linhaInicial + tamanhoNormalizado;
    }

    static std::size_t calcularIndiceLinearDoTile(PosicaoNaGrade posicao) {
        return static_cast<std::size_t>(posicao.indiceLinha) *
                   static_cast<std::size_t>(Compartilhado::Constantes::QUANTIDADE_DE_COLUNAS_DA_GRADE_GLOBAL) +
               static_cast<std::size_t>(posicao.indiceColuna);
    }

    TileDeTerra* obterTile(PosicaoNaGrade posicao) {
        if (!posicaoEstaDentroDaGradeGlobal(posicao)) {
            return nullptr;
        }

        return &tiles_[calcularIndiceLinearDoTile(posicao)];
    }

    const TileDeTerra* obterTile(PosicaoNaGrade posicao) const {
        if (!posicaoEstaDentroDaGradeGlobal(posicao)) {
            return nullptr;
        }

        return &tiles_[calcularIndiceLinearDoTile(posicao)];
    }

    Canteiros::Canteiro* obterCanteiro(PosicaoNaGrade posicao) {
        TileDeTerra* tile = obterTile(posicao);
        if (tile == nullptr || !tile->existeNoMapa()) {
            return nullptr;
        }

        return &tile->canteiroMutavel();
    }

    const Canteiros::Canteiro* obterCanteiro(PosicaoNaGrade posicao) const {
        const TileDeTerra* tile = obterTile(posicao);
        if (tile == nullptr || !tile->existeNoMapa()) {
            return nullptr;
        }

        return &tile->canteiro();
    }

    const std::vector<PosicaoNaGrade>& posicoesDeTilesExistentes() const {
        return posicoesDeTilesExistentes_;
    }

    const std::vector<PosicaoNaGrade>& posicoesDeCanteirosEmCrescimento() const {
        return posicoesDeCanteirosEmCrescimento_;
    }

    std::size_t quantidadeDeTilesExistentes() const {
        return posicoesDeTilesExistentes_.size();
    }

    std::size_t quantidadeDeCanteirosEmCrescimento() const {
        return posicoesDeCanteirosEmCrescimento_.size();
    }

    TileDeTerra* ativarTile(PosicaoNaGrade posicao) {
        TileDeTerra* tile = obterTile(posicao);
        if (tile == nullptr) {
            return nullptr;
        }

        if (tile->existeNoMapa()) {
            return tile;
        }

        tile->ativar(posicoesDeTilesExistentes_.size());
        posicoesDeTilesExistentes_.push_back(posicao);

        return tile;
    }

    void removerTile(PosicaoNaGrade posicao) {
        TileDeTerra* tile = obterTile(posicao);
        if (tile == nullptr || !tile->existeNoMapa()) {
            return;
        }

        removerCanteiroDaListaDeCrescimento(posicao);

        const std::size_t indiceRemovido = tile->indiceNaListaDeTilesExistentes_;
        if (indiceRemovido >= posicoesDeTilesExistentes_.size()) {
            tile->desativar();
            return;
        }

        const std::size_t ultimoIndice = posicoesDeTilesExistentes_.size() - 1;

        if (indiceRemovido != ultimoIndice) {
            const PosicaoNaGrade posicaoMovida = posicoesDeTilesExistentes_[ultimoIndice];
            posicoesDeTilesExistentes_[indiceRemovido] = posicaoMovida;

            TileDeTerra* tileMovido = obterTile(posicaoMovida);
            if (tileMovido != nullptr) {
                tileMovido->indiceNaListaDeTilesExistentes_ = indiceRemovido;
            }
        }

        posicoesDeTilesExistentes_.pop_back();
        tile->desativar();
    }

    void sincronizarCrescimentoDoCanteiro(PosicaoNaGrade posicao) {
        const TileDeTerra* tile = obterTile(posicao);
        if (tile != nullptr && tile->existeNoMapa() && tile->canteiro().precisaAvancarCrescimento()) {
            registrarCanteiroEmCrescimento(posicao);
            return;
        }

        removerCanteiroDaListaDeCrescimento(posicao);
    }

    void removerCanteiroDaListaDeCrescimento(PosicaoNaGrade posicao) {
        TileDeTerra* tile = obterTile(posicao);
        if (tile == nullptr || tile->indiceNaListaDeCrescimento_ == INDICE_INVALIDO) {
            return;
        }

        const std::size_t indiceRemovido = tile->indiceNaListaDeCrescimento_;
        if (indiceRemovido >= posicoesDeCanteirosEmCrescimento_.size()) {
            tile->indiceNaListaDeCrescimento_ = INDICE_INVALIDO;
            return;
        }

        const std::size_t ultimoIndice = posicoesDeCanteirosEmCrescimento_.size() - 1;

        if (indiceRemovido != ultimoIndice) {
            const PosicaoNaGrade posicaoMovida = posicoesDeCanteirosEmCrescimento_[ultimoIndice];
            posicoesDeCanteirosEmCrescimento_[indiceRemovido] = posicaoMovida;

            TileDeTerra* tileMovido = obterTile(posicaoMovida);
            if (tileMovido != nullptr) {
                tileMovido->indiceNaListaDeCrescimento_ = indiceRemovido;
            }
        }

        posicoesDeCanteirosEmCrescimento_.pop_back();
        tile->indiceNaListaDeCrescimento_ = INDICE_INVALIDO;
    }

private:
    void registrarCanteiroEmCrescimento(PosicaoNaGrade posicao) {
        TileDeTerra* tile = obterTile(posicao);
        if (tile == nullptr ||
            !tile->existeNoMapa() ||
            tile->indiceNaListaDeCrescimento_ != INDICE_INVALIDO ||
            !tile->canteiro().precisaAvancarCrescimento()) {
            return;
        }

        tile->indiceNaListaDeCrescimento_ = posicoesDeCanteirosEmCrescimento_.size();
        posicoesDeCanteirosEmCrescimento_.push_back(posicao);
    }

    std::vector<TileDeTerra> tiles_;
    std::vector<PosicaoNaGrade> posicoesDeTilesExistentes_;
    std::vector<PosicaoNaGrade> posicoesDeCanteirosEmCrescimento_;
};

} // namespace MiniFazenda::Dominio::Grade
