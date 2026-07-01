#pragma once

#include "Dominio/Plantas/Especies/PlantaMirtilo.hpp"
#include "Dominio/Plantas/Planta.hpp"

#include <memory>
#include <vector>

namespace MiniFazenda::Dominio::Plantas {

class FabricaDePlantas {
public:
    std::unique_ptr<Planta> criarPorIdentificadorDeSemente(int identificadorDaSemente) const {
        if (identificadorDaSemente == Especies::PlantaMirtilo::IDENTIFICADOR_DA_SEMENTE) {
            return criarMirtilo();
        }

        return nullptr;
    }

    std::vector<std::unique_ptr<Planta>> todasAsEspecies() const {
        std::vector<std::unique_ptr<Planta>> especies;
        especies.push_back(criarMirtilo());
        return especies;
    }

private:
    std::unique_ptr<Planta> criarMirtilo() const {
        return std::make_unique<Especies::PlantaMirtilo>();
    }
};

} // namespace MiniFazenda::Dominio::Plantas
