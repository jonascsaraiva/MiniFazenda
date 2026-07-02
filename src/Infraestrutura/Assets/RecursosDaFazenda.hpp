#pragma once

#include "Apresentacao/ConfiguracoesDoLayout.hpp"
#include "Dominio/Canteiros/EstadoDoCanteiro.hpp"
#include "Dominio/Ferramentas/ResultadoDaFerramenta.hpp"
#include "Dominio/Ferramentas/TipoDeFerramenta.hpp"
#include "Dominio/Plantas/FabricaDePlantas.hpp"
#include "Dominio/Plantas/Planta.hpp"
#include "Infraestrutura/Assets/CatalogoVisualDePlantas.hpp"
#include "Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp"
#include "Infraestrutura/Assets/EstadoVisualDaPlanta.hpp"
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
using TexturasDoPersonagem = std::array<SDL_Texture*, ConfigVisualDoPersonagem::QUANTIDADE_DE_ANIMACOES>;

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
        if (!estadoVisualTemPlantaParaDesenho(estado)) {
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
    TexturasDoPersonagem texturasDoPersonagem{};
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
        std::cerr << "Background principal não localizado. Fallback verde ativo. Pasta assets: "
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
    SDL_Texture* texturaTerraVazia =
        carregarPrimeiraTexturaExistente(ativos, candidatosParaTexturaTerraSeca(diretorioAssets));
    SDL_Texture* texturaTerraArada =
        carregarPrimeiraTexturaExistente(ativos, candidatosParaTexturaTerraArada(diretorioAssets));
    SDL_Texture* texturaTerraRestos =
        carregarPrimeiraTexturaExistente(ativos, candidatosParaTexturaTerraRestos(diretorioAssets));

    texturas.terraPorEstado[Dominio::Canteiros::indiceDoEstado(EstadoVisualDoCanteiro::TerraVazia)] =
        texturaTerraVazia;
    texturas.terraPorEstado[Dominio::Canteiros::indiceDoEstado(EstadoVisualDoCanteiro::TerraArada)] =
        texturaTerraArada;
    texturas.terraPorEstado[Dominio::Canteiros::indiceDoEstado(EstadoVisualDoCanteiro::SementePlantada)] =
        texturaTerraArada;
    texturas.terraPorEstado[Dominio::Canteiros::indiceDoEstado(EstadoVisualDoCanteiro::PlantaCrescendo)] =
        texturaTerraArada;
    texturas.terraPorEstado[Dominio::Canteiros::indiceDoEstado(EstadoVisualDoCanteiro::PlantaJovem)] =
        texturaTerraArada;
    texturas.terraPorEstado[Dominio::Canteiros::indiceDoEstado(EstadoVisualDoCanteiro::PlantaMadura)] =
        texturaTerraArada;
    texturas.terraPorEstado[Dominio::Canteiros::indiceDoEstado(EstadoVisualDoCanteiro::PlantaMorta)] =
        texturaTerraArada;
    texturas.terraPorEstado[Dominio::Canteiros::indiceDoEstado(EstadoVisualDoCanteiro::Restos)] =
        texturaTerraRestos;

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

inline void validarDimensoesDaTexturaDoPersonagem(
    SDL_Texture* textura,
    const std::filesystem::path& caminho,
    const ConfigVisualDoPersonagem::ConfiguracaoDaAnimacao& configuracao
) {
    if (textura == nullptr) {
        return;
    }

    int largura = 0;
    int altura = 0;
    if (SDL_QueryTexture(textura, nullptr, nullptr, &largura, &altura) != 0) {
        std::cerr << "Dimensoes da textura do personagem nao consultadas: "
                  << caminho.string() << " | " << SDL_GetError() << '\n';
        return;
    }

    if (configuracao.quantidadeFrames <= 0 ||
        configuracao.frameLargura <= 0 ||
        configuracao.frameAltura <= 0) {
        std::cerr << "Configuracao de sprite do personagem invalida: " << caminho.string()
                  << " | frames=" << configuracao.quantidadeFrames
                  << ", frame=" << configuracao.frameLargura << 'x' << configuracao.frameAltura << '\n';
        return;
    }

    const int larguraNecessaria = configuracao.frameOrigemX +
        (configuracao.quantidadeFrames - 1) * (configuracao.frameLargura + configuracao.frameEspacamentoX) +
        configuracao.frameLargura;
    const int alturaNecessaria = configuracao.frameOrigemY + configuracao.frameAltura;

    if (largura < larguraNecessaria || altura < alturaNecessaria) {
        std::cerr << "Spritesheet do personagem fora do recorte configurado: " << caminho.string()
                  << " | textura " << largura << 'x' << altura
                  << ", recorte precisa de " << larguraNecessaria << 'x' << alturaNecessaria << '\n';
    }
}

inline TexturasDoPersonagem carregarTexturasDoPersonagem(
    GerenciadorDeAtivosSDL& ativos,
    const std::filesystem::path& diretorioAssets
) {
    TexturasDoPersonagem texturas{};

    for (std::size_t indice = 0; indice < ConfigVisualDoPersonagem::CONFIGURACOES_POR_ANIMACAO.size(); ++indice) {
        const ConfigVisualDoPersonagem::ConfiguracaoDaAnimacao& configuracao =
            ConfigVisualDoPersonagem::CONFIGURACOES_POR_ANIMACAO[indice];
        const std::filesystem::path caminho = resolverArquivoDentroDeAssets(diretorioAssets, configuracao.caminhoTextura);

        if (!std::filesystem::exists(caminho)) {
            std::cerr << "Sprite do personagem ausente: " << caminho.string() << '\n';
            continue;
        }

        SDL_Texture* textura = ativos.carregarTextura(caminho);
        validarDimensoesDaTexturaDoPersonagem(textura, caminho, configuracao);
        texturas[indice] = textura;
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

        const int identificadorDaSemente = especie->identificadorDaSemente();
        const ConfigVisualDaPlanta* configuracaoVisual =
            configuracaoVisualDaPlantaPorSemente(identificadorDaSemente);

        if (configuracaoVisual == nullptr) {
            std::cerr << "Catalogo visual de planta ausente para " << especie->nome()
                      << " (semente " << identificadorDaSemente << "). Sprites da planta nao carregados.\n";
            continue;
        }

        TexturasDaPlantaPorFase texturasDaPlanta{};
        for (std::size_t indice = 0; indice < configuracaoVisual->arquivosPorFase.size(); ++indice) {
            const std::filesystem::path caminho =
                caminhoDoSpriteDaPlanta(diretorioAssets, *configuracaoVisual, indice);

            if (!std::filesystem::exists(caminho)) {
                std::cerr << "Sprite ausente para " << especie->nome() << " na fase "
                          << nomeDaFaseVisualDaPlanta(indice) << ": " << caminho.string() << '\n';
                continue;
            }

            SpriteDaPlanta sprite = calcularAncoraDaBaseDoSpriteDaPlanta(caminho);
            sprite.textura = ativos.carregarTextura(caminho);
            texturasDaPlanta[indice] = sprite;
        }

        texturasPorSemente[identificadorDaSemente] = texturasDaPlanta;
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

        const int identificadorDaSemente = especie->identificadorDaSemente();
        const ConfigVisualDaPlanta* configuracaoVisual =
            configuracaoVisualDaPlantaPorSemente(identificadorDaSemente);

        if (configuracaoVisual == nullptr) {
            std::cerr << "Catalogo visual de planta ausente para " << especie->nome()
                      << " (semente " << identificadorDaSemente << "). Icone de semente nao carregado.\n";
            continue;
        }

        const std::filesystem::path caminho = caminhoDoIconeDaSemente(diretorioAssets, *configuracaoVisual);
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

        texturasPorSemente[identificadorDaSemente] = textura;
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
    recursos.texturasDoPersonagem = carregarTexturasDoPersonagem(ativos, diretorioAssets);
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
        case Dominio::Ferramentas::AcaoDaFerramenta::LimparCanteiro:
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
