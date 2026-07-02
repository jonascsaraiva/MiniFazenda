#pragma once

#include "Compartilhado/ConstantesDaIsometria.hpp"

#include <string>

namespace MiniFazenda::Apresentacao {

struct ConfiguracoesDoLayout {
    int centroVisualBackgroundX = Compartilhado::Constantes::CENTRO_VISUAL_BACKGROUND_X;
    int centroVisualBackgroundY = Compartilhado::Constantes::CENTRO_VISUAL_BACKGROUND_Y;
    int origemGradeHorizontal = Compartilhado::Constantes::CENTRO_VISUAL_BACKGROUND_X;
    int origemGradeVertical = Compartilhado::Constantes::CENTRO_VISUAL_BACKGROUND_Y;

    std::string arquivoBackgroundPrincipal = "background.png";
};

} // namespace MiniFazenda::Apresentacao
