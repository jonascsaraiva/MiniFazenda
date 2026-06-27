#pragma once

#include "Apresentacao/Renderizacao/Primitivas/PrimitivasSDL.hpp"
#include "Compartilhado/Constantes.hpp"
#include "Dominio/Canteiros/Canteiro.hpp"

#include <SDL.h>

namespace MiniFazenda::Apresentacao::Renderizacao::Mundo {

inline SDL_Color corParaEstado(Dominio::Canteiros::EstadoVisualDoCanteiro estado) {
    switch (estado) {
        case Dominio::Canteiros::EstadoVisualDoCanteiro::TerraArada:
            return SDL_Color{118, 77, 45, 255};
        case Dominio::Canteiros::EstadoVisualDoCanteiro::SementePlantada:
            return SDL_Color{96, 67, 40, 255};
        case Dominio::Canteiros::EstadoVisualDoCanteiro::PlantaCrescendo:
            return SDL_Color{78, 144, 74, 255};
        case Dominio::Canteiros::EstadoVisualDoCanteiro::PlantaJovem:
            return SDL_Color{63, 126, 68, 255};
        case Dominio::Canteiros::EstadoVisualDoCanteiro::PlantaMadura:
            return SDL_Color{230, 187, 65, 255};
        case Dominio::Canteiros::EstadoVisualDoCanteiro::PlantaMorta:
            return SDL_Color{92, 84, 78, 255};
        case Dominio::Canteiros::EstadoVisualDoCanteiro::TerraVazia:
        default:
            return SDL_Color{147, 98, 56, 255};
    }
}

inline void desenharFundo(SDL_Renderer* renderizador, SDL_Texture* texturaFundo) {
    SDL_Rect tela{0, 0, Compartilhado::Constantes::LARGURA_DA_JANELA, Compartilhado::Constantes::ALTURA_DA_JANELA};

    Primitivas::preencherRetangulo(renderizador, tela, SDL_Color{99, 157, 86, 255});

    if (texturaFundo != nullptr) {
        SDL_RenderCopy(renderizador, texturaFundo, nullptr, &tela);
    }
}

inline void desenharCanteiro(
    SDL_Renderer* renderizador,
    SDL_Texture* textura,
    const Dominio::Canteiros::Canteiro& canteiro,
    SDL_Rect destino,
    bool destacado
) {
    if (textura != nullptr) {
        SDL_RenderCopy(renderizador, textura, nullptr, &destino);
        if (destacado) {
            Primitivas::desenharContornoLosango(renderizador, destino, SDL_Color{255, 244, 169, 255});
        }
    } else {
        SDL_Color borda = destacado ? SDL_Color{255, 244, 169, 255} : SDL_Color{79, 56, 38, 255};
        Primitivas::desenharLosango(renderizador, destino, corParaEstado(canteiro.estadoVisualAtual()), borda);

        if (canteiro.estadoVisualAtual() == Dominio::Canteiros::EstadoVisualDoCanteiro::SementePlantada) {
            Primitivas::preencherRetangulo(
                renderizador,
                SDL_Rect{destino.x + destino.w / 2 - 3, destino.y + destino.h / 2 - 3, 6, 6},
                SDL_Color{47, 37, 23, 255}
            );
        }

        if (canteiro.estadoVisualAtual() == Dominio::Canteiros::EstadoVisualDoCanteiro::PlantaCrescendo ||
            canteiro.estadoVisualAtual() == Dominio::Canteiros::EstadoVisualDoCanteiro::PlantaJovem ||
            canteiro.estadoVisualAtual() == Dominio::Canteiros::EstadoVisualDoCanteiro::PlantaMadura) {
            const SDL_Color corPlanta =
                canteiro.estadoVisualAtual() == Dominio::Canteiros::EstadoVisualDoCanteiro::PlantaMadura
                    ? SDL_Color{255, 222, 91, 255}
                    : SDL_Color{44, 123, 64, 255};

            Primitivas::preencherRetangulo(
                renderizador,
                SDL_Rect{destino.x + destino.w / 2 - 6, destino.y + destino.h / 2 - 18, 12, 20},
                corPlanta
            );
            Primitivas::preencherRetangulo(
                renderizador,
                SDL_Rect{destino.x + destino.w / 2 - 18, destino.y + destino.h / 2 - 9, 36, 8},
                corPlanta
            );
        }
    }

    if (destacado) {
        Primitivas::desenharLosango(renderizador, destino, SDL_Color{255, 255, 255, 26}, SDL_Color{255, 244, 169, 255});
    }
}

} // namespace MiniFazenda::Apresentacao::Renderizacao::Mundo
