#pragma once

#include "Compartilhado/Animacao/ConfigAnimacao.hpp"
#include "Compartilhado/Geometria/Posicoes.hpp"

namespace MiniFazenda::Dominio::Animacao {

class AnimacaoIdle {
public:
    void avancar(float deltaTime) {
        if (deltaTime <= 0.0f) {
            return;
        }

        tempoAcumulado_ += deltaTime;

        while (tempoAcumulado_ >= Compartilhado::ConfigAnimacao::VELOCIDADE_FRAME) {
            tempoAcumulado_ -= Compartilhado::ConfigAnimacao::VELOCIDADE_FRAME;
            frameAtual_ = (frameAtual_ + 1) % Compartilhado::ConfigAnimacao::TOTAL_FRAMES_IDLE;
        }
    }

    Compartilhado::Geometria::Retangulo calcularRetanguloDeOrigem() const {
        return Compartilhado::Geometria::Retangulo{
            frameAtual_ * Compartilhado::ConfigAnimacao::FRAME_LARGURA,
            Compartilhado::ConfigAnimacao::FRAME_ORIGEM_Y,
            Compartilhado::ConfigAnimacao::FRAME_LARGURA,
            Compartilhado::ConfigAnimacao::FRAME_ALTURA
        };
    }

private:
    float tempoAcumulado_ = 0.0f;
    int frameAtual_ = 0;
};

} // namespace MiniFazenda::Dominio::Animacao
