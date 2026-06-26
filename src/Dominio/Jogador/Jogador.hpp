#pragma once

#include "Dominio/Plantas/Planta.hpp"

namespace MiniFazenda::Dominio::Jogador {

class Jogador {
public:
    int moedas() const {
        return moedas_;
    }

    int experiencia() const {
        return experiencia_;
    }

    int nivel() const {
        return nivel_;
    }

    bool gastarMoedas(int quantidade) {
        if (quantidade < 0 || moedas_ < quantidade) {
            return false;
        }

        moedas_ -= quantidade;
        return true;
    }

    void adicionarRecompensa(Plantas::RecompensaDaColheita recompensa) {
        moedas_ += recompensa.moedas;
        experiencia_ += recompensa.experiencia;
        recalcularNivel();
    }

private:
    void recalcularNivel() {
        nivel_ = 1 + experiencia_ / 50;
    }

    int moedas_ = 50;
    int experiencia_ = 0;
    int nivel_ = 1;
};

} // namespace MiniFazenda::Dominio::Jogador
