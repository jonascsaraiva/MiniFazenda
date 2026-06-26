#pragma once

#include "Apresentacao/Renderizacao/UI/BarraDeFerramentasRenderer.hpp"
#include "Dominio/Ferramentas/TipoDeFerramenta.hpp"

#include <SDL.h>

namespace MiniFazenda::Apresentacao::Renderizacao::Cursores {

inline void desenharCursorCustomizado(
    SDL_Renderer* renderizador,
    int mouseX,
    int mouseY,
    Dominio::Ferramentas::TipoDeFerramenta ferramenta
) {
    SDL_Rect area{mouseX, mouseY, 38, 38};
    const SDL_Color cor = SDL_Color{255, 252, 218, 255};
    UI::desenharIconeDaFerramenta(renderizador, area, cor, ferramenta);
}

} // namespace MiniFazenda::Apresentacao::Renderizacao::Cursores
