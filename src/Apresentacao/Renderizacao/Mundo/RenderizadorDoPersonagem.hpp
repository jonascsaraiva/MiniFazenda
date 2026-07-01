#pragma once

#include "Apresentacao/Camera/CameraDoJogo.hpp"
#include "Apresentacao/ConfiguracoesDoLayout.hpp"
#include "Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp"
#include "Compartilhado/Geometria/Posicoes.hpp"
#include "Dominio/Personagem/Personagem.hpp"
#include "Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp"
#include "Infraestrutura/Assets/RecursosDaFazenda.hpp"

#include <SDL.h>

#include <cmath>

namespace MiniFazenda::Apresentacao::Renderizacao::Mundo {

inline SDL_Rect converterRetanguloLogicoParaSDL(Compartilhado::Geometria::Retangulo retangulo) {
    return SDL_Rect{retangulo.x, retangulo.y, retangulo.w, retangulo.h};
}

inline Compartilhado::Geometria::PosicaoNaTela calcularPontoDosPesDoPersonagemNaTela(
    Compartilhado::Geometria::PosicaoNaGradeDecimal posicaoDosPesNaGrade,
    const ConfiguracoesDoLayout& configuracoes,
    const Camera::EstadoDaCamera& camera
) {
    const Compartilhado::Geometria::PosicaoNaGrade posicaoBase{
        static_cast<int>(std::floor(posicaoDosPesNaGrade.indiceColuna)),
        static_cast<int>(std::floor(posicaoDosPesNaGrade.indiceLinha))
    };
    const SDL_Rect destinoDoCanteiroBase = calcularDestinoDoCanteiro(posicaoBase, configuracoes, camera);
    Compartilhado::Geometria::PosicaoNaTela pontoDosPes{
        destinoDoCanteiroBase.x + destinoDoCanteiroBase.w / 2,
        destinoDoCanteiroBase.y + destinoDoCanteiroBase.h / 2
    };

    const Camera::DimensoesDoCanteiroRenderizado dimensoes =
        Camera::calcularDimensoesDoCanteiroRenderizado(camera.zoomAtual);
    const float deslocamentoColuna = posicaoDosPesNaGrade.indiceColuna - posicaoBase.indiceColuna;
    const float deslocamentoLinha = posicaoDosPesNaGrade.indiceLinha - posicaoBase.indiceLinha;

    pontoDosPes.coordenadaHorizontal += static_cast<int>(
        std::lround((deslocamentoColuna - deslocamentoLinha) * (dimensoes.largura / 2))
    );
    pontoDosPes.coordenadaVertical += static_cast<int>(
        std::lround((deslocamentoColuna + deslocamentoLinha) * (dimensoes.altura / 2))
    );

    return pontoDosPes;
}

inline SDL_Rect calcularDestinoDoPersonagem(
    Compartilhado::Geometria::PosicaoNaGradeDecimal posicaoDosPesNaGrade,
    const Infraestrutura::Assets::ConfigVisualDoPersonagem::ConfiguracaoDaAnimacao& configuracaoVisual,
    const ConfiguracoesDoLayout& configuracoes,
    const Camera::EstadoDaCamera& camera
) {
    const Compartilhado::Geometria::PosicaoNaTela pontoDosPes =
        calcularPontoDosPesDoPersonagemNaTela(posicaoDosPesNaGrade, configuracoes, camera);

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
    Dominio::Personagem::AnimacaoVisualDoPersonagem animacao
) {
    const std::size_t indice = Infraestrutura::Assets::ConfigVisualDoPersonagem::indiceDaAnimacao(animacao);
    return indice < texturas.size() ? texturas[indice] : nullptr;
}

inline void desenharPersonagem(
    SDL_Renderer* renderizador,
    const Infraestrutura::Assets::TexturasDoPersonagem& texturasDoPersonagem,
    const Dominio::Personagem::Personagem& personagem,
    const ConfiguracoesDoLayout& configuracoes,
    const Camera::EstadoDaCamera& camera
) {
    const Dominio::Personagem::AnimacaoVisualDoPersonagem animacao = personagem.animacaoVisualAtual();
    const auto& configuracaoVisual =
        Infraestrutura::Assets::ConfigVisualDoPersonagem::configuracaoParaAnimacao(animacao);
    SDL_Texture* texturaDoPersonagem = texturaParaAnimacaoDoPersonagem(texturasDoPersonagem, animacao);

    if (texturaDoPersonagem == nullptr) {
        return;
    }

    const SDL_Rect destinoDoPersonagem = calcularDestinoDoPersonagem(
        personagem.posicaoDosPesNaGrade(),
        configuracaoVisual,
        configuracoes,
        camera
    );

    if (!retanguloApareceNaTela(destinoDoPersonagem)) {
        return;
    }

    const int indiceFrame = personagem.indiceFrameDaAnimacaoVisualAtual();
    const SDL_Rect origem = converterRetanguloLogicoParaSDL(
        Infraestrutura::Assets::ConfigVisualDoPersonagem::calcularRetanguloDeOrigem(
            configuracaoVisual,
            indiceFrame
        )
    );
    SDL_RenderCopy(renderizador, texturaDoPersonagem, &origem, &destinoDoPersonagem);

    const Compartilhado::Geometria::PosicaoNaTela pontoDosPes =
        calcularPontoDosPesDoPersonagemNaTela(personagem.posicaoDosPesNaGrade(), configuracoes, camera);
    desenharDebugDoPersonagem(renderizador, destinoDoPersonagem, pontoDosPes);
}

} // namespace MiniFazenda::Apresentacao::Renderizacao::Mundo
