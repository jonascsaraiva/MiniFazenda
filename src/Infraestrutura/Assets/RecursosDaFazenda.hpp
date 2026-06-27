#pragma once

#include "Apresentacao/ConfiguracoesDoLayout.hpp"
#include "Dominio/Canteiros/EstadoDoCanteiro.hpp"
#include "Dominio/Ferramentas/ResultadoDaFerramenta.hpp"
#include "Dominio/Plantas/Planta.hpp"
#include "Infraestrutura/Assets/GerenciadorDeAtivosSDL.hpp"
#include "Infraestrutura/Assets/LocalizadorDeAssets.hpp"

#include <SDL.h>

#include <array>
#include <cstddef>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace MiniFazenda::Infraestrutura::Assets {

constexpr std::size_t QUANTIDADE_DE_FASES_VISUAIS_DA_PLANTA = 5;

using TexturasDaPlantaPorFase = std::array<SDL_Texture*, QUANTIDADE_DE_FASES_VISUAIS_DA_PLANTA>;
using TexturasDasPlantasPorSemente = std::unordered_map<int, TexturasDaPlantaPorFase>;

inline bool estadoEhFaseVisualDaPlanta(Dominio::Canteiros::EstadoVisualDoCanteiro estado) {
    switch (estado) {
        case Dominio::Canteiros::EstadoVisualDoCanteiro::SementePlantada:
        case Dominio::Canteiros::EstadoVisualDoCanteiro::PlantaCrescendo:
        case Dominio::Canteiros::EstadoVisualDoCanteiro::PlantaJovem:
        case Dominio::Canteiros::EstadoVisualDoCanteiro::PlantaMadura:
        case Dominio::Canteiros::EstadoVisualDoCanteiro::PlantaMorta:
            return true;
        case Dominio::Canteiros::EstadoVisualDoCanteiro::TerraVazia:
        case Dominio::Canteiros::EstadoVisualDoCanteiro::TerraArada:
        default:
            return false;
    }
}

inline std::size_t indiceDaFaseVisualDaPlanta(Dominio::Canteiros::EstadoVisualDoCanteiro estado) {
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
        default:
            return 0;
    }
}

struct TexturasDosCanteiros {
    std::array<SDL_Texture*, Dominio::Canteiros::QUANTIDADE_DE_ESTADOS_DO_CANTEIRO> terraPorEstado{};
    TexturasDasPlantasPorSemente plantasPorSemente;

    SDL_Texture* texturaDeTerraParaEstado(Dominio::Canteiros::EstadoVisualDoCanteiro estado) const {
        return terraPorEstado[Dominio::Canteiros::indiceDoEstado(estado)];
    }

    SDL_Texture* texturaDePlantaParaEstado(
        int identificadorDaSemente,
        Dominio::Canteiros::EstadoVisualDoCanteiro estado
    ) const {
        if (!estadoEhFaseVisualDaPlanta(estado)) {
            return nullptr;
        }

        const auto encontrada = plantasPorSemente.find(identificadorDaSemente);
        if (encontrada == plantasPorSemente.end()) {
            return nullptr;
        }

        return encontrada->second[indiceDaFaseVisualDaPlanta(estado)];
    }
};

inline SDL_Texture* carregarPrimeiraTexturaExistente(
    GerenciadorDeAtivosSDL& ativos,
    const std::vector<std::filesystem::path>& candidatos
) {
    for (const std::filesystem::path& candidato : candidatos) {
        if (!std::filesystem::exists(candidato)) {
            continue;
        }

        SDL_Texture* textura = ativos.carregarTextura(candidato);
        if (textura != nullptr) {
            return textura;
        }
    }

    return nullptr;
}

inline SDL_Texture* carregarTexturaDeFundoPrincipal(
    GerenciadorDeAtivosSDL& ativos,
    const std::filesystem::path& diretorioAssets,
    const Apresentacao::ConfiguracoesDoLayout& configuracoes
) {
    SDL_Texture* textura = carregarPrimeiraTexturaExistente(
        ativos,
        candidatosParaBackground(diretorioAssets, configuracoes)
    );

    if (textura == nullptr) {
        std::cerr << "Background principal nao localizado. Fallback verde ativo. Pasta assets: "
                  << diretorioAssets.string() << '\n';
    }

    return textura;
}

