#pragma once

#include "Apresentacao/Renderizacao/Primitivas/PrimitivasSDL.hpp"

#include <SDL.h>

namespace MiniFazenda::Apresentacao::Renderizacao::UI {

inline void desenharIconeCursor(SDL_Renderer* renderizador, SDL_Rect area, SDL_Color cor) {
    Primitivas::definirCor(renderizador, cor);
    SDL_Point pontos[] = {
        {area.x + 16, area.y + 12},
        {area.x + 16, area.y + 38},
        {area.x + 24, area.y + 31},
        {area.x + 30, area.y + 42},
        {area.x + 36, area.y + 39},
        {area.x + 30, area.y + 28},
        {area.x + 40, area.y + 28},
        {area.x + 16, area.y + 12}
    };
    SDL_RenderDrawLines(renderizador, pontos, 8);
}

inline void desenharIconeEnxada(SDL_Renderer* renderizador, SDL_Rect area, SDL_Color cor) {
    Primitivas::definirCor(renderizador, cor);
    SDL_RenderDrawLine(renderizador, area.x + 17, area.y + 39, area.x + 36, area.y + 16);
    SDL_RenderDrawLine(renderizador, area.x + 31, area.y + 15, area.x + 42, area.y + 24);
    SDL_RenderDrawLine(renderizador, area.x + 27, area.y + 19, area.x + 38, area.y + 28);
}

inline void desenharIconeSemente(SDL_Renderer* renderizador, SDL_Rect area, SDL_Color cor) {
    Primitivas::preencherRetangulo(renderizador, SDL_Rect{area.x + 23, area.y + 25, 7, 7}, cor);
    Primitivas::preencherRetangulo(renderizador, SDL_Rect{area.x + 18, area.y + 32, 7, 7}, cor);
    Primitivas::preencherRetangulo(renderizador, SDL_Rect{area.x + 30, area.y + 32, 7, 7}, cor);
    Primitivas::definirCor(renderizador, cor);
    SDL_RenderDrawLine(renderizador, area.x + 27, area.y + 25, area.x + 36, area.y + 15);
    SDL_RenderDrawLine(renderizador, area.x + 36, area.y + 15, area.x + 42, area.y + 18);
}

inline void desenharIconeLoja(SDL_Renderer* renderizador, SDL_Rect area, SDL_Color cor) {
    Primitivas::definirCor(renderizador, cor);
    SDL_RenderDrawLine(renderizador, area.x + 16, area.y + 18, area.x + 40, area.y + 18);
    SDL_RenderDrawLine(renderizador, area.x + 16, area.y + 18, area.x + 12, area.y + 27);
    SDL_RenderDrawLine(renderizador, area.x + 40, area.y + 18, area.x + 44, area.y + 27);
    Primitivas::preencherRetangulo(renderizador, SDL_Rect{area.x + 14, area.y + 28, 28, 17}, cor);
    Primitivas::preencherRetangulo(renderizador, SDL_Rect{area.x + 18, area.y + 32, 8, 13}, SDL_Color{255, 242, 178, 255});
    Primitivas::preencherRetangulo(renderizador, SDL_Rect{area.x + 30, area.y + 32, 8, 6}, SDL_Color{255, 242, 178, 255});
}

inline void desenharIconeRemoverTerra(SDL_Renderer* renderizador, SDL_Rect area, SDL_Color cor) {
    Primitivas::definirCor(renderizador, cor);
    SDL_RenderDrawLine(renderizador, area.x + 15, area.y + 15, area.x + 39, area.y + 39);
    SDL_RenderDrawLine(renderizador, area.x + 39, area.y + 15, area.x + 15, area.y + 39);
    SDL_Rect quadrado{area.x + 19, area.y + 19, 16, 16};
    SDL_RenderDrawRect(renderizador, &quadrado);
}

} // namespace MiniFazenda::Apresentacao::Renderizacao::UI
