#pragma once

#include "Apresentacao/ConfiguracoesDoLayout.hpp"
#include "Dominio/Canteiros/EstadoDoCanteiro.hpp"
#include "Dominio/Ferramentas/ResultadoDaFerramenta.hpp"
#include "Dominio/Ferramentas/TipoDeFerramenta.hpp"
#include "Dominio/Plantas/FabricaDePlantas.hpp"
#include "Dominio/Plantas/Planta.hpp"
#include "Infraestrutura/Assets/GerenciadorDeAtivosSDL.hpp"
#include "Infraestrutura/Assets/LocalizadorDeAssets.hpp"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

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

struct SpriteDaPlanta {
    SDL_Texture* textura = nullptr;
    int largura = 0;
    int altura = 0;
    SDL_Point ancoraDaBase{0, 0};
    bool possuiAncora = false;
};

using TexturasDaPlantaPorFase = std::array<SpriteDaPlanta, QUANTIDADE_DE_FASES_VISUAIS_DA_PLANTA>;
using TexturasDasPlantasPorSemente = std::unordered_map<int, TexturasDaPlantaPorFase>;
using TexturasDasSementesPorSemente = std::unordered_map<int, SDL_Texture*>;
using TexturasDosBotoes = std::array<SDL_Texture*, Dominio::Ferramentas::QUANTIDADE_DE_FERRAMENTAS>;

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

    const SpriteDaPlanta* spriteDePlantaParaEstado(
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

        return &encontrada->second[indiceDaFaseVisualDaPlanta(estado)];
    }
};

struct RecursosDaFazenda {
    SDL_Texture* texturaFundo = nullptr;
    TexturasDosCanteiros texturasCanteiro;
    TexturasDosBotoes texturasDosBotoes{};
    TexturasDasSementesPorSemente texturasDasSementes;
};

