#pragma once

#include "Dominio/Plantas/Planta.hpp"

#include <memory>
#include <string>

namespace MiniFazenda::Dominio::Plantas::Especies {

class PlantaMirtilo final : public Planta {
public:
    static constexpr int IDENTIFICADOR_DA_SEMENTE = 1;
    static constexpr int SEGUNDOS_POR_HORA = 60 * 60;
    static constexpr int CUSTO_EM_MOEDAS = 15;
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

    int custoEmMoedas() const override {
        return CUSTO_EM_MOEDAS;
    }

    int tempoParaCrescer() const override {
        return DURACAO_ATE_CRESCIMENTO_EM_SEGUNDOS; // A planta começa a crescer após o tempo definido para crescimento.
    }

    int tempoParaFicarJovem() const override {
        return DURACAO_ATE_JUVENTUDE_EM_SEGUNDOS; // A planta atinge a juventude após o tempo definido para juventude.
    }

    int tempoParaMaturar() const override {
        return DURACAO_ATE_MATURIDADE_EM_SEGUNDOS; // A planta atinge a maturidade após o tempo definido para colheita.
    }

    int tempoParaMorrer() const override {
        return DURACAO_ATE_MORTE_EM_SEGUNDOS; // A planta morre após o dobro do tempo de maturidade.
    }

    RecompensaDaColheita recompensaDaColheita() const override {
        return RecompensaDaColheita{35,3}; // 35 moedas e 3 de experiência como recompensa da colheita.
    }
};

} // namespace MiniFazenda::Dominio::Plantas::Especies
