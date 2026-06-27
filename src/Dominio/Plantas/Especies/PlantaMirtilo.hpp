#pragma once

#include "Dominio/Plantas/Planta.hpp"

#include <memory>
#include <string>

namespace MiniFazenda::Dominio::Plantas::Especies {

class PlantaMirtilo final : public Planta {
public:
    static constexpr int IDENTIFICADOR_DA_SEMENTE = 1;
    static constexpr int SEGUNDOS_POR_HORA = 60 * 60;
    static constexpr int CUSTO_EM_MOEDAS = 2;
    static constexpr int DURACAO_ATE_COLHEITA_EM_SEGUNDOS = 4 * SEGUNDOS_POR_HORA;
    static constexpr int DURACAO_ATE_CRESCIMENTO_EM_SEGUNDOS = DURACAO_ATE_COLHEITA_EM_SEGUNDOS / 3;
    static constexpr int DURACAO_ATE_JUVENTUDE_EM_SEGUNDOS = (DURACAO_ATE_COLHEITA_EM_SEGUNDOS * 2) / 3;
    static constexpr int DURACAO_ATE_MATURIDADE_EM_SEGUNDOS = DURACAO_ATE_COLHEITA_EM_SEGUNDOS;
    static constexpr int DURACAO_ATE_MORTE_EM_SEGUNDOS = DURACAO_ATE_COLHEITA_EM_SEGUNDOS * 2;

    std::unique_ptr<Planta> clonar() const override {
        return std::make_unique<PlantaMirtilo>(*this);
    }

    int identificadorDaSemente() const override {
        return IDENTIFICADOR_DA_SEMENTE;
    }

    std::string nome() const override {
        return "Mirtilo";
    }

    // Sprites esperados em assets/sprites/plantas/mirtilo:
    // mirtilo_fase_1.png = semente plantada; mirtilo_fase_2.png = crescendo;
    // mirtilo_fase_3.png = jovem; mirtilo_fase_4.png = madura; mirtilo_morto.png = morta.
    std::string pastaDeSprites() const override {
        return "mirtilo";
    }

    int custoEmMoedas() const override {
        return CUSTO_EM_MOEDAS;
    }

    int tempoParaCrescer() const override {
        return DURACAO_ATE_CRESCIMENTO_EM_SEGUNDOS;
    }

    int tempoParaFicarJovem() const override {
        return DURACAO_ATE_JUVENTUDE_EM_SEGUNDOS;
    }

    int tempoParaMaturar() const override {
        return DURACAO_ATE_MATURIDADE_EM_SEGUNDOS;
    }

    int tempoParaMorrer() const override {
        return DURACAO_ATE_MORTE_EM_SEGUNDOS;
    }

    RecompensaDaColheita recompensaDaColheita() const override {
        return RecompensaDaColheita{8, 5};
    }
};

} // namespace MiniFazenda::Dominio::Plantas::Especies
