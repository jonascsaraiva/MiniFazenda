#pragma once

#include "Apresentacao/Animacao/AnimadorDoPersonagem.hpp"
#include "Apresentacao/Camera/CameraDoJogo.hpp"
#include "Apresentacao/ConfiguracoesDoLayout.hpp"
#include "Apresentacao/Isometria/Isometrico.hpp"
#include "Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp"
#include "Compartilhado/Geometria/Posicoes.hpp"
#include "Dominio/Personagem/Personagem.hpp"
#include "Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp"
#include "Infraestrutura/Assets/RecursosDaFazenda.hpp"

#include <SDL.h>

namespace MiniFazenda::Apresentacao::Renderizacao::Mundo {

inline SDL_Rect converterRetanguloLogicoParaSDL(Compartilhado::Geometria::Retangulo retangulo) {
    return SDL_Rect{retangulo.x, retangulo.y, retangulo.w, retangulo.h};
}

inline void centralizarPontoDosPesDoPersonagemNaUnidadeDeOcupacao(
    Compartilhado::Geometria::PosicaoNaTela& pontoDosPes,
    const Camera::DimensoesDaUnidadeDeOcupacaoRenderizada& unidade
) {
    pontoDosPes.coordenadaHorizontal += unidade.largura / 2;
}

inline Compartilhado::Geometria::PosicaoNaTela calcularPontoDosPesDoPersonagemNaTela(
    Compartilhado::Geometria::PosicaoDecimalNaGradeDeOcupacao posicaoDosPesNaGradeDeOcupacao,
    const ConfiguracoesDoLayout& configuracoes,
    const Camera::EstadoDaCamera& camera
) {
    const Camera::DimensoesDaUnidadeDeOcupacaoRenderizada unidade =
        Camera::calcularDimensoesDaUnidadeDeOcupacaoRenderizada(camera.zoomAtual);

    Compartilhado::Geometria::PosicaoNaTela pontoDosPes =
        Isometria::converterCentroDaUnidadeDeOcupacaoGlobalParaTela(
            posicaoDosPesNaGradeDeOcupacao,
            unidade.largura,
            unidade.altura,
            configuracoes.origemGradeHorizontal,
            configuracoes.origemGradeVertical,
            camera.offsetHorizontal,
            camera.offsetVertical
        );

    centralizarPontoDosPesDoPersonagemNaUnidadeDeOcupacao(pontoDosPes, unidade);
    return pontoDosPes;
}

inline SDL_Rect calcularDestinoDoPersonagem(
    Compartilhado::Geometria::PosicaoDecimalNaGradeDeOcupacao posicaoDosPesNaGradeDeOcupacao,
    const Infraestrutura::Assets::ConfigVisualDoPersonagem::ConfiguracaoDaAnimacao& configuracaoVisual,
    const ConfiguracoesDoLayout& configuracoes,
    const Camera::EstadoDaCamera& camera
) {
    const Compartilhado::Geometria::PosicaoNaTela pontoDosPes =
        calcularPontoDosPesDoPersonagemNaTela(posicaoDosPesNaGradeDeOcupacao, configuracoes, camera);

    return converterRetanguloLogicoParaSDL(
        Infraestrutura::Assets::ConfigVisualDoPersonagem::calcularRetanguloDeDestino(
            configuracaoVisual,
            pontoDosPes,
            camera.zoomAtual
        )
    );
}

inline void desenharDebugDoPersonagem(
    SDL_Renderer* renderizador,
    SDL_Rect destinoDoPersonagem,
    Compartilhado::Geometria::PosicaoNaTela pontoDosPes
) {
    if constexpr (!Infraestrutura::Assets::ConfigVisualDoPersonagem::debugDesenhoDoPersonagem) {
        return;
    }

    SDL_SetRenderDrawColor(renderizador, 0, 255, 0, 190);
    SDL_RenderDrawRect(renderizador, &destinoDoPersonagem);

    SDL_SetRenderDrawColor(renderizador, 255, 32, 32, 220);
    SDL_RenderDrawLine(
        renderizador,
        pontoDosPes.coordenadaHorizontal - 4,
        pontoDosPes.coordenadaVertical,
        pontoDosPes.coordenadaHorizontal + 4,
        pontoDosPes.coordenadaVertical
    );
    SDL_RenderDrawLine(
        renderizador,
        pontoDosPes.coordenadaHorizontal,
        pontoDosPes.coordenadaVertical - 4,
        pontoDosPes.coordenadaHorizontal,
        pontoDosPes.coordenadaVertical + 4
    );
}

inline SDL_Texture* texturaParaAnimacaoDoPersonagem(
    const Infraestrutura::Assets::TexturasDoPersonagem& texturas,
    Infraestrutura::Assets::ConfigVisualDoPersonagem::AnimacaoVisualDoPersonagem animacao
) {
    const std::size_t indice = Infraestrutura::Assets::ConfigVisualDoPersonagem::indiceDaAnimacao(animacao);
    return indice < texturas.size() ? texturas[indice] : nullptr;
}

inline void desenharPersonagem(
    SDL_Renderer* renderizador,
    const Infraestrutura::Assets::TexturasDoPersonagem& texturasDoPersonagem,
    const Animacao::EstadoVisualDoPersonagem& estadoVisualDoPersonagem,
    const Dominio::Personagem::Personagem& personagem,
    const ConfiguracoesDoLayout& configuracoes,
    const Camera::EstadoDaCamera& camera
) {
    const auto animacao = estadoVisualDoPersonagem.animacaoAtual;
    const auto& configuracaoVisual =
        Infraestrutura::Assets::ConfigVisualDoPersonagem::configuracaoParaAnimacao(animacao);
    SDL_Texture* texturaDoPersonagem = texturaParaAnimacaoDoPersonagem(texturasDoPersonagem, animacao);

    if (texturaDoPersonagem == nullptr) {
        return;
    }

    const SDL_Rect destinoDoPersonagem = calcularDestinoDoPersonagem(
        personagem.posicaoDosPesNaGradeDeOcupacao(),
        configuracaoVisual,
        configuracoes,
        camera
    );

    if (!retanguloApareceNaTela(destinoDoPersonagem)) {
        return;
    }

    const SDL_Rect origem = converterRetanguloLogicoParaSDL(
        Infraestrutura::Assets::ConfigVisualDoPersonagem::calcularRetanguloDeOrigem(
            configuracaoVisual,
            estadoVisualDoPersonagem.indiceFrameAtual
        )
    );
    SDL_RenderCopy(renderizador, texturaDoPersonagem, &origem, &destinoDoPersonagem);

    const Compartilhado::Geometria::PosicaoNaTela pontoDosPes =
        calcularPontoDosPesDoPersonagemNaTela(personagem.posicaoDosPesNaGradeDeOcupacao(), configuracoes, camera);
    desenharDebugDoPersonagem(renderizador, destinoDoPersonagem, pontoDosPes);
}

} // namespace MiniFazenda::Apresentacao::Renderizacao::Mundo
