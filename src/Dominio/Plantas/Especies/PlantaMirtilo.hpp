#pragma once

#include "Compartilhado/Constantes.hpp"
#include "Dominio/Plantas/Planta.hpp"

#include <memory>
#include <string>

namespace MiniFazenda::Dominio::Plantas::Especies {

class PlantaMirtilo final : public Planta {
public:
    static constexpr int IDENTIFICADOR_DA_SEMENTE = 1;

    std::unique_ptr<Planta> clonar() const override {
        return std::make_unique<PlantaMirtilo>(*this);
    }

    int identificadorDaSemente() const override {
        return IDENTIFICADOR_DA_SEMENTE;
    }

    std::string nome() const override {
        return "Mirtilo";
    }

    int tempoParaCrescer() const override {
        return Compartilhado::Constantes::TEMPO_PARA_CRESCER;
    }

    int tempoParaMaturar() const override {
        return Compartilhado::Constantes::TEMPO_PARA_MADURAR;
    }

    int tempoParaMorrer() const override {
        return Compartilhado::Constantes::TEMPO_PARA_MORRER;
    }

    RecompensaDaColheita recompensaDaColheita() const override {
        return RecompensaDaColheita{8, 5};
    }
};

} // namespace MiniFazenda::Dominio::Plantas::Especies
