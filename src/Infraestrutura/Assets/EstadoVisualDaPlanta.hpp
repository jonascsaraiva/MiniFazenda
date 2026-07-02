#pragma once

#include "Dominio/Canteiros/EstadoDoCanteiro.hpp"

#include <cstddef>
#include <optional>

namespace MiniFazenda::Infraestrutura::Assets {

inline std::optional<std::size_t> faseVisualDaPlantaParaEstado(
    Dominio::Canteiros::EstadoVisualDoCanteiro estado
) {
    switch (estado) {
        case Dominio::Canteiros::EstadoVisualDoCanteiro::SementePlantada:
            return 0;
        case Dominio::Canteiros::EstadoVisualDoCanteiro::PlantaCrescendo:
            return 1;
        case Dominio::Canteiros::EstadoVisualDoCanteiro::PlantaJovem:
            return 2;
        case Dominio::Canteiros::EstadoVisualDoCanteiro::PlantaMadura:
            return 3;
        case Dominio::Canteiros::EstadoVisualDoCanteiro::PlantaMorta:
            return 4;
        case Dominio::Canteiros::EstadoVisualDoCanteiro::TerraVazia:
        case Dominio::Canteiros::EstadoVisualDoCanteiro::TerraArada:
        case Dominio::Canteiros::EstadoVisualDoCanteiro::Restos:
        default:
            return std::nullopt;
    }
}

inline bool estadoVisualTemPlantaParaDesenho(Dominio::Canteiros::EstadoVisualDoCanteiro estado) {
    return faseVisualDaPlantaParaEstado(estado).has_value();
}

inline std::size_t indiceDaFaseVisualDaPlanta(Dominio::Canteiros::EstadoVisualDoCanteiro estado) {
    return faseVisualDaPlantaParaEstado(estado).value_or(0);
}

} // namespace MiniFazenda::Infraestrutura::Assets
