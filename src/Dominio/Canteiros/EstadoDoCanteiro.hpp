#pragma once

#include <cstddef>

namespace MiniFazenda::Dominio::Canteiros {

enum class EstadoVisualDoCanteiro {
    TerraVazia = 0,
    TerraArada = 1,
    SementePlantada = 2,
    PlantaCrescendo = 3,
    PlantaJovem = 4,
    PlantaMadura = 5,
    PlantaMorta = 6
};

constexpr std::size_t QUANTIDADE_DE_ESTADOS_DO_CANTEIRO = 7;

inline std::size_t indiceDoEstado(EstadoVisualDoCanteiro estado) {
    return static_cast<std::size_t>(estado);
}

} // namespace MiniFazenda::Dominio::Canteiros
