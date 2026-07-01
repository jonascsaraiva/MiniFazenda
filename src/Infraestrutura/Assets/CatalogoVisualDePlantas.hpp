#pragma once

#include "Dominio/Plantas/Especies/PlantaMirtilo.hpp"

#include <array>
#include <cstddef>

namespace MiniFazenda::Infraestrutura::Assets {

inline constexpr std::size_t QUANTIDADE_DE_FASES_VISUAIS_DA_PLANTA = 5;

struct ConfigVisualDaPlanta {
    int identificadorDaSemente = -1;
    const char* pastaDaEspecie = "";
    std::array<const char*, QUANTIDADE_DE_FASES_VISUAIS_DA_PLANTA> arquivosPorFase{};
    const char* arquivoDoIconeDaSemente = "";
};

inline constexpr std::array<ConfigVisualDaPlanta, 1> CATALOGO_VISUAL_DE_PLANTAS = {{
    {
        Dominio::Plantas::Especies::PlantaMirtilo::IDENTIFICADOR_DA_SEMENTE,
        "mirtilo",
        {{
            "mirtilo_fase_1.png",
            "mirtilo_fase_2.png",
            "mirtilo_fase_3.png",
            "mirtilo_fase_4.png",
            "mirtilo_morto.png"
        }},
        "semente_mirtilo.png"
    }
}};

inline const ConfigVisualDaPlanta* configuracaoVisualDaPlantaPorSemente(int identificadorDaSemente) {
    for (const ConfigVisualDaPlanta& configuracao : CATALOGO_VISUAL_DE_PLANTAS) {
        if (configuracao.identificadorDaSemente == identificadorDaSemente) {
            return &configuracao;
        }
    }

    return nullptr;
}

} // namespace MiniFazenda::Infraestrutura::Assets
