#pragma once

#include "Dominio/Ferramentas/Ferramenta.hpp"
#include "Dominio/Plantas/FabricaDePlantas.hpp"

namespace MiniFazenda::Dominio::Ferramentas {

class FerramentaDeSemente final : public Ferramenta {
public:
    TipoDeFerramenta tipo() const override {
        return TipoDeFerramenta::Semente;
    }

    ResultadoDaFerramenta aplicar(
        ContextoDaFerramenta& contexto,
        Compartilhado::Geometria::PosicaoNaGrade posicao
    ) const override {
        if (!posicaoPodeReceberAcaoDaFerramenta(contexto.grade, posicao, contexto.tamanhoAtualDoGrid)) {
            return ResultadoDaFerramenta{};
        }

        Canteiros::Canteiro* canteiro = contexto.grade.obterCanteiro(posicao);
        if (canteiro == nullptr || !canteiro->estaArado() || !contexto.jogador.gastarMoedas(CUSTO_DA_SEMENTE)) {
            return ResultadoDaFerramenta{};
        }

        if (!canteiro->plantar(fabricaDePlantas_.criarPlantaInicial())) {
            return ResultadoDaFerramenta{};
        }

        contexto.grade.sincronizarCrescimentoDoCanteiro(posicao);
        return ResultadoDaFerramenta{AcaoDaFerramenta::Plantar};
    }

private:
    static constexpr int CUSTO_DA_SEMENTE = 2;
    Plantas::FabricaDePlantas fabricaDePlantas_;
};

} // namespace MiniFazenda::Dominio::Ferramentas
