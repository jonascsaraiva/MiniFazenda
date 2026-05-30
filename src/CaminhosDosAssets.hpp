#pragma once

#include "Configuracao.hpp"

#include <SDL.h>

#include <filesystem>
#include <string>
#include <vector>

namespace CaminhosDosAssets {

inline std::filesystem::path obterDiretorioBaseDoExecutavel() {
    char* caminhoBase = SDL_GetBasePath();
    if (caminhoBase == nullptr) {
        return std::filesystem::current_path();
    }

    std::filesystem::path diretorio(caminhoBase);
    SDL_free(caminhoBase);
    return diretorio;
}

inline std::filesystem::path localizarDiretorioDeAssets() {
    const std::filesystem::path diretorioBase = obterDiretorioBaseDoExecutavel();

    const std::vector<std::filesystem::path> candidatos = {
        std::filesystem::current_path() / "assets",
        diretorioBase / "assets",
        diretorioBase.parent_path() / "assets",
        diretorioBase.parent_path().parent_path() / "assets"
    };

    for (const std::filesystem::path& candidato : candidatos) {
        if (std::filesystem::exists(candidato / "config.ini")) {
            return candidato;
        }
    }

    return diretorioBase / "assets";
}

inline std::filesystem::path resolverArquivoDentroDeAssets(
    const std::filesystem::path& diretorioAssets,
    const std::filesystem::path& caminhoRelativoOuAbsoluto
) {
    if (caminhoRelativoOuAbsoluto.is_absolute()) {
        return caminhoRelativoOuAbsoluto;
    }

    return diretorioAssets / caminhoRelativoOuAbsoluto;
}

inline std::vector<std::filesystem::path> candidatosParaBackground(
    const std::filesystem::path& diretorioAssets,
    const ConfiguracoesDoLayout& configuracoes
) {
    std::filesystem::path arquivoConfigurado = configuracoes.arquivoBackgroundPrincipal;
    if (arquivoConfigurado.is_relative() && arquivoConfigurado.parent_path().empty()) {
        arquivoConfigurado = std::filesystem::path("background") / arquivoConfigurado;
    }

    return {
        resolverArquivoDentroDeAssets(diretorioAssets, arquivoConfigurado),
        diretorioAssets / "background" / "background.png",
        diretorioAssets / "background" / "plano_de_fundo.png",
        diretorioAssets / "background" / "fazenda.png"
    };
}

inline std::vector<std::filesystem::path> candidatosParaTexturaTerraSeca(const std::filesystem::path& diretorioAssets) {
    return {
        diretorioAssets / "sprites" / "tiles" / "tile_terra_seca.png",
        diretorioAssets / "sprites" / "terra_vazia.png"
    };
}

inline std::vector<std::filesystem::path> candidatosParaTexturaTerraArada(const std::filesystem::path& diretorioAssets) {
    return {
        diretorioAssets / "sprites" / "tiles" / "tile_terra_arada.png",
        diretorioAssets / "sprites" / "terra_arada.png"
    };
}

inline std::vector<std::filesystem::path> candidatosParaTexturaTerraRestos(const std::filesystem::path& diretorioAssets) {
    return {
        diretorioAssets / "sprites" / "tiles" / "tile_terra_restos.png",
        diretorioAssets / "sprites" / "planta_morta.png"
    };
}

} // namespace CaminhosDosAssets
