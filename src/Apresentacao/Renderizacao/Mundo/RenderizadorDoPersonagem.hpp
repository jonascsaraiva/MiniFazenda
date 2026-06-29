#pragma once

#include "Apresentacao/Camera/CameraDoJogo.hpp"
#include "Apresentacao/ConfiguracoesDoLayout.hpp"
#include "Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp"
#include "Compartilhado/Animacao/ConfigAnimacao.hpp"
#include "Compartilhado/Geometria/Posicoes.hpp"
#include "Dominio/Personagem/Personagem.hpp"

#include <SDL.h>

#include <algorithm>
#include <cmath>

namespace MiniFazenda::Apresentacao::Renderizacao::Mundo {

inline SDL_Rect converterRetanguloLogicoParaSDL(Compartilhado::Geometria::Retangulo retangulo) {
    return SDL_Rect{retangulo.x, retangulo.y, retangulo.w, retangulo.h};
}

inline SDL_Rect calcularDestinoDoPersonagem(SDL_Rect destinoDoCanteiro) {
    const int tamanhoRenderizado = std::max(
        Compartilhado::ConfigAnimacao::TAMANHO_MINIMO_RENDER,
        static_cast<int>(std::lround(destinoDoCanteiro.w * Compartilhado::ConfigAnimacao::FATOR_ESCALA_RENDER_IDLE))
    );
    const int pontoDeAncoragemX = destinoDoCanteiro.x + destinoDoCanteiro.w / 2;
    const int pontoDeAncoragemY = destinoDoCanteiro.y + destinoDoCanteiro.h / 2;

    return SDL_Rect{
        pontoDeAncoragemX - tamanhoRenderizado / 2,
        pontoDeAncoragemY - tamanhoRenderizado,
        tamanhoRenderizado,
        tamanhoRenderizado
    };
}

inline void desenharPersonagem(
    SDL_Renderer* renderizador,
    SDL_Texture* texturaDoPersonagem,
    const Dominio::Personagem::Personagem& personagem,
    const ConfiguracoesDoLayout& configuracoes,
    const Camera::EstadoDaCamera& camera
) {
    if (texturaDoPersonagem == nullptr) {
        return;
    }

    const SDL_Rect destinoDoCanteiro = calcularDestinoDoCanteiro(
        personagem.posicaoNaGrade(),
        configuracoes,
        camera
    );
    const SDL_Rect destinoDoPersonagem = calcularDestinoDoPersonagem(destinoDoCanteiro);

    if (!retanguloApareceNaTela(destinoDoPersonagem)) {
        return;
    }

    const SDL_Rect origem = converterRetanguloLogicoParaSDL(personagem.retanguloDeOrigemIdle());
    SDL_RenderCopy(renderizador, texturaDoPersonagem, &origem, &destinoDoPersonagem);
}

} // namespace MiniFazenda::Apresentacao::Renderizacao::Mundo
