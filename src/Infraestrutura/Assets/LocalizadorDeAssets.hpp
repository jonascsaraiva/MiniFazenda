#pragma once

#include "Apresentacao/ConfiguracoesDoLayout.hpp"
#include "Dominio/Ferramentas/TipoDeFerramenta.hpp"

#include <SDL.h>

#include <array>
#include <filesystem>
#include <string>
#include <vector>

namespace MiniFazenda::Infraestrutura::Assets {

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
    const Apresentacao::ConfiguracoesDoLayout& configuracoes
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

inline std::filesystem::path candidatosParaIconeDaFerramenta(
    const std::filesystem::path& diretorioAssets,
    Dominio::Ferramentas::TipoDeFerramenta ferramenta
) {
    const std::filesystem::path diretorioToolbar = diretorioAssets / "sprites" / "icons" / "toolbar";

    switch (ferramenta) {
        case Dominio::Ferramentas::TipoDeFerramenta::Cursor:
            return diretorioToolbar / "cursor.png";
        case Dominio::Ferramentas::TipoDeFerramenta::Enxada:
            return diretorioToolbar / "enxada.png";
        case Dominio::Ferramentas::TipoDeFerramenta::RemoverTerra:
            return diretorioToolbar / "remover_terra.png";
        case Dominio::Ferramentas::TipoDeFerramenta::Semente:
            return diretorioToolbar / "saco_sementes.png";
        case Dominio::Ferramentas::TipoDeFerramenta::Presente:
            return diretorioToolbar / "loja.png";
        default:
            return {};
    }
}

inline std::filesystem::path caminhoDoSpriteDaPlanta(
    const std::filesystem::path& diretorioAssets,
    const std::string& pastaEspecie,
    const std::string& nomeArquivo
) {
    return diretorioAssets / "sprites" / "plantas" / pastaEspecie / nomeArquivo;
}

inline std::array<std::string, 5> nomesDeArquivoPorFaseVisual(const std::string& pastaEspecie) {
    // Contrato do pipeline: SementePlantada, PlantaCrescendo, PlantaJovem,
    // PlantaMadura e PlantaMorta, nessa ordem.
    return {
        pastaEspecie + "_fase_1.png",
        pastaEspecie + "_fase_2.png",
        pastaEspecie + "_fase_3.png",
        pastaEspecie + "_fase_4.png",
        pastaEspecie + "_morto.png"
    };
}

} // namespace MiniFazenda::Infraestrutura::Assets
