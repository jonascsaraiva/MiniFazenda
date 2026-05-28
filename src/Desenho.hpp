#pragma once

#include "Constantes.hpp"
#include "Tipos.hpp"

#include <SDL.h>
#include <SDL_ttf.h>

#include <algorithm>
#include <string>

namespace Desenho {

inline SDL_Color corParaEstado(EstadoDoCanteiro estado) {
    switch (estado) {
        case ESTADO_TERRA_ARADA:
            return SDL_Color{118, 77, 45, 255};
        case ESTADO_SEMENTE_PLANTADA:
            return SDL_Color{96, 67, 40, 255};
        case ESTADO_PLANTA_CRESCENDO:
            return SDL_Color{78, 144, 74, 255};
        case ESTADO_PLANTA_MADURA:
            return SDL_Color{230, 187, 65, 255};
        case ESTADO_PLANTA_MORTA:
            return SDL_Color{92, 84, 78, 255};
        case ESTADO_TERRA_VAZIA:
        default:
            return SDL_Color{147, 98, 56, 255};
    }
}

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

inline void desenharFundo(SDL_Renderer* renderizador, SDL_Texture* texturaFundo) {
    SDL_Rect tela{0, 0, Constantes::LARGURA_DA_JANELA, Constantes::ALTURA_DA_JANELA};
    if (texturaFundo != nullptr) {
        SDL_RenderCopy(renderizador, texturaFundo, nullptr, &tela);
        return;
    }

    preencherRetangulo(renderizador, tela, SDL_Color{99, 157, 86, 255});

    SDL_Rect rio{0, 520, Constantes::LARGURA_DA_JANELA, 100};
    preencherRetangulo(renderizador, rio, SDL_Color{74, 145, 186, 255});
    preencherRetangulo(renderizador, SDL_Rect{0, 512, Constantes::LARGURA_DA_JANELA, 10}, SDL_Color{124, 181, 118, 255});
    preencherRetangulo(renderizador, SDL_Rect{0, 620, Constantes::LARGURA_DA_JANELA, 10}, SDL_Color{124, 181, 118, 255});
}

inline void desenharCanteiro(
    SDL_Renderer* renderizador,
    SDL_Texture* textura,
    const DadosDoCanteiro& canteiro,
    SDL_Rect destino,
    bool destacado
) {
    if (textura != nullptr) {
        SDL_RenderCopy(renderizador, textura, nullptr, &destino);
    } else {
        SDL_Color borda = destacado ? SDL_Color{255, 244, 169, 255} : SDL_Color{79, 56, 38, 255};
        desenharLosango(renderizador, destino, corParaEstado(canteiro.estadoVisualAtual), borda);

        if (canteiro.estadoVisualAtual == ESTADO_SEMENTE_PLANTADA) {
            preencherRetangulo(
                renderizador,
                SDL_Rect{destino.x + destino.w / 2 - 3, destino.y + destino.h / 2 - 3, 6, 6},
                SDL_Color{47, 37, 23, 255}
            );
        }

        if (canteiro.estadoVisualAtual == ESTADO_PLANTA_CRESCENDO ||
            canteiro.estadoVisualAtual == ESTADO_PLANTA_MADURA) {
            const SDL_Color corPlanta = canteiro.estadoVisualAtual == ESTADO_PLANTA_MADURA
                ? SDL_Color{255, 222, 91, 255}
                : SDL_Color{44, 123, 64, 255};

            preencherRetangulo(
                renderizador,
                SDL_Rect{destino.x + destino.w / 2 - 6, destino.y + destino.h / 2 - 18, 12, 20},
                corPlanta
            );
            preencherRetangulo(
                renderizador,
                SDL_Rect{destino.x + destino.w / 2 - 18, destino.y + destino.h / 2 - 9, 36, 8},
                corPlanta
            );
        }
    }

    if (destacado) {
        desenharLosango(renderizador, destino, SDL_Color{255, 255, 255, 26}, SDL_Color{255, 244, 169, 255});
    }
}

inline void desenharCasa(SDL_Renderer* renderizador, SDL_Texture* textura, SDL_Rect destino) {
    if (textura != nullptr) {
        SDL_RenderCopy(renderizador, textura, nullptr, &destino);
        return;
    }

    preencherRetangulo(renderizador, SDL_Rect{destino.x + destino.w / 8, destino.y + destino.h / 3, destino.w * 3 / 4, destino.h * 2 / 3}, SDL_Color{221, 180, 123, 255});
    preencherRetangulo(renderizador, SDL_Rect{destino.x, destino.y + destino.h / 4, destino.w, destino.h / 4}, SDL_Color{126, 65, 55, 255});
    preencherRetangulo(renderizador, SDL_Rect{destino.x + destino.w / 2 - destino.w / 10, destino.y + destino.h * 2 / 3, destino.w / 5, destino.h / 3}, SDL_Color{100, 70, 46, 255});
}

inline void desenharCasinha(SDL_Renderer* renderizador, SDL_Texture* textura, SDL_Rect destino) {
    if (textura != nullptr) {
        SDL_RenderCopy(renderizador, textura, nullptr, &destino);
        return;
    }

    preencherRetangulo(renderizador, SDL_Rect{destino.x + destino.w / 7, destino.y + destino.h / 2, destino.w * 5 / 7, destino.h / 2}, SDL_Color{198, 141, 88, 255});
    preencherRetangulo(renderizador, SDL_Rect{destino.x, destino.y + destino.h / 3, destino.w, destino.h / 4}, SDL_Color{105, 67, 69, 255});
    preencherRetangulo(renderizador, SDL_Rect{destino.x + destino.w / 2 - destino.w / 9, destino.y + destino.h * 2 / 3, destino.w * 2 / 9, destino.h / 3}, SDL_Color{58, 43, 37, 255});
}

inline void desenharIconeCursor(SDL_Renderer* renderizador, SDL_Rect area, SDL_Color cor) {
    definirCor(renderizador, cor);
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
    definirCor(renderizador, cor);
    SDL_RenderDrawLine(renderizador, area.x + 17, area.y + 39, area.x + 36, area.y + 16);
    SDL_RenderDrawLine(renderizador, area.x + 31, area.y + 15, area.x + 42, area.y + 24);
    SDL_RenderDrawLine(renderizador, area.x + 27, area.y + 19, area.x + 38, area.y + 28);
}

inline void desenharIconeSemente(SDL_Renderer* renderizador, SDL_Rect area, SDL_Color cor) {
    preencherRetangulo(renderizador, SDL_Rect{area.x + 23, area.y + 25, 7, 7}, cor);
    preencherRetangulo(renderizador, SDL_Rect{area.x + 18, area.y + 32, 7, 7}, cor);
    preencherRetangulo(renderizador, SDL_Rect{area.x + 30, area.y + 32, 7, 7}, cor);
    definirCor(renderizador, cor);
    SDL_RenderDrawLine(renderizador, area.x + 27, area.y + 25, area.x + 36, area.y + 15);
    SDL_RenderDrawLine(renderizador, area.x + 36, area.y + 15, area.x + 42, area.y + 18);
}

inline void desenharIconePresente(SDL_Renderer* renderizador, SDL_Rect area, SDL_Color cor) {
    preencherRetangulo(renderizador, SDL_Rect{area.x + 14, area.y + 24, 28, 20}, cor);
    preencherRetangulo(renderizador, SDL_Rect{area.x + 12, area.y + 18, 32, 8}, cor);
    preencherRetangulo(renderizador, SDL_Rect{area.x + 26, area.y + 18, 4, 26}, SDL_Color{255, 242, 178, 255});
    preencherRetangulo(renderizador, SDL_Rect{area.x + 20, area.y + 13, 7, 7}, cor);
    preencherRetangulo(renderizador, SDL_Rect{area.x + 30, area.y + 13, 7, 7}, cor);
}

inline void desenharIconeEngrenagem(SDL_Renderer* renderizador, SDL_Rect area, SDL_Color cor) {
    definirCor(renderizador, cor);
    const int centroX = area.x + area.w / 2;
    const int centroY = area.y + area.h / 2;
    SDL_Rect nucleo{centroX - 8, centroY - 8, 16, 16};
    SDL_RenderDrawRect(renderizador, &nucleo);
    SDL_RenderDrawLine(renderizador, centroX, area.y + 10, centroX, area.y + 18);
    SDL_RenderDrawLine(renderizador, centroX, area.y + area.h - 10, centroX, area.y + area.h - 18);
    SDL_RenderDrawLine(renderizador, area.x + 10, centroY, area.x + 18, centroY);
    SDL_RenderDrawLine(renderizador, area.x + area.w - 10, centroY, area.x + area.w - 18, centroY);
    SDL_RenderDrawLine(renderizador, area.x + 16, area.y + 16, area.x + 21, area.y + 21);
    SDL_RenderDrawLine(renderizador, area.x + area.w - 16, area.y + 16, area.x + area.w - 21, area.y + 21);
    SDL_RenderDrawLine(renderizador, area.x + 16, area.y + area.h - 16, area.x + 21, area.y + area.h - 21);
    SDL_RenderDrawLine(renderizador, area.x + area.w - 16, area.y + area.h - 16, area.x + area.w - 21, area.y + area.h - 21);
}

inline void desenharBotaoConfiguracao(SDL_Renderer* renderizador, AreaDeInteracao areaDeInteracao) {
    SDL_Rect area{
        areaDeInteracao.posicaoBotaoHorizontal,
        areaDeInteracao.posicaoBotaoVertical,
        areaDeInteracao.tamanhoBotaoLargura,
        areaDeInteracao.tamanhoBotaoAltura
    };

    preencherRetangulo(renderizador, area, SDL_Color{238, 226, 203, 255});
    definirCor(renderizador, SDL_Color{117, 104, 87, 255});
    SDL_RenderDrawRect(renderizador, &area);
    desenharIconeEngrenagem(renderizador, area, SDL_Color{81, 76, 67, 255});
}

inline void desenharBotaoFerramenta(
    SDL_Renderer* renderizador,
    AreaDeInteracao areaDeInteracao,
    FerramentaSelecionada ferramentaDoBotao,
    FerramentaSelecionada ferramentaSelecionada
) {
    SDL_Rect area{
        areaDeInteracao.posicaoBotaoHorizontal,
        areaDeInteracao.posicaoBotaoVertical,
        areaDeInteracao.tamanhoBotaoLargura,
        areaDeInteracao.tamanhoBotaoAltura
    };

    const bool selecionado = ferramentaDoBotao == ferramentaSelecionada;
    preencherRetangulo(renderizador, area, selecionado ? SDL_Color{248, 226, 148, 255} : SDL_Color{238, 226, 203, 255});
    definirCor(renderizador, selecionado ? SDL_Color{94, 75, 45, 255} : SDL_Color{117, 104, 87, 255});
    SDL_RenderDrawRect(renderizador, &area);

    const SDL_Color corIcone = selecionado ? SDL_Color{65, 49, 31, 255} : SDL_Color{81, 76, 67, 255};
    switch (ferramentaDoBotao) {
        case FERRAMENTA_CURSOR:
            desenharIconeCursor(renderizador, area, corIcone);
            break;
        case FERRAMENTA_ENXADA:
            desenharIconeEnxada(renderizador, area, corIcone);
            break;
        case FERRAMENTA_SEMENTE:
            desenharIconeSemente(renderizador, area, corIcone);
            break;
        case FERRAMENTA_PRESENTE:
            desenharIconePresente(renderizador, area, corIcone);
            break;
    }
}

inline void desenharInterface(
    SDL_Renderer* renderizador,
    TTF_Font* fonte,
    int moedas,
    int experiencia,
    int nivel,
    FerramentaSelecionada ferramentaSelecionada,
    AreaDeInteracao botaoConfiguracao,
    AreaDeInteracao botaoCursor,
    AreaDeInteracao botaoEnxada,
    AreaDeInteracao botaoSemente,
    AreaDeInteracao botaoPresente
) {
    preencherRetangulo(renderizador, SDL_Rect{0, 0, Constantes::LARGURA_DA_JANELA, 72}, SDL_Color{58, 72, 68, 235});
    preencherRetangulo(renderizador, SDL_Rect{0, Constantes::ALTURA_DA_JANELA - 84, Constantes::LARGURA_DA_JANELA, 84}, SDL_Color{61, 65, 57, 235});

    desenharTexto(renderizador, fonte, "Moedas: " + std::to_string(moedas), 24, 18, SDL_Color{255, 245, 204, 255});
    desenharTexto(renderizador, fonte, "XP: " + std::to_string(experiencia), 198, 18, SDL_Color{232, 245, 255, 255});
    desenharTexto(renderizador, fonte, "Nivel: " + std::to_string(nivel), 314, 18, SDL_Color{230, 255, 224, 255});

    desenharBotaoConfiguracao(renderizador, botaoConfiguracao);
    desenharBotaoFerramenta(renderizador, botaoCursor, FERRAMENTA_CURSOR, ferramentaSelecionada);
    desenharBotaoFerramenta(renderizador, botaoEnxada, FERRAMENTA_ENXADA, ferramentaSelecionada);
    desenharBotaoFerramenta(renderizador, botaoSemente, FERRAMENTA_SEMENTE, ferramentaSelecionada);
    desenharBotaoFerramenta(renderizador, botaoPresente, FERRAMENTA_PRESENTE, ferramentaSelecionada);
}

inline void desenharCursorCustomizado(SDL_Renderer* renderizador, int mouseX, int mouseY, FerramentaSelecionada ferramenta) {
    SDL_Rect area{mouseX, mouseY, 38, 38};
    const SDL_Color cor = SDL_Color{255, 252, 218, 255};

    switch (ferramenta) {
        case FERRAMENTA_ENXADA:
            desenharIconeEnxada(renderizador, area, cor);
            break;
        case FERRAMENTA_SEMENTE:
            desenharIconeSemente(renderizador, area, cor);
            break;
        case FERRAMENTA_PRESENTE:
            desenharIconePresente(renderizador, area, cor);
            break;
        case FERRAMENTA_CURSOR:
        default:
            desenharIconeCursor(renderizador, area, cor);
            break;
    }
}

} // namespace Desenho
