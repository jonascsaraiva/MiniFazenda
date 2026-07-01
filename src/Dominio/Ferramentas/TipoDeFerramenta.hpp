#pragma once

#include <cstddef>

namespace MiniFazenda::Dominio::Ferramentas {

enum class TipoDeFerramenta {
    Cursor = 0,
    Enxada = 1,
    RemoverTerra = 2,
    Semente = 3,
    Loja = 4
};

constexpr std::size_t QUANTIDADE_DE_FERRAMENTAS = 5;

inline std::size_t indiceDaFerramenta(TipoDeFerramenta ferramenta) {
    return static_cast<std::size_t>(ferramenta);
}

} // namespace MiniFazenda::Dominio::Ferramentas
