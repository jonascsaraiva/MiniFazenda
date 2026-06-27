#pragma once

#include "Aplicacao/Estado/EstadoDoJogo.hpp"
#include "Apresentacao/Camera/CameraDoJogo.hpp"
#include "Apresentacao/ConfiguracoesDoLayout.hpp"
#include "Apresentacao/Isometria/Isometrico.hpp"
#include "Apresentacao/Renderizacao/Mundo/DesenhoDoMundo.hpp"
#include "Apresentacao/Renderizacao/Primitivas/PrimitivasSDL.hpp"
#include "Compartilhado/Constantes.hpp"
#include "Compartilhado/Geometria/Posicoes.hpp"
#include "Dominio/Ferramentas/TipoDeFerramenta.hpp"
#include "Dominio/Grade/GradeGlobalDeCanteiros.hpp"
#include "Infraestrutura/Assets/RecursosDaFazenda.hpp"

#include <SDL.h>

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

inline bool retanguloApareceNaTela(SDL_Rect retangulo) {
    return retangulo.x < Compartilhado::Constantes::LARGURA_DA_JANELA &&
           retangulo.x + retangulo.w > 0 &&
           retangulo.y < Compartilhado::Constantes::ALTURA_DA_JANELA &&
           retangulo.y + retangulo.h > 0;
}

inline void desenharGradeAtiva(
    SDL_Renderer* renderizador,
    const Aplicacao::Estado::EstadoDoJogo& jogo,
    const Infraestrutura::Assets::TexturasDosCanteiros& texturasCanteiro,
    const ConfiguracoesDoLayout& configuracoes,
    const Camera::EstadoDaCamera& camera,
    Compartilhado::Geometria::PosicaoNaGrade posicaoRealcada
) {
    const Dominio::Grade::GradeGlobalDeCanteiros& grade = jogo.grade();

    for (const Compartilhado::Geometria::PosicaoNaGrade& posicaoDoTile : grade.posicoesDeTilesExistentes()) {
        if (!Dominio::Grade::GradeGlobalDeCanteiros::posicaoEstaDentroDaGradeAtual(posicaoDoTile, jogo.tamanhoAtualDoGrid())) {
            continue;
        }

        const Dominio::Grade::TileDeTerra* tile = grade.obterTile(posicaoDoTile);
        if (tile == nullptr || !tile->existeNoMapa()) {
            continue;
        }

        const SDL_Rect destino = calcularDestinoDoCanteiro(posicaoDoTile, configuracoes, camera);
        if (!retanguloApareceNaTela(destino)) {
            continue;
        }

        const Dominio::Canteiros::EstadoVisualDoCanteiro estadoVisual = tile->canteiro().estadoVisualAtual();
        SDL_Texture* texturaCanteiro = nullptr;
        if (Infraestrutura::Assets::estadoEhFaseVisualDaPlanta(estadoVisual)) {
            texturaCanteiro = texturasCanteiro.texturaDePlantaParaEstado(
                tile->canteiro().identificadorDaSemente(),
                estadoVisual
            );
        } else {
            texturaCanteiro = texturasCanteiro.texturaDeTerraParaEstado(estadoVisual);
        }

        desenharCanteiro(
            renderizador,
            texturaCanteiro,
            tile->canteiro(),
            destino,
            Compartilhado::Geometria::posicoesDaGradeSaoIguais(posicaoRealcada, posicaoDoTile)
        );
    }
}

inline void desenharPreviewDeCriacaoDeTerra(
    SDL_Renderer* renderizador,
    const Aplicacao::Estado::EstadoDoJogo& jogo,
    const ConfiguracoesDoLayout& configuracoes,
    const Camera::EstadoDaCamera& camera,
    Compartilhado::Geometria::PosicaoNaGrade posicaoRealcada
) {
    const Dominio::Grade::TileDeTerra* tileRealcado = jogo.grade().obterTile(posicaoRealcada);
    if (jogo.ferramentaSelecionada() != Dominio::Ferramentas::TipoDeFerramenta::Enxada ||
        !Dominio::Grade::GradeGlobalDeCanteiros::posicaoEstaDentroDaGradeGlobal(posicaoRealcada) ||
        !Dominio::Grade::GradeGlobalDeCanteiros::posicaoEstaDentroDaGradeAtual(posicaoRealcada, jogo.tamanhoAtualDoGrid()) ||
        (tileRealcado != nullptr && tileRealcado->existeNoMapa())) {
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
