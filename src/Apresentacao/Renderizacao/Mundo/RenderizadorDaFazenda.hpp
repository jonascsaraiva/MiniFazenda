#pragma once

#include "Aplicacao/Estado/EstadoDoJogo.hpp"
#include "Apresentacao/Camera/CameraDoJogo.hpp"
#include "Apresentacao/ConfiguracoesDoLayout.hpp"
#include "Apresentacao/Isometria/Isometrico.hpp"
#include "Apresentacao/Renderizacao/Mundo/DesenhoDoMundo.hpp"
#include "Apresentacao/Renderizacao/Primitivas/PrimitivasSDL.hpp"
#include "Compartilhado/ConstantesDaIsometria.hpp"
#include "Compartilhado/ConstantesDaJanela.hpp"
#include "Compartilhado/Geometria/Posicoes.hpp"
#include "Dominio/Ferramentas/TipoDeFerramenta.hpp"
#include "Dominio/Mapa/MapaDaFazenda.hpp"
#include "Infraestrutura/Assets/RecursosDaFazenda.hpp"

#include <SDL.h>

#include <algorithm>
#include <cmath>

namespace MiniFazenda::Apresentacao::Renderizacao::Mundo {

inline Compartilhado::Geometria::PosicaoNaGrade converterMouseParaGradeGlobal(
    int mouseX,
    int mouseY,
    const ConfiguracoesDoLayout& configuracoes,
    const Camera::EstadoDaCamera& camera
) {
    const Camera::DimensoesDoCanteiroRenderizado dimensoes =
        Camera::calcularDimensoesDoCanteiroRenderizado(camera.zoomAtual);

    return Isometria::converterTelaParaGradeGlobal(
        mouseX,
        mouseY,
        dimensoes.largura,
        dimensoes.altura,
        configuracoes.origemGradeHorizontal,
        configuracoes.origemGradeVertical,
        camera.offsetHorizontal,
        camera.offsetVertical
    );
}

inline Compartilhado::Geometria::PosicaoDeCanteiroNoMapa converterMouseParaCanteiroNoMapa(
    int mouseX,
    int mouseY,
    const ConfiguracoesDoLayout& configuracoes,
    const Camera::EstadoDaCamera& camera
) {
    return Compartilhado::Geometria::converterPosicaoNaGradeParaCanteiroNoMapa(
        converterMouseParaGradeGlobal(mouseX, mouseY, configuracoes, camera)
    );
}

inline SDL_Rect calcularDestinoDoCanteiro(
    Compartilhado::Geometria::PosicaoNaGrade posicao,
    const ConfiguracoesDoLayout& configuracoes,
    const Camera::EstadoDaCamera& camera
) {
    const Camera::DimensoesDoCanteiroRenderizado dimensoes =
        Camera::calcularDimensoesDoCanteiroRenderizado(camera.zoomAtual);
    const Compartilhado::Geometria::PosicaoNaTela posicaoNaTela = Isometria::converterGradeGlobalParaTela(
        posicao,
        dimensoes.largura,
        dimensoes.altura,
        configuracoes.origemGradeHorizontal,
        configuracoes.origemGradeVertical,
        camera.offsetHorizontal,
        camera.offsetVertical
    );

    return SDL_Rect{
        posicaoNaTela.coordenadaHorizontal,
        posicaoNaTela.coordenadaVertical,
        dimensoes.largura,
        dimensoes.altura
    };
}

inline SDL_Rect calcularDestinoDoCanteiro(
    Compartilhado::Geometria::PosicaoDeCanteiroNoMapa posicao,
    const ConfiguracoesDoLayout& configuracoes,
    const Camera::EstadoDaCamera& camera
) {
    return calcularDestinoDoCanteiro(
        Compartilhado::Geometria::converterCanteiroNoMapaParaPosicaoNaGrade(posicao),
        configuracoes,
        camera
    );
}

inline bool retanguloApareceNaTela(SDL_Rect retangulo) {
    return retangulo.x < Compartilhado::Constantes::LARGURA_DA_JANELA &&
           retangulo.x + retangulo.w > 0 &&
           retangulo.y < Compartilhado::Constantes::ALTURA_DA_JANELA &&
           retangulo.y + retangulo.h > 0;
}

inline SDL_Rect calcularHitboxDoCanteiro(SDL_Rect destinoDoCanteiro) {
    return SDL_Rect{
        destinoDoCanteiro.x,
        destinoDoCanteiro.y,
        (destinoDoCanteiro.w / 2) * 2,
        (destinoDoCanteiro.h / 2) * 2
    };
}

inline void desenharDebugDaHitboxDoCanteiro(SDL_Renderer* renderizador, SDL_Rect destinoDoCanteiro) {
    const SDL_Rect hitbox = calcularHitboxDoCanteiro(destinoDoCanteiro);
    const int centroX = hitbox.x + hitbox.w / 2;
    const int centroY = hitbox.y + hitbox.h / 2;

    Primitivas::desenharContornoLosango(renderizador, hitbox, SDL_Color{255, 0, 0, 150});
    Primitivas::preencherRetangulo(renderizador, SDL_Rect{centroX - 2, centroY - 2, 4, 4}, SDL_Color{255, 225, 0, 220});
}

inline SDL_Rect calcularDestinoDoSpriteDaPlanta(
    SDL_Rect destinoDoCanteiro,
    const Infraestrutura::Assets::SpriteDaPlanta& sprite
) {
    if (!sprite.possuiAncora || sprite.largura <= 0 || sprite.altura <= 0) {
        return destinoDoCanteiro;
    }

    const double escala = static_cast<double>(destinoDoCanteiro.w) / static_cast<double>(sprite.largura);
    const int largura = std::max(1, static_cast<int>(std::lround(sprite.largura * escala)));
    const int altura = std::max(1, static_cast<int>(std::lround(sprite.altura * escala)));
    const int ancoraX = static_cast<int>(std::lround(sprite.ancoraDaBase.x * escala));
    const int ancoraY = static_cast<int>(std::lround(sprite.ancoraDaBase.y * escala));
    const int pontoDePlantioX = destinoDoCanteiro.x + destinoDoCanteiro.w / 2;
    const int pontoDePlantioY = destinoDoCanteiro.y + destinoDoCanteiro.h / 2;

    return SDL_Rect{
        pontoDePlantioX - ancoraX,
        pontoDePlantioY - ancoraY,
        largura,
        altura
    };
}

inline void desenharGradeAtiva(
    SDL_Renderer* renderizador,
    const Aplicacao::Estado::EstadoDoJogo& jogo,
    const Infraestrutura::Assets::TexturasDosCanteiros& texturasCanteiro,
    const ConfiguracoesDoLayout& configuracoes,
    const Camera::EstadoDaCamera& camera,
    Compartilhado::Geometria::PosicaoDeCanteiroNoMapa posicaoRealcada
) {
    for (const Dominio::Mapa::EntidadeDoMapa& entidade : jogo.mapa().entidades()) {
        if (!entidade.ehCanteiroAgricola() || !entidade.posicaoDoCanteiroNoMapa().has_value()) {
            continue;
        }

        const Compartilhado::Geometria::PosicaoDeCanteiroNoMapa posicaoDoCanteiro =
            *entidade.posicaoDoCanteiroNoMapa();
        if (!Dominio::Mapa::MapaDaFazenda::posicaoEstaDentroDaAreaJogavel(
                posicaoDoCanteiro,
                jogo.tamanhoAtualDoGrid()
            )) {
            continue;
        }

        const Dominio::Canteiros::Canteiro* canteiro = entidade.canteiroAgricola();
        if (canteiro == nullptr) {
            continue;
        }

        const SDL_Rect destino = calcularDestinoDoCanteiro(posicaoDoCanteiro, configuracoes, camera);
        if (!retanguloApareceNaTela(destino)) {
            continue;
        }

        const Dominio::Canteiros::EstadoVisualDoCanteiro estadoVisual = canteiro->estadoVisualAtual();
        SDL_Texture* texturaDaTerra = nullptr;
        SDL_Texture* texturaDaPlanta = nullptr;
        SDL_Rect destinoDaPlanta = destino;
        if (Infraestrutura::Assets::estadoVisualTemPlantaParaDesenho(estadoVisual)) {
            texturaDaTerra = texturasCanteiro.texturaDeTerraParaEstado(
                Dominio::Canteiros::EstadoVisualDoCanteiro::TerraArada
            );
            const Infraestrutura::Assets::SpriteDaPlanta* spriteDaPlanta = texturasCanteiro.spriteDePlantaParaEstado(
                canteiro->identificadorDaSemente(),
                estadoVisual
            );
            if (spriteDaPlanta != nullptr && spriteDaPlanta->textura != nullptr) {
                texturaDaPlanta = spriteDaPlanta->textura;
                destinoDaPlanta = calcularDestinoDoSpriteDaPlanta(destino, *spriteDaPlanta);
            }
        } else {
            texturaDaTerra = texturasCanteiro.texturaDeTerraParaEstado(estadoVisual);
        }

        desenharCanteiro(
            renderizador,
            texturaDaTerra,
            texturaDaPlanta,
            destinoDaPlanta,
            *canteiro,
            destino,
            Compartilhado::Geometria::posicoesDeCanteiroNoMapaSaoIguais(posicaoRealcada, posicaoDoCanteiro)
        );

        if constexpr (Compartilhado::Constantes::DEBUG_HITBOX_TILES) {
            desenharDebugDaHitboxDoCanteiro(renderizador, destino);
        }
    }
}

inline void desenharPreviewDeCriacaoDeTerra(
    SDL_Renderer* renderizador,
    const Aplicacao::Estado::EstadoDoJogo& jogo,
    const ConfiguracoesDoLayout& configuracoes,
    const Camera::EstadoDaCamera& camera,
    Compartilhado::Geometria::PosicaoDeCanteiroNoMapa posicaoRealcada
) {
    if (jogo.ferramentaSelecionada() != Dominio::Ferramentas::TipoDeFerramenta::Enxada ||
        !Dominio::Mapa::MapaDaFazenda::posicaoEstaDentroDoMapaGlobal(posicaoRealcada) ||
        !Dominio::Mapa::MapaDaFazenda::posicaoEstaDentroDaAreaJogavel(posicaoRealcada, jogo.tamanhoAtualDoGrid()) ||
        jogo.mapa().existeCanteiroEm(posicaoRealcada)) {
        return;
    }

    const SDL_Rect destino = calcularDestinoDoCanteiro(posicaoRealcada, configuracoes, camera);
    if (!retanguloApareceNaTela(destino)) {
        return;
    }

    Primitivas::desenharContornoLosango(renderizador, destino, SDL_Color{255, 244, 169, 160});
}

inline void desenharLimiteDaGradeJogavel(
    SDL_Renderer* renderizador,
    const Aplicacao::Estado::EstadoDoJogo& jogo,
    const ConfiguracoesDoLayout& configuracoes,
    const Camera::EstadoDaCamera& camera
) {
    const Camera::RetanguloDeGradeRenderizada retangulo =
        Camera::calcularRetanguloDaGradeRenderizada(configuracoes, camera, jogo.tamanhoAtualDoGrid());

    SDL_Rect destino{
        retangulo.x,
        retangulo.y,
        retangulo.largura,
        retangulo.altura
    };

    if (retanguloApareceNaTela(destino)) {
        Primitivas::desenharContornoLosango(renderizador, destino, SDL_Color{255, 244, 169, 160});
    }
}

} // namespace MiniFazenda::Apresentacao::Renderizacao::Mundo
