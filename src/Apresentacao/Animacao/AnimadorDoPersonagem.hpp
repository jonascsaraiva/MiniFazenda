#pragma once

#include "Apresentacao/Animacao/AnimacaoIdleDoPersonagem.hpp"
#include "Dominio/Personagem/Personagem.hpp"
#include "Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp"

#include <algorithm>
#include <cmath>

namespace MiniFazenda::Apresentacao::Animacao {

namespace ConfigPersonagem = MiniFazenda::Infraestrutura::Assets::ConfigVisualDoPersonagem;

struct EstadoVisualDoPersonagem {
    ConfigPersonagem::AnimacaoVisualDoPersonagem animacaoAtual = ConfigPersonagem::AnimacaoVisualDoPersonagem::Idle;
    int indiceFrameAtual = 0;
    float tempoAcumulado = 0.0f;
    AnimacaoIdleDoPersonagem animacaoIdle;
};

inline ConfigPersonagem::AnimacaoVisualDoPersonagem animacaoVisualParaEstadoDoPersonagem(
    const Dominio::Personagem::Personagem& personagem
) {
    if (personagem.estadoAtual() == Dominio::Personagem::EstadoDoPersonagem::Parado) {
        return ConfigPersonagem::AnimacaoVisualDoPersonagem::Idle;
    }

    switch (personagem.direcaoAtual()) {
        case Dominio::Personagem::DirecaoIsometrica::BaixoDireita:
            return ConfigPersonagem::AnimacaoVisualDoPersonagem::WalkBaixoDireita;
        case Dominio::Personagem::DirecaoIsometrica::BaixoEsquerda:
            return ConfigPersonagem::AnimacaoVisualDoPersonagem::WalkBaixoEsquerda;
        case Dominio::Personagem::DirecaoIsometrica::CimaDireita:
            return ConfigPersonagem::AnimacaoVisualDoPersonagem::WalkCimaDireita;
        case Dominio::Personagem::DirecaoIsometrica::CimaEsquerda:
        default:
            return ConfigPersonagem::AnimacaoVisualDoPersonagem::WalkCimaEsquerda;
    }
}

inline void reiniciarAnimacaoVisualDoPersonagem(
    EstadoVisualDoPersonagem& estadoVisual,
    ConfigPersonagem::AnimacaoVisualDoPersonagem animacao
) {
    estadoVisual.animacaoAtual = animacao;
    estadoVisual.indiceFrameAtual = 0;
    estadoVisual.tempoAcumulado = 0.0f;

    const auto& configuracao = ConfigPersonagem::configuracaoParaAnimacao(animacao);
    if (configuracao.modoDeReproducao == ConfigPersonagem::ModoDeReproducaoDaAnimacao::IdleComPiscadas) {
        estadoVisual.animacaoIdle.reiniciar();
    }
}

inline void avancarLoopDeFrames(
    EstadoVisualDoPersonagem& estadoVisual,
    const ConfigPersonagem::ConfiguracaoDaAnimacao& configuracao,
    float deltaTime
) {
    if (configuracao.quantidadeFrames <= 1) {
        estadoVisual.indiceFrameAtual = 0;
        estadoVisual.tempoAcumulado = 0.0f;
        return;
    }

    if (deltaTime <= 0.0f) {
        return;
    }

    const float duracaoPorFrame = std::max(0.001f, configuracao.duracaoPorFrame);
    estadoVisual.tempoAcumulado += deltaTime;

    const int framesAvancados = static_cast<int>(estadoVisual.tempoAcumulado / duracaoPorFrame);
    if (framesAvancados <= 0) {
        return;
    }

    estadoVisual.tempoAcumulado = std::fmod(estadoVisual.tempoAcumulado, duracaoPorFrame);
    estadoVisual.indiceFrameAtual =
        (estadoVisual.indiceFrameAtual + framesAvancados) % configuracao.quantidadeFrames;
}

inline void avancarAnimacaoDoPersonagem(
    EstadoVisualDoPersonagem& estadoVisual,
    const Dominio::Personagem::Personagem& personagem,
    float deltaTime
) {
    const ConfigPersonagem::AnimacaoVisualDoPersonagem animacaoDesejada =
        animacaoVisualParaEstadoDoPersonagem(personagem);

    if (estadoVisual.animacaoAtual != animacaoDesejada) {
        reiniciarAnimacaoVisualDoPersonagem(estadoVisual, animacaoDesejada);
        return;
    }

    const auto& configuracao = ConfigPersonagem::configuracaoParaAnimacao(estadoVisual.animacaoAtual);
    switch (configuracao.modoDeReproducao) {
        case ConfigPersonagem::ModoDeReproducaoDaAnimacao::IdleComPiscadas:
            estadoVisual.animacaoIdle.avancar(deltaTime);
            estadoVisual.indiceFrameAtual = estadoVisual.animacaoIdle.indiceFrameAtual();
            return;
        case ConfigPersonagem::ModoDeReproducaoDaAnimacao::LoopContinuo:
            avancarLoopDeFrames(estadoVisual, configuracao, deltaTime);
            return;
        case ConfigPersonagem::ModoDeReproducaoDaAnimacao::FrameFixo:
        default:
            estadoVisual.indiceFrameAtual = 0;
            estadoVisual.tempoAcumulado = 0.0f;
            return;
    }
}

} // namespace MiniFazenda::Apresentacao::Animacao
