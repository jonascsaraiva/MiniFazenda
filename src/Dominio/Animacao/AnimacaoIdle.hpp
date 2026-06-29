#pragma once

namespace MiniFazenda::Dominio::Animacao {

class AnimacaoIdle {
public:
    void avancar(float deltaTime) {
        if (deltaTime <= 0.0f) {
            return;
        }

        tempoAcumulado_ += deltaTime;
    }

    float tempoAcumulado() const {
        return tempoAcumulado_;
    }

private:
    float tempoAcumulado_ = 0.0f;
};

} // namespace MiniFazenda::Dominio::Animacao
