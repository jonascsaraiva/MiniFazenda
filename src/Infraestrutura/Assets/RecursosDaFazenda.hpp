#pragma once

#include "Apresentacao/ConfiguracoesDoLayout.hpp"
#include "Dominio/Canteiros/EstadoDoCanteiro.hpp"
#include "Dominio/Ferramentas/ResultadoDaFerramenta.hpp"
#include "Infraestrutura/Assets/GerenciadorDeAtivosSDL.hpp"
#include "Infraestrutura/Assets/LocalizadorDeAssets.hpp"

#include <SDL.h>

#include <array>
#include <filesystem>
#include <iostream>
#include <vector>

namespace MiniFazenda::Infraestrutura::Assets {

struct TexturasDosCanteiros {
    std::array<SDL_Texture*, Dominio::Canteiros::QUANTIDADE_DE_ESTADOS_DO_CANTEIRO> porEstado{};

    SDL_Texture* paraEstado(Dominio::Canteiros::EstadoVisualDoCanteiro estado) const {
        return porEstado[Dominio::Canteiros::indiceDoEstado(estado)];
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
    texturas.porEstado[Dominio::Canteiros::indiceDoEstado(EstadoVisualDoCanteiro::TerraVazia)] =
        carregarPrimeiraTexturaExistente(ativos, candidatosParaTexturaTerraSeca(diretorioAssets));
    texturas.porEstado[Dominio::Canteiros::indiceDoEstado(EstadoVisualDoCanteiro::TerraArada)] =
        carregarPrimeiraTexturaExistente(ativos, candidatosParaTexturaTerraArada(diretorioAssets));
    texturas.porEstado[Dominio::Canteiros::indiceDoEstado(EstadoVisualDoCanteiro::SementePlantada)] =
        carregarPrimeiraTexturaExistente(ativos, {diretorioAssets / "sprites" / "semente_plantada.png"});
    texturas.porEstado[Dominio::Canteiros::indiceDoEstado(EstadoVisualDoCanteiro::PlantaCrescendo)] =
        carregarPrimeiraTexturaExistente(ativos, {diretorioAssets / "sprites" / "planta_crescendo.png"});
    texturas.porEstado[Dominio::Canteiros::indiceDoEstado(EstadoVisualDoCanteiro::PlantaMadura)] =
        carregarPrimeiraTexturaExistente(ativos, {diretorioAssets / "sprites" / "planta_madura.png"});
    texturas.porEstado[Dominio::Canteiros::indiceDoEstado(EstadoVisualDoCanteiro::PlantaMorta)] =
        carregarPrimeiraTexturaExistente(ativos, candidatosParaTexturaTerraRestos(diretorioAssets));

    return texturas;
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
