#pragma once

#include "Compartilhado/Constantes.hpp"
#include "Compartilhado/Geometria/Posicoes.hpp"
#include "Dominio/Animacao/AnimacaoIdle.hpp"

namespace MiniFazenda::Dominio::Personagem {

class Personagem {
public:
    Compartilhado::Geometria::PosicaoNaGrade posicaoNaGrade() const {
        return posicaoNaGrade_;
    }

    void avancarAnimacaoIdle(float deltaTime) {
        animacaoIdle_.avancar(deltaTime);
    }

    Compartilhado::Geometria::Retangulo retanguloDeOrigemIdle() const {
        return animacaoIdle_.calcularRetanguloDeOrigem();
    }

private:
    Compartilhado::Geometria::PosicaoNaGrade posicaoNaGrade_{
        Compartilhado::Constantes::COLUNA_CENTRAL_DA_GRADE_GLOBAL,
        Compartilhado::Constantes::LINHA_CENTRAL_DA_GRADE_GLOBAL
    };
    Animacao::AnimacaoIdle animacaoIdle_;
};

} // namespace MiniFazenda::Dominio::Personagem
