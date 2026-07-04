#pragma once

#include "Infraestrutura/Assets/GerenciadorDeAtivosSDL.hpp"

#include <SDL.h>

#include <filesystem>
#include <iostream>

namespace MiniFazenda::Infraestrutura::Assets {

struct RecursosDaLoja {
    SDL_Texture* texturaFundo = nullptr;
};

inline std::filesystem::path caminhoDoFundoDaLoja(const std::filesystem::path& diretorioAssets) {
    return diretorioAssets / "sprites" / "loja" / "loja_fundo.png";
}

inline RecursosDaLoja carregarRecursosDaLoja(
    GerenciadorDeAtivosSDL& ativos,
    const std::filesystem::path& diretorioAssets
) {
    RecursosDaLoja recursos;

    const std::filesystem::path caminhoDoFundo = caminhoDoFundoDaLoja(diretorioAssets);
    if (std::filesystem::exists(caminhoDoFundo)) {
        recursos.texturaFundo = ativos.carregarTextura(caminhoDoFundo);
    } else {
        std::cerr << "Fundo da loja ausente. Fallback retangular ativo: "
                  << caminhoDoFundo.string() << '\n';
    }

    return recursos;
}

} // namespace MiniFazenda::Infraestrutura::Assets
