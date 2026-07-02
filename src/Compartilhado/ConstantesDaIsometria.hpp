#pragma once

#include "Compartilhado/ConstantesDoJogo.hpp"

namespace MiniFazenda::Compartilhado::Constantes {

constexpr int CENTRO_VISUAL_BACKGROUND_X = 576;
constexpr int CENTRO_VISUAL_BACKGROUND_Y = 296;

constexpr int COLUNA_DE_ORIGEM_VISUAL_DA_GRADE_GLOBAL = COLUNA_INICIAL_DO_NUCLEO_INICIAL;
constexpr int LINHA_DE_ORIGEM_VISUAL_DA_GRADE_GLOBAL = LINHA_INICIAL_DO_NUCLEO_INICIAL;

constexpr int LARGURA_DO_CANTEIRO = 128;
constexpr int ALTURA_DO_CANTEIRO = 64;

constexpr bool DEBUG_HITBOX_TILES = false;

} // namespace MiniFazenda::Compartilhado::Constantes
