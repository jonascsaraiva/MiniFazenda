#pragma once

#include "Apresentacao/Renderizacao/Primitivas/PrimitivasSDL.hpp"
#include "Compartilhado/ConstantesDaJanela.hpp"

#include <SDL.h>
#include <SDL_ttf.h>

#include <algorithm>
#include <string>

namespace MiniFazenda::Apresentacao::Renderizacao::UI::TooltipDoCanteiroRenderer {

inline SDL_Rect calcularAreaDoTooltip(int cursorX, int cursorY, int larguraDoTexto, int alturaDoTexto) {
    constexpr int margemInternaHorizontal = 8;
    constexpr int margemInternaVertical = 5;
    constexpr int distanciaDoCursor = 14;
    constexpr int margemDaJanela = 6;

    SDL_Rect area{
        cursorX + distanciaDoCursor,
        cursorY + distanciaDoCursor,
        larguraDoTexto + margemInternaHorizontal * 2,
        alturaDoTexto + margemInternaVertical * 2
    };

    if (area.x + area.w > Compartilhado::Constantes::LARGURA_DA_JANELA - margemDaJanela) {
        area.x = cursorX - area.w - distanciaDoCursor;
    }

    if (area.y + area.h > Compartilhado::Constantes::ALTURA_DA_JANELA - margemDaJanela) {
        area.y = cursorY - area.h - distanciaDoCursor;
    }

    const int limiteMaximoX = std::max(margemDaJanela, Compartilhado::Constantes::LARGURA_DA_JANELA - area.w - margemDaJanela);
    const int limiteMaximoY = std::max(margemDaJanela, Compartilhado::Constantes::ALTURA_DA_JANELA - area.h - margemDaJanela);
    area.x = std::clamp(area.x, margemDaJanela, limiteMaximoX);
    area.y = std::clamp(area.y, margemDaJanela, limiteMaximoY);

    return area;
}

inline void desenharTooltipDoCanteiro(
    SDL_Renderer* renderizador,
    TTF_Font* fonte,
    const std::string& texto,
    int cursorX,
    int cursorY
) {
    if (renderizador == nullptr || fonte == nullptr || texto.empty()) {
        return;
    }

    int larguraDoTexto = 0;
    int alturaDoTexto = 0;
    if (TTF_SizeUTF8(fonte, texto.c_str(), &larguraDoTexto, &alturaDoTexto) != 0) {
        return;
    }

    constexpr int margemInternaHorizontal = 8;
    constexpr int margemInternaVertical = 5;
    const SDL_Rect area = calcularAreaDoTooltip(cursorX, cursorY, larguraDoTexto, alturaDoTexto);
    const SDL_Rect areaDoTexto{
        area.x + margemInternaHorizontal,
        area.y + margemInternaVertical,
        larguraDoTexto,
        alturaDoTexto
    };

    Primitivas::preencherRetangulo(renderizador, area, SDL_Color{35, 33, 30, 224});
    Primitivas::definirCor(renderizador, SDL_Color{255, 245, 180, 180});
    SDL_RenderDrawRect(renderizador, &area);
    Primitivas::desenharTexto(
        renderizador,
        fonte,
        texto,
        areaDoTexto.x,
        areaDoTexto.y,
        SDL_Color{255, 245, 180, 255}
    );
}

} // namespace MiniFazenda::Apresentacao::Renderizacao::UI::TooltipDoCanteiroRenderer
