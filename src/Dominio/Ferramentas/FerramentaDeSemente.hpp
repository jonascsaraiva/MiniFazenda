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
        Compartilhado::Geometria::PosicaoDeCanteiroNoMapa posicao
    ) const override {
        if (!posicaoPodeReceberAcaoDaFerramenta(contexto.mapa, posicao, contexto.tamanhoAtualDoGrid)) {
            return ResultadoDaFerramenta{};
        }

        Canteiros::Canteiro* canteiro = contexto.mapa.obterCanteiroAgricola(posicao);
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

        contexto.mapa.sincronizarCrescimentoDoCanteiro(posicao);
        return ResultadoDaFerramenta{AcaoDaFerramenta::Plantar};
    }

private:
    Plantas::FabricaDePlantas fabricaDePlantas_;
};

} // namespace MiniFazenda::Dominio::Ferramentas