struct RecursosDeHud {
    TTF_Font* fonte = nullptr;
    SDL_Texture* iconeConfiguracoes = nullptr;
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

inline TexturasDosBotoes carregarTexturasDosBotoes(
    GerenciadorDeAtivosSDL& ativos,
    const std::filesystem::path& diretorioAssets
) {
    TexturasDosBotoes texturas{};

    for (std::size_t indice = 0; indice < Dominio::Ferramentas::QUANTIDADE_DE_FERRAMENTAS; ++indice) {
        const auto ferramenta = static_cast<Dominio::Ferramentas::TipoDeFerramenta>(indice);
        const std::size_t indiceDaTextura = Dominio::Ferramentas::indiceDaFerramenta(ferramenta);
        const std::filesystem::path caminho = candidatosParaIconeDaFerramenta(diretorioAssets, ferramenta);

        if (caminho.empty()) {
            std::cerr << "Icone da toolbar sem mapeamento. Fallback vetorial ativo no indice "
                      << indiceDaTextura << '\n';
            continue;
        }

        if (!std::filesystem::exists(caminho)) {
            std::cerr << "Icone da toolbar ausente. Fallback vetorial ativo: " << caminho.string() << '\n';
            continue;
        }

        SDL_Texture* textura = ativos.carregarTextura(caminho);
        if (textura == nullptr) {
            std::cerr << "Icone da toolbar nao carregado. Fallback vetorial ativo: " << caminho.string() << '\n';
            continue;
        }

        texturas[indiceDaTextura] = textura;
    }

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

inline SpriteDaPlanta calcularAncoraDaBaseDoSpriteDaPlanta(const std::filesystem::path& caminho) {
    SpriteDaPlanta sprite;

    SDL_Surface* superficieOriginal = IMG_Load(caminho.string().c_str());
    if (superficieOriginal == nullptr) {
        std::cerr << "Sprite da planta sem metadados de ancora: " << caminho.string()
                  << " | " << IMG_GetError() << '\n';
        return sprite;
    }

    SDL_Surface* superficie = SDL_ConvertSurfaceFormat(superficieOriginal, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(superficieOriginal);

    if (superficie == nullptr) {
        std::cerr << "Sprite da planta sem conversao RGBA para ancora: " << caminho.string()
                  << " | " << SDL_GetError() << '\n';
        return sprite;
    }

    sprite.largura = superficie->w;
    sprite.altura = superficie->h;

    if (SDL_MUSTLOCK(superficie)) {
        SDL_LockSurface(superficie);
    }

    int minimoX = sprite.largura;
    int maximoX = -1;
    int maximoY = -1;

    const Uint8* pixels = static_cast<const Uint8*>(superficie->pixels);
    for (int y = 0; y < sprite.altura; ++y) {
        const Uint8* linha = pixels + (y * superficie->pitch);
        for (int x = 0; x < sprite.largura; ++x) {
            const Uint32 pixel = *reinterpret_cast<const Uint32*>(linha + (x * 4));
            Uint8 vermelho = 0;
            Uint8 verde = 0;
            Uint8 azul = 0;
            Uint8 alfa = 0;
            SDL_GetRGBA(pixel, superficie->format, &vermelho, &verde, &azul, &alfa);

            if (alfa == 0) {
                continue;
            }

            if (x < minimoX) {
                minimoX = x;
            }
            if (x > maximoX) {
                maximoX = x;
            }
            if (y > maximoY) {
                maximoY = y;
            }
        }
    }

    if (SDL_MUSTLOCK(superficie)) {
        SDL_UnlockSurface(superficie);
    }

    SDL_FreeSurface(superficie);

    if (maximoX < 0 || maximoY < 0) {
        sprite.ancoraDaBase = SDL_Point{sprite.largura / 2, sprite.altura};
        sprite.possuiAncora = true;
        return sprite;
    }

    sprite.ancoraDaBase = SDL_Point{(minimoX + maximoX) / 2, maximoY};
    sprite.possuiAncora = true;
    return sprite;
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

            SpriteDaPlanta sprite = calcularAncoraDaBaseDoSpriteDaPlanta(caminho);
            sprite.textura = ativos.carregarTextura(caminho);
            texturasDaPlanta[indice] = sprite;
        }

        texturasPorSemente[especie->identificadorDaSemente()] = texturasDaPlanta;
    }

    return texturasPorSemente;
}

inline TexturasDasSementesPorSemente carregarIconesDasSementes(
    GerenciadorDeAtivosSDL& ativos,
    const std::filesystem::path& diretorioAssets,
    const std::vector<std::unique_ptr<Dominio::Plantas::Planta>>& especies
) {
    TexturasDasSementesPorSemente texturasPorSemente;

    for (const std::unique_ptr<Dominio::Plantas::Planta>& especie : especies) {
        if (especie == nullptr) {
            continue;
        }

        const std::filesystem::path caminho = caminhoDoIconeDaSemente(diretorioAssets, especie->pastaDeSprites());
        if (!std::filesystem::exists(caminho)) {
            std::cerr << "Icone de semente ausente. Fallback vetorial ativo para "
                      << especie->nome() << ": " << caminho.string() << '\n';
            continue;
        }

        SDL_Texture* textura = ativos.carregarTextura(caminho);
        if (textura == nullptr) {
            std::cerr << "Icone de semente nao carregado. Fallback vetorial ativo para "
                      << especie->nome() << ": " << caminho.string() << '\n';
            continue;
        }

        texturasPorSemente[especie->identificadorDaSemente()] = textura;
    }

    return texturasPorSemente;
}

inline RecursosDaFazenda carregarRecursosDaFazenda(
    GerenciadorDeAtivosSDL& ativos,
    const std::filesystem::path& diretorioAssets,
    const Apresentacao::ConfiguracoesDoLayout& configuracoes,
    const Dominio::Plantas::FabricaDePlantas& fabrica
) {
    RecursosDaFazenda recursos;
    const std::vector<std::unique_ptr<Dominio::Plantas::Planta>> especies = fabrica.todasAsEspecies();
    recursos.texturaFundo = carregarTexturaDeFundoPrincipal(ativos, diretorioAssets, configuracoes);
    recursos.texturasCanteiro = carregarTexturasDosCanteiros(ativos, diretorioAssets);
    recursos.texturasDosBotoes = carregarTexturasDosBotoes(ativos, diretorioAssets);
    recursos.texturasCanteiro.plantasPorSemente = carregarSpritesDeTodasAsEspecies(
        ativos,
        diretorioAssets,
        especies
    );
    recursos.texturasDasSementes = carregarIconesDasSementes(ativos, diretorioAssets, especies);

    return recursos;
}

inline RecursosDeHud carregarRecursosDeHud(
    GerenciadorDeAtivosSDL& ativos,
    const std::filesystem::path& diretorioAssets
) {
    RecursosDeHud recursos;

    const std::filesystem::path caminhoDaFonte = caminhoDaFontePrincipal(diretorioAssets);
    if (std::filesystem::exists(caminhoDaFonte)) {
        recursos.fonte = ativos.carregarFonte(caminhoDaFonte, 16);
    } else {
        std::cerr << "Fonte principal da HUD ausente. Texto da HUD desativado: "
                  << caminhoDaFonte.string() << '\n';
    }

    const std::filesystem::path caminhoDoIcone = caminhoDoIconeDeConfiguracoes(diretorioAssets);
    if (std::filesystem::exists(caminhoDoIcone)) {
        recursos.iconeConfiguracoes = ativos.carregarTextura(caminhoDoIcone);
    } else {
        std::cerr << "Icone de configuracoes ausente. Fallback vetorial ativo: "
                  << caminhoDoIcone.string() << '\n';
    }

    return recursos;
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
