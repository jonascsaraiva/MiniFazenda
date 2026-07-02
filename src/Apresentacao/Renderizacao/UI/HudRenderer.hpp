#pragma once

#include "Apresentacao/Interface/EstadoDaCenaFazenda.hpp"
#include "Apresentacao/Renderizacao/Primitivas/PrimitivasSDL.hpp"
#include "Compartilhado/ConstantesDaJanela.hpp"
#include "Dominio/Jogador/Jogador.hpp"

#include <SDL.h>
#include <SDL_ttf.h>

#include <string>

namespace MiniFazenda::Apresentacao::Renderizacao::UI::HudRenderer {

inline SDL_Rect calcularAreaDoBotaoConfiguracoes() {
    constexpr int margem = 12;
    constexpr int tamanho = 36;
    return SDL_Rect{
        Compartilhado::Constantes::LARGURA_DA_JANELA - margem - tamanho,
        margem,
        tamanho,
        tamanho
    };
}

inline SDL_Rect calcularAreaDoPainelConfiguracoes() {
    constexpr int larguraDoPainel = 480;
    constexpr int alturaDoPainel = 320;
    return SDL_Rect{
        (Compartilhado::Constantes::LARGURA_DA_JANELA - larguraDoPainel) / 2,
        (Compartilhado::Constantes::ALTURA_DA_JANELA - alturaDoPainel) / 2,
        larguraDoPainel,
        alturaDoPainel
    };
}

inline SDL_Rect calcularAreaDoBotaoSomConfiguracoes() {
    constexpr int larguraDoBotao = 160;
    constexpr int alturaDoBotao = 40;
    const SDL_Rect painel = calcularAreaDoPainelConfiguracoes();

    return SDL_Rect{
        painel.x + (painel.w - larguraDoBotao) / 2,
        painel.y + 150,
        larguraDoBotao,
        alturaDoBotao
    };
}

inline void desenharStatusDoJogador(
    SDL_Renderer* renderizador,
    TTF_Font* fonte,
    const Dominio::Jogador::Jogador& jogador
) {
    if (fonte == nullptr) {
        return;
    }

    constexpr int margem = 12;
    constexpr int espacamentoVertical = 22;
    const SDL_Color corDoTexto{255, 245, 180, 255};
    const int experienciaParaProximoNivel = jogador.nivel() * 100;

    Primitivas::desenharTexto(
        renderizador,
        fonte,
        "Moedas: " + std::to_string(jogador.moedas()),
        margem,
        margem,
        corDoTexto
    );
    Primitivas::desenharTexto(
        renderizador,
        fonte,
        "XP: " + std::to_string(jogador.experiencia()) + " / " + std::to_string(experienciaParaProximoNivel),
        margem,
        margem + espacamentoVertical,
        corDoTexto
    );
    Primitivas::desenharTexto(
        renderizador,
        fonte,
        "Nivel: " + std::to_string(jogador.nivel()),
        margem,
        margem + espacamentoVertical * 2,
        corDoTexto
    );
}

inline void desenharBotaoConfiguracoes(SDL_Renderer* renderizador, SDL_Texture* textura, bool destacado) {
    SDL_Rect area = calcularAreaDoBotaoConfiguracoes();

    if (textura != nullptr) {
        SDL_RenderCopy(renderizador, textura, nullptr, &area);
    } else {
        Primitivas::preencherRetangulo(renderizador, area, SDL_Color{61, 65, 57, 235});
        Primitivas::definirCor(renderizador, SDL_Color{255, 245, 180, 255});
        SDL_RenderDrawRect(renderizador, &area);

        constexpr int recuo = 10;
        SDL_RenderDrawLine(renderizador, area.x + area.w - recuo, area.y + recuo, area.x + recuo, area.y + recuo);
        SDL_RenderDrawLine(renderizador, area.x + recuo, area.y + recuo, area.x + recuo, area.y + area.h - recuo);
        SDL_RenderDrawLine(
            renderizador,
            area.x + recuo,
            area.y + area.h - recuo,
            area.x + area.w - recuo,
            area.y + area.h - recuo
        );
    }

    if (destacado) {
        Primitivas::definirCor(renderizador, SDL_Color{255, 255, 255, 255});
        SDL_RenderDrawRect(renderizador, &area);
    }
}

inline SDL_Rect desenharPainelConfiguracoes(
    SDL_Renderer* renderizador,
    TTF_Font* fonte,
    const MiniFazenda::Apresentacao::Interface::EstadoDaCenaFazenda& estadoDaCena
) {
    SDL_Rect painel = calcularAreaDoPainelConfiguracoes();
    SDL_Rect botaoSom = calcularAreaDoBotaoSomConfiguracoes();

    Primitivas::preencherRetangulo(renderizador, painel, SDL_Color{50, 50, 50, 220});
    Primitivas::definirCor(renderizador, SDL_Color{180, 180, 180, 255});
    SDL_RenderDrawRect(renderizador, &painel);

    const bool somAtivo = !estadoDaCena.audioMutado();
    Primitivas::preencherRetangulo(
        renderizador,
        botaoSom,
        somAtivo ? SDL_Color{80, 120, 80, 255} : SDL_Color{120, 80, 80, 255}
    );
    Primitivas::definirCor(renderizador, SDL_Color{210, 210, 210, 255});
    SDL_RenderDrawRect(renderizador, &botaoSom);

    if (fonte != nullptr) {
        const auto desenharTextoCentralizado = [renderizador, fonte](
            const std::string& texto,
            SDL_Rect area,
            SDL_Color cor
        ) {
            int larguraDoTexto = 0;
            int alturaDoTexto = 0;
            if (TTF_SizeUTF8(fonte, texto.c_str(), &larguraDoTexto, &alturaDoTexto) != 0) {
                Primitivas::desenharTexto(renderizador, fonte, texto, area.x, area.y, cor);
                return;
            }

            Primitivas::desenharTexto(
                renderizador,
                fonte,
                texto,
                area.x + (area.w - larguraDoTexto) / 2,
                area.y + (area.h - alturaDoTexto) / 2,
                cor
            );
        };

        SDL_Rect areaDoTitulo{painel.x, painel.y + 28, painel.w, 40};
        const SDL_Color corDoTitulo{255, 255, 255, 255};
        const bool fontePossuiAcentos =
            TTF_GlyphIsProvided(fonte, 0x00E7) != 0 && TTF_GlyphIsProvided(fonte, 0x00F5) != 0;
        if (fontePossuiAcentos) {
            desenharTextoCentralizado("Configura\xC3\xA7\xC3\xB5" "es", areaDoTitulo, corDoTitulo);
        } else {
            const std::string tituloSemAcentos = "Configuracoes";
            int larguraDoTitulo = 0;
            int alturaDoTitulo = 0;
            if (TTF_SizeUTF8(fonte, tituloSemAcentos.c_str(), &larguraDoTitulo, &alturaDoTitulo) == 0) {
                const int tituloX = areaDoTitulo.x + (areaDoTitulo.w - larguraDoTitulo) / 2;
                const int tituloY = areaDoTitulo.y + (areaDoTitulo.h - alturaDoTitulo) / 2;
                Primitivas::desenharTexto(renderizador, fonte, tituloSemAcentos, tituloX, tituloY, corDoTitulo);

                const auto larguraDoTexto = [fonte](const std::string& texto) {
                    int largura = 0;
                    int altura = 0;
                    TTF_SizeUTF8(fonte, texto.c_str(), &largura, &altura);
                    return largura;
                };

                const int centroDaCedilha =
                    tituloX + larguraDoTexto("Configura") + larguraDoTexto("c") / 2;
                const int centroDoTil =
                    tituloX + larguraDoTexto("Configurac") + larguraDoTexto("o") / 2;

                Primitivas::definirCor(renderizador, corDoTitulo);
                SDL_RenderDrawLine(
                    renderizador,
                    centroDaCedilha,
                    tituloY + alturaDoTitulo - 2,
                    centroDaCedilha - 2,
                    tituloY + alturaDoTitulo + 2
                );
                SDL_RenderDrawLine(
                    renderizador,
                    centroDaCedilha - 2,
                    tituloY + alturaDoTitulo + 2,
                    centroDaCedilha + 1,
                    tituloY + alturaDoTitulo + 4
                );
                SDL_RenderDrawLine(renderizador, centroDoTil - 5, tituloY + 2, centroDoTil - 2, tituloY);
                SDL_RenderDrawLine(renderizador, centroDoTil - 2, tituloY, centroDoTil + 1, tituloY + 2);
                SDL_RenderDrawLine(renderizador, centroDoTil + 1, tituloY + 2, centroDoTil + 4, tituloY);
            } else {
                desenharTextoCentralizado(tituloSemAcentos, areaDoTitulo, corDoTitulo);
            }
        }
        desenharTextoCentralizado(
            somAtivo ? "Som: ON" : "Som: OFF",
            botaoSom,
            SDL_Color{255, 255, 255, 255}
        );
    }

    return botaoSom;
}

} // namespace MiniFazenda::Apresentacao::Renderizacao::UI::HudRenderer
