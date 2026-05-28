#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include <filesystem>
#include <string>
#include <unordered_map>

class GerenciadorDeAtivos {
public:
    explicit GerenciadorDeAtivos(SDL_Renderer* renderizadorDaAplicacao)
        : renderizador(renderizadorDaAplicacao) {
    }

    GerenciadorDeAtivos(const GerenciadorDeAtivos&) = delete;
    GerenciadorDeAtivos& operator=(const GerenciadorDeAtivos&) = delete;

    ~GerenciadorDeAtivos() {
        liberarTudo();
    }

    SDL_Texture* carregarTextura(const std::filesystem::path& caminho) {
        const std::string chave = caminho.string();
        const auto encontrada = texturas.find(chave);
        if (encontrada != texturas.end()) {
            return encontrada->second;
        }

        SDL_Texture* textura = IMG_LoadTexture(renderizador, chave.c_str());
        if (textura == nullptr) {
            texturas[chave] = nullptr;
            return nullptr;
        }

        texturas[chave] = textura;
        return textura;
    }

    TTF_Font* carregarFonte(const std::filesystem::path& caminho, int tamanho) {
        const std::string chave = caminho.string() + "#" + std::to_string(tamanho);
        const auto encontrada = fontes.find(chave);
        if (encontrada != fontes.end()) {
            return encontrada->second;
        }

        TTF_Font* fonte = TTF_OpenFont(caminho.string().c_str(), tamanho);
        if (fonte == nullptr) {
            fontes[chave] = nullptr;
            return nullptr;
        }

        fontes[chave] = fonte;
        return fonte;
    }

    Mix_Chunk* carregarSom(const std::filesystem::path& caminho) {
        const std::string chave = caminho.string();
        const auto encontrado = sons.find(chave);
        if (encontrado != sons.end()) {
            return encontrado->second;
        }

        Mix_Chunk* som = Mix_LoadWAV(chave.c_str());
        if (som == nullptr) {
            sons[chave] = nullptr;
            return nullptr;
        }

        sons[chave] = som;
        return som;
    }

    Mix_Music* carregarMusica(const std::filesystem::path& caminho) {
        const std::string chave = caminho.string();
        const auto encontrada = musicas.find(chave);
        if (encontrada != musicas.end()) {
            return encontrada->second;
        }

        Mix_Music* musica = Mix_LoadMUS(chave.c_str());
        if (musica == nullptr) {
            musicas[chave] = nullptr;
            return nullptr;
        }

        musicas[chave] = musica;
        return musica;
    }

    void tocarSom(const std::filesystem::path& caminho) {
        Mix_Chunk* som = carregarSom(caminho);
        if (som != nullptr) {
            Mix_PlayChannel(-1, som, 0);
        }
    }

    void tocarMusica(const std::filesystem::path& caminho) {
        Mix_Music* musica = carregarMusica(caminho);
        if (musica != nullptr && Mix_PlayingMusic() == 0) {
            Mix_PlayMusic(musica, -1);
        }
    }

private:
    void liberarTudo() {
        for (auto& par : texturas) {
            if (par.second != nullptr) {
                SDL_DestroyTexture(par.second);
            }
        }

        for (auto& par : fontes) {
            if (par.second != nullptr) {
                TTF_CloseFont(par.second);
            }
        }

        for (auto& par : sons) {
            if (par.second != nullptr) {
                Mix_FreeChunk(par.second);
            }
        }

        for (auto& par : musicas) {
            if (par.second != nullptr) {
                Mix_FreeMusic(par.second);
            }
        }

        texturas.clear();
        fontes.clear();
        sons.clear();
        musicas.clear();
    }

    SDL_Renderer* renderizador = nullptr;
    std::unordered_map<std::string, SDL_Texture*> texturas;
    std::unordered_map<std::string, TTF_Font*> fontes;
    std::unordered_map<std::string, Mix_Chunk*> sons;
    std::unordered_map<std::string, Mix_Music*> musicas;
};

