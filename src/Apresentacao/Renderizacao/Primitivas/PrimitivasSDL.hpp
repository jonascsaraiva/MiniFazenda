#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

#include <algorithm>
#include <string>

namespace MiniFazenda::Apresentacao::Renderizacao::Primitivas {

inline void definirCor(SDL_Renderer* renderizador, SDL_Color cor) {
    SDL_SetRenderDrawColor(renderizador, cor.r, cor.g, cor.b, cor.a);
}

inline void preencherRetangulo(SDL_Renderer* renderizador, SDL_Rect retangulo, SDL_Color cor) {
    definirCor(renderizador, cor);
    SDL_RenderFillRect(renderizador, &retangulo);
}

inline void desenharLosango(SDL_Renderer* renderizador, SDL_Rect destino, SDL_Color preenchimento, SDL_Color borda) {
    const int centroX = destino.x + destino.w / 2;
    const int topoY = destino.y;
    const int meioY = destino.y + destino.h / 2;
    const int baseY = destino.y + destino.h;
    const int metadeDaLargura = destino.w / 2;
    const int metadeDaAltura = std::max(1, destino.h / 2);

    definirCor(renderizador, preenchimento);
    for (int y = topoY; y <= meioY; ++y) {
        const int distancia = y - topoY;
        const int metadeLinha = (metadeDaLargura * distancia) / metadeDaAltura;
        SDL_RenderDrawLine(renderizador, centroX - metadeLinha, y, centroX + metadeLinha, y);
    }

    for (int y = meioY + 1; y <= baseY; ++y) {
        const int distancia = baseY - y;
        const int metadeLinha = (metadeDaLargura * distancia) / metadeDaAltura;
        SDL_RenderDrawLine(renderizador, centroX - metadeLinha, y, centroX + metadeLinha, y);
    }

    definirCor(renderizador, borda);
    SDL_RenderDrawLine(renderizador, centroX, topoY, destino.x + destino.w, meioY);
    SDL_RenderDrawLine(renderizador, destino.x + destino.w, meioY, centroX, baseY);
    SDL_RenderDrawLine(renderizador, centroX, baseY, destino.x, meioY);
    SDL_RenderDrawLine(renderizador, destino.x, meioY, centroX, topoY);
}

inline void desenharContornoLosango(SDL_Renderer* renderizador, SDL_Rect destino, SDL_Color cor) {
    const int centroX = destino.x + destino.w / 2;
    const int topoY = destino.y;
    const int meioY = destino.y + destino.h / 2;
    const int baseY = destino.y + destino.h;

    definirCor(renderizador, cor);
    SDL_RenderDrawLine(renderizador, centroX, topoY, destino.x + destino.w, meioY);
    SDL_RenderDrawLine(renderizador, destino.x + destino.w, meioY, centroX, baseY);
    SDL_RenderDrawLine(renderizador, centroX, baseY, destino.x, meioY);
    SDL_RenderDrawLine(renderizador, destino.x, meioY, centroX, topoY);
}

inline void desenharTexto(
    SDL_Renderer* renderizador,
    TTF_Font* fonte,
    const std::string& texto,
    int x,
    int y,
    SDL_Color cor
) {
    if (fonte == nullptr || texto.empty()) {
        return;
    }

    SDL_Surface* superficie = TTF_RenderUTF8_Blended(fonte, texto.c_str(), cor);
    if (superficie == nullptr) {
        return;
    }

    SDL_Texture* textura = SDL_CreateTextureFromSurface(renderizador, superficie);
    SDL_Rect destino{x, y, superficie->w, superficie->h};
    SDL_FreeSurface(superficie);

    if (textura == nullptr) {
        return;
    }

    SDL_RenderCopy(renderizador, textura, nullptr, &destino);
    SDL_DestroyTexture(textura);
}

} // namespace MiniFazenda::Apresentacao::Renderizacao::Primitivas
