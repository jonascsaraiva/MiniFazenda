#pragma once

#include "Ativos.hpp"
#include "CaminhosDosAssets.hpp"
#include "SistemasDoJogo.hpp"
#include "Tipos.hpp"

#include <SDL.h>

#include <array>
#include <filesystem>
#include <iostream>
#include <vector>

struct TexturasDosCanteiros {
    std::array<SDL_Texture*, QUANTIDADE_DE_ESTADOS_DO_CANTEIRO> porEstado{};

    SDL_Texture* paraEstado(EstadoDoCanteiro estado) const {
        return porEstado[static_cast<std::size_t>(estado)];
    }
};

inline SDL_Texture* carregarPrimeiraTexturaExistente(
    GerenciadorDeAtivos& ativos,
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
    GerenciadorDeAtivos& ativos,
    const std::filesystem::path& diretorioAssets,
    const ConfiguracoesDoLayout& configuracoes
) {
    SDL_Texture* textura = carregarPrimeiraTexturaExistente(
        ativos,
        CaminhosDosAssets::candidatosParaBackground(diretorioAssets, configuracoes)
    );

    if (textura == nullptr) {
        std::cerr << "Background principal nao localizado. Fallback verde ativo. Pasta assets: "
                  << diretorioAssets.string() << '\n';
    }

    return textura;
}

inline TexturasDosCanteiros carregarTexturasDosCanteiros(
    GerenciadorDeAtivos& ativos,
    const std::filesystem::path& diretorioAssets
) {
    TexturasDosCanteiros texturas;
    texturas.porEstado[ESTADO_TERRA_VAZIA] =
        carregarPrimeiraTexturaExistente(ativos, CaminhosDosAssets::candidatosParaTexturaTerraSeca(diretorioAssets));
    texturas.porEstado[ESTADO_TERRA_ARADA] =
        carregarPrimeiraTexturaExistente(ativos, CaminhosDosAssets::candidatosParaTexturaTerraArada(diretorioAssets));
    texturas.porEstado[ESTADO_SEMENTE_PLANTADA] =
        carregarPrimeiraTexturaExistente(ativos, {diretorioAssets / "sprites" / "semente_plantada.png"});
    texturas.porEstado[ESTADO_PLANTA_CRESCENDO] =
        carregarPrimeiraTexturaExistente(ativos, {diretorioAssets / "sprites" / "planta_crescendo.png"});
    texturas.porEstado[ESTADO_PLANTA_MADURA] =
        carregarPrimeiraTexturaExistente(ativos, {diretorioAssets / "sprites" / "planta_madura.png"});
    texturas.porEstado[ESTADO_PLANTA_MORTA] =
        carregarPrimeiraTexturaExistente(ativos, CaminhosDosAssets::candidatosParaTexturaTerraRestos(diretorioAssets));

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
    AcaoDaFerramenta acao
) {
    switch (acao) {
        case ACAO_PLANTAR:
            return diretorioAssets / "sounds" / "buy.wav";
        case ACAO_ACELERAR_CRESCIMENTO:
            return diretorioAssets / "sounds" / "gift_open.wav";
        case ACAO_COLHER:
            return diretorioAssets / "sounds" / "harvest.wav";
        case ACAO_REMOVER_TERRA:
            return diretorioAssets / "sounds" / "weed.wav";
        case ACAO_CRIAR_TERRA:
        case ACAO_ARAR_TERRA:
            return diretorioAssets / "sounds" / "click_panel.wav";
        case ACAO_NENHUMA:
        default:
            return {};
    }
}

inline void tocarSomDaAcao(
    GerenciadorDeAtivos& ativos,
    const std::filesystem::path& diretorioAssets,
    ResultadoDaFerramenta resultado
) {
    if (!resultado.houveMudanca()) {
        return;
    }

    const std::filesystem::path caminho = caminhoDoSomDaAcao(diretorioAssets, resultado.acao);
    if (!caminho.empty()) {
        ativos.tocarSom(caminho);
    }
}