inline TexturasDosCanteiros carregarTexturasDosCanteiros(
    GerenciadorDeAtivosSDL& ativos,
    const std::filesystem::path& diretorioAssets
) {
    using Dominio::Canteiros::EstadoVisualDoCanteiro;

    TexturasDosCanteiros texturas;
    texturas.terraPorEstado[Dominio::Canteiros::indiceDoEstado(EstadoVisualDoCanteiro::TerraVazia)] =
        carregarPrimeiraTexturaExistente(ativos, candidatosParaTexturaTerraSeca(diretorioAssets));
    texturas.terraPorEstado[Dominio::Canteiros::indiceDoEstado(EstadoVisualDoCanteiro::TerraArada)] =
        carregarPrimeiraTexturaExistente(ativos, candidatosParaTexturaTerraArada(diretorioAssets));
    texturas.terraPorEstado[Dominio::Canteiros::indiceDoEstado(EstadoVisualDoCanteiro::PlantaMorta)] =
        carregarPrimeiraTexturaExistente(ativos, candidatosParaTexturaTerraRestos(diretorioAssets));

    return texturas;
}

inline const char* nomeDaFaseVisualDaPlanta(std::size_t indice) {
    static constexpr std::array<const char*, QUANTIDADE_DE_FASES_VISUAIS_DA_PLANTA> nomes = {
        "SementePlantada",
        "PlantaCrescendo",
        "PlantaJovem",
        "PlantaMadura",
        "PlantaMorta"
    };

    return indice < nomes.size() ? nomes[indice] : "FaseDesconhecida";
}

inline TexturasDasPlantasPorSemente carregarSpritesDeTodasAsEspecies(
    GerenciadorDeAtivosSDL& ativos,
    const std::filesystem::path& diretorioAssets,
    const std::vector<std::unique_ptr<Dominio::Plantas::Planta>>& especies
) {
    TexturasDasPlantasPorSemente texturasPorSemente;

    for (const std::unique_ptr<Dominio::Plantas::Planta>& especie : especies) {
        if (especie == nullptr) {
            continue;
        }

        const std::string pastaEspecie = especie->pastaDeSprites();
        const std::array<std::string, QUANTIDADE_DE_FASES_VISUAIS_DA_PLANTA> nomesArquivos =
            nomesDeArquivoPorFaseVisual(pastaEspecie);

        TexturasDaPlantaPorFase texturasDaPlanta{};
        for (std::size_t indice = 0; indice < nomesArquivos.size(); ++indice) {
            const std::filesystem::path caminho =
                caminhoDoSpriteDaPlanta(diretorioAssets, pastaEspecie, nomesArquivos[indice]);

            if (!std::filesystem::exists(caminho)) {
                std::cerr << "Sprite ausente para " << especie->nome() << " na fase "
                          << nomeDaFaseVisualDaPlanta(indice) << ": " << caminho.string() << '\n';
                continue;
            }

            texturasDaPlanta[indice] = ativos.carregarTextura(caminho);
        }

        texturasPorSemente[especie->identificadorDaSemente()] = texturasDaPlanta;
    }

    return texturasPorSemente;
}

inline std::filesystem::path caminhoDaMusicaAmbiente(const std::filesystem::path& diretorioAssets) {
    return diretorioAssets / "sounds" / "minifazenda-background-som-loop.mp3";
}

inline std::filesystem::path caminhoDoSomDeCliqueDaInterface(const std::filesystem::path& diretorioAssets) {
    return diretorioAssets / "sounds" / "click_button.wav";
}

inline std::filesystem::path caminhoDoSomDaAcao(
    const std::filesystem::path& diretorioAssets,
    Dominio::Ferramentas::AcaoDaFerramenta acao
) {
    switch (acao) {
        case Dominio::Ferramentas::AcaoDaFerramenta::Plantar:
            return diretorioAssets / "sounds" / "buy.wav";
        case Dominio::Ferramentas::AcaoDaFerramenta::AcelerarCrescimento:
            return diretorioAssets / "sounds" / "gift_open.wav";
        case Dominio::Ferramentas::AcaoDaFerramenta::Colher:
            return diretorioAssets / "sounds" / "harvest.wav";
        case Dominio::Ferramentas::AcaoDaFerramenta::RemoverTerra:
            return diretorioAssets / "sounds" / "weed.wav";
        case Dominio::Ferramentas::AcaoDaFerramenta::CriarTerra:
        case Dominio::Ferramentas::AcaoDaFerramenta::ArarTerra:
            return diretorioAssets / "sounds" / "click_panel.wav";
        case Dominio::Ferramentas::AcaoDaFerramenta::Nenhuma:
        default:
            return {};
    }
}

inline void tocarSomDaAcao(
    GerenciadorDeAtivosSDL& ativos,
    const std::filesystem::path& diretorioAssets,
    Dominio::Ferramentas::ResultadoDaFerramenta resultado
) {
    if (!resultado.houveMudanca()) {
        return;
    }

    const std::filesystem::path caminho = caminhoDoSomDaAcao(diretorioAssets, resultado.acao);
    if (!caminho.empty()) {
        ativos.tocarSom(caminho);
    }
}

} // namespace MiniFazenda::Infraestrutura::Assets
