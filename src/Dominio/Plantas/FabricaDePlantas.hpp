#pragma once

#include "Dominio/Plantas/Especies/PlantaMirtilo.hpp"
#include "Dominio/Plantas/Planta.hpp"

#include <memory>

namespace MiniFazenda::Dominio::Plantas {

class FabricaDePlantas {
public:
    std::unique_ptr<Planta> criarPlantaInicial() const {
        return criarMirtilo();
    }

    std::unique_ptr<Planta> criarPorIdentificadorDeSemente(int identificadorDaSemente) const {
        if (identificadorDaSemente == Especies::PlantaMirtilo::IDENTIFICADOR_DA_SEMENTE) {
            return criarMirtilo();
        }

        return nullptr;
    }

private:
    std::unique_ptr<Planta> criarMirtilo() const {
        return std::make_unique<Especies::PlantaMirtilo>();
    }
};

} // namespace MiniFazenda::Dominio::Plantas
