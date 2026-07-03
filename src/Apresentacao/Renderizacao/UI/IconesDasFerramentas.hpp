#pragma once

#include "Apresentacao/Renderizacao/Primitivas/PrimitivasSDL.hpp"

#include <SDL.h>

#include <algorithm>

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

inline void desenharLinhaHorizontalGrossa(
    SDL_Renderer* renderizador,
    int xInicial,
    int xFinal,
    int y,
    int espessura
) {
    for (int deslocamento = -espessura / 2; deslocamento <= espessura / 2; ++deslocamento) {
        SDL_RenderDrawLine(renderizador, xInicial, y + deslocamento, xFinal, y + deslocamento);
    }
}

inline void desenharLinhaVerticalGrossa(
    SDL_Renderer* renderizador,
    int x,
    int yInicial,
    int yFinal,
    int espessura
) {
    for (int deslocamento = -espessura / 2; deslocamento <= espessura / 2; ++deslocamento) {
        SDL_RenderDrawLine(renderizador, x + deslocamento, yInicial, x + deslocamento, yFinal);
    }
}

inline void desenharIconeZoomMais(SDL_Renderer* renderizador, SDL_Rect area, SDL_Color cor) {
    Primitivas::definirCor(renderizador, cor);
    const int centroX = area.x + area.w / 2;
    const int centroY = area.y + area.h / 2;
    const int raio = std::max(6, std::min(area.w, area.h) / 4);
    desenharLinhaHorizontalGrossa(renderizador, centroX - raio, centroX + raio, centroY, 3);
    desenharLinhaVerticalGrossa(renderizador, centroX, centroY - raio, centroY + raio, 3);
}

inline void desenharIconeZoomMenos(SDL_Renderer* renderizador, SDL_Rect area, SDL_Color cor) {
    Primitivas::definirCor(renderizador, cor);
    const int centroX = area.x + area.w / 2;
    const int centroY = area.y + area.h / 2;
    const int raio = std::max(6, std::min(area.w, area.h) / 4);
    desenharLinhaHorizontalGrossa(renderizador, centroX - raio, centroX + raio, centroY, 3);
}

inline void desenharIconeAjuda(SDL_Renderer* renderizador, SDL_Rect area, SDL_Color cor) {
    Primitivas::definirCor(renderizador, cor);
    const int centroX = area.x + area.w / 2;
    const int topo = area.y + area.h / 5;
    const int meio = area.y + area.h / 2;
    const int base = area.y + area.h * 3 / 4;
    const int largura = std::max(5, area.w / 5);

    SDL_RenderDrawLine(renderizador, centroX - largura, topo, centroX + largura / 2, topo);
    SDL_RenderDrawLine(renderizador, centroX + largura / 2, topo, centroX + largura, meio - 2);
    SDL_RenderDrawLine(renderizador, centroX + largura, meio - 2, centroX, meio + 3);
    SDL_RenderDrawLine(renderizador, centroX, meio + 3, centroX, base - 4);
    Primitivas::preencherRetangulo(renderizador, SDL_Rect{centroX - 2, base, 4, 4}, cor);
}

inline void desenharIconeEstrela(SDL_Renderer* renderizador, SDL_Rect area, SDL_Color cor) {
    Primitivas::definirCor(renderizador, cor);
    SDL_Point pontos[] = {
        {area.x + area.w / 2, area.y + area.h / 6},
        {area.x + area.w * 60 / 100, area.y + area.h * 40 / 100},
        {area.x + area.w * 85 / 100, area.y + area.h * 40 / 100},
        {area.x + area.w * 65 / 100, area.y + area.h * 56 / 100},
        {area.x + area.w * 74 / 100, area.y + area.h * 82 / 100},
        {area.x + area.w / 2, area.y + area.h * 66 / 100},
        {area.x + area.w * 26 / 100, area.y + area.h * 82 / 100},
        {area.x + area.w * 35 / 100, area.y + area.h * 56 / 100},
        {area.x + area.w * 15 / 100, area.y + area.h * 40 / 100},
        {area.x + area.w * 40 / 100, area.y + area.h * 40 / 100},
        {area.x + area.w / 2, area.y + area.h / 6}
    };
    SDL_RenderDrawLines(renderizador, pontos, 11);
}

inline void desenharIconePresente(SDL_Renderer* renderizador, SDL_Rect area, SDL_Color cor) {
    Primitivas::definirCor(renderizador, cor);
    SDL_Rect caixa{
        area.x + area.w / 4,
        area.y + area.h * 45 / 100,
        area.w / 2,
        area.h * 38 / 100
    };
    SDL_Rect tampa{
        area.x + area.w / 5,
        area.y + area.h * 35 / 100,
        area.w * 3 / 5,
        area.h / 7
    };
    SDL_RenderDrawRect(renderizador, &caixa);
    SDL_RenderDrawRect(renderizador, &tampa);
    SDL_RenderDrawLine(renderizador, area.x + area.w / 2, tampa.y, area.x + area.w / 2, caixa.y + caixa.h);
    SDL_RenderDrawLine(renderizador, caixa.x, caixa.y + caixa.h / 3, caixa.x + caixa.w, caixa.y + caixa.h / 3);
    SDL_RenderDrawLine(renderizador, area.x + area.w / 2, tampa.y, area.x + area.w / 3, area.y + area.h / 5);
    SDL_RenderDrawLine(renderizador, area.x + area.w / 2, tampa.y, area.x + area.w * 2 / 3, area.y + area.h / 5);
}

inline void desenharIconeFoto(SDL_Renderer* renderizador, SDL_Rect area, SDL_Color cor) {
    Primitivas::definirCor(renderizador, cor);
    SDL_Rect corpo{
        area.x + area.w / 5,
        area.y + area.h / 3,
        area.w * 3 / 5,
        area.h * 2 / 5
    };
    SDL_Rect topo{
        area.x + area.w / 3,
        area.y + area.h / 4,
        area.w / 5,
        area.h / 9
    };
    SDL_Rect lente{
        area.x + area.w / 2 - area.w / 10,
        area.y + area.h / 2 - area.h / 10,
        area.w / 5,
        area.h / 5
    };
    SDL_RenderDrawRect(renderizador, &corpo);
    SDL_RenderDrawRect(renderizador, &topo);
    SDL_RenderDrawRect(renderizador, &lente);
}

} // namespace MiniFazenda::Apresentacao::Renderizacao::UI
