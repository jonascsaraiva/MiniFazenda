#pragma once

#include "Dominio/Canteiros/Canteiro.hpp"

#include <cstddef>
#include <limits>

namespace MiniFazenda::Dominio::Grade {

constexpr std::size_t INDICE_INVALIDO = std::numeric_limits<std::size_t>::max();

class TileDeTerra {
public:
    bool existeNoMapa() const {
        return existeNoMapa_;
    }

    const Canteiros::Canteiro& canteiro() const {
        return canteiro_;
    }

    std::size_t indiceNaListaDeTilesExistentes() const {
        return indiceNaListaDeTilesExistentes_;
    }

    std::size_t indiceNaListaDeCrescimento() const {
        return indiceNaListaDeCrescimento_;
    }

private:
    friend class GradeGlobalDeCanteiros;

    Canteiros::Canteiro& canteiroMutavel() {
        return canteiro_;
    }

    void ativar(std::size_t indiceNaLista) {
        existeNoMapa_ = true;
        canteiro_ = Canteiros::Canteiro{};
        indiceNaListaDeTilesExistentes_ = indiceNaLista;
        indiceNaListaDeCrescimento_ = INDICE_INVALIDO;
    }

    void desativar() {
        existeNoMapa_ = false;
        canteiro_ = Canteiros::Canteiro{};
        indiceNaListaDeTilesExistentes_ = INDICE_INVALIDO;
        indiceNaListaDeCrescimento_ = INDICE_INVALIDO;
    }

    bool existeNoMapa_ = false;
    Canteiros::Canteiro canteiro_;
    std::size_t indiceNaListaDeTilesExistentes_ = INDICE_INVALIDO;
    std::size_t indiceNaListaDeCrescimento_ = INDICE_INVALIDO;
};

} // namespace MiniFazenda::Dominio::Grade
