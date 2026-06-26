#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include <iostream>

namespace MiniFazenda::Infraestrutura::SDL {

struct InicializacaoSDL {
    bool sdlInicializado = false;
    bool imagemInicializada = false;
    bool fontesInicializadas = false;
    bool audioInicializado = false;

    bool inicializar() {
        SDL_SetMainReady();

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
            std::cerr << "Falha ao inicializar SDL2: " << SDL_GetError() << '\n';
            return false;
        }
        sdlInicializado = true;

        const int formatosImagem = IMG_INIT_PNG;
        if ((IMG_Init(formatosImagem) & formatosImagem) != formatosImagem) {
            std::cerr << "Falha ao inicializar SDL2_image: " << IMG_GetError() << '\n';
            return false;
        }
        imagemInicializada = true;

        if (TTF_Init() != 0) {
            std::cerr << "Falha ao inicializar SDL2_ttf: " << TTF_GetError() << '\n';
            return false;
        }
        fontesInicializadas = true;

        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == 0) {
            audioInicializado = true;
        } else {
            std::cerr << "Audio indisponivel: " << Mix_GetError() << '\n';
        }

        return true;
    }

    ~InicializacaoSDL() {
        if (audioInicializado) {
            Mix_CloseAudio();
        }

        if (fontesInicializadas) {
            TTF_Quit();
        }

        if (imagemInicializada) {
            IMG_Quit();
        }

        if (sdlInicializado) {
            SDL_Quit();
        }
    }
};

struct JanelaSDL {
    SDL_Window* ponteiro = nullptr;

    explicit JanelaSDL(SDL_Window* janela)
        : ponteiro(janela) {
    }

    JanelaSDL(const JanelaSDL&) = delete;
    JanelaSDL& operator=(const JanelaSDL&) = delete;

    ~JanelaSDL() {
        if (ponteiro != nullptr) {
            SDL_DestroyWindow(ponteiro);
        }
    }
};

struct RenderizadorSDL {
    SDL_Renderer* ponteiro = nullptr;

    explicit RenderizadorSDL(SDL_Renderer* renderizador)
        : ponteiro(renderizador) {
    }

    RenderizadorSDL(const RenderizadorSDL&) = delete;
    RenderizadorSDL& operator=(const RenderizadorSDL&) = delete;

    ~RenderizadorSDL() {
        if (ponteiro != nullptr) {
            SDL_DestroyRenderer(ponteiro);
        }
    }
};

struct CursorOculto {
    CursorOculto() {
        SDL_ShowCursor(SDL_DISABLE);
    }

    ~CursorOculto() {
        SDL_ShowCursor(SDL_ENABLE);
    }
};

inline SDL_Renderer* criarRenderizador(SDL_Window* janela) {
    SDL_Renderer* renderizador = SDL_CreateRenderer(
        janela,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (renderizador == nullptr) {
        renderizador = SDL_CreateRenderer(janela, -1, SDL_RENDERER_SOFTWARE);
    }

    return renderizador;
}

} // namespace MiniFazenda::Infraestrutura::SDL
