#pragma once

#include "Dominio/Ferramentas/Ferramenta.hpp"
#include "Dominio/Plantas/FabricaDePlantas.hpp"

#include <memory>
#include <utility>

namespace MiniFazenda::Dominio::Ferramentas {

class FerramentaDeSemente final : public Ferramenta {
public:
    TipoDeFerramenta tipo() const override {
        return TipoDeFerramenta::Semente;
    }

    ResultadoDaFerramenta aplicar(
        ContextoDaFerramenta& contexto,
        Compartilhado::Geometria::PosicaoNaGradeDeOcupacao posicao
    ) const override {
        if (!posicaoDeOcupacaoPodeReceberAcaoDaFerramenta(contexto.mapa, posicao, contexto.tamanhoAtualDoGrid)) {
            return ResultadoDaFerramenta{};
        }

        Canteiros::Canteiro* canteiro = contexto.mapa.obterCanteiroAgricolaEm(posicao);
        if (canteiro == nullptr || !canteiro->estaArado()) {
            return ResultadoDaFerramenta{};
        }

        if (!contexto.identificadorDaSementeSelecionada.has_value()) {
            return ResultadoDaFerramenta{};
        }

        std::unique_ptr<Plantas::Planta> planta =
            fabricaDePlantas_.criarPorIdentificadorDeSemente(*contexto.identificadorDaSementeSelecionada);
        if (planta == nullptr || !contexto.jogador.gastarMoedas(planta->custoEmMoedas())) {
            return ResultadoDaFerramenta{};
        }

        if (!canteiro->plantar(std::move(planta))) {
            return ResultadoDaFerramenta{};
        }

        contexto.mapa.sincronizarCrescimentoDoCanteiroEm(posicao);
        return ResultadoDaFerramenta{AcaoDaFerramenta::Plantar};
    }

private:
    Plantas::FabricaDePlantas fabricaDePlantas_;
};

} // namespace MiniFazenda::Dominio::Ferramentas
