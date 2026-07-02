#pragma once

#include "Dominio/Ferramentas/Ferramenta.hpp"

namespace MiniFazenda::Dominio::Ferramentas {

class Enxada final : public Ferramenta {
public:
    TipoDeFerramenta tipo() const override {
        return TipoDeFerramenta::Enxada;
    }

    ResultadoDaFerramenta aplicar(
        ContextoDaFerramenta& contexto,
        Compartilhado::Geometria::PosicaoNaGradeDeOcupacao posicao
    ) const override {
        if (!posicaoDeOcupacaoPodeReceberAcaoDaFerramenta(contexto.mapa, posicao, contexto.tamanhoAtualDoGrid)) {
            return ResultadoDaFerramenta{};
        }

        if (!contexto.mapa.existeCanteiroEm(posicao)) {
            if (!contexto.mapa.criarCanteiroEm(posicao, contexto.tamanhoAtualDoGrid).has_value()) {
                return ResultadoDaFerramenta{};
            }
            return ResultadoDaFerramenta{AcaoDaFerramenta::CriarTerra};
        }

        Canteiros::Canteiro* canteiro = contexto.mapa.obterCanteiroAgricolaEm(posicao);
        if (canteiro == nullptr) {
            return ResultadoDaFerramenta{};
        }

        if (canteiro->limparPlantaMorta() || canteiro->limparRestos()) {
            contexto.mapa.sincronizarCrescimentoDoCanteiroEm(posicao);
            return ResultadoDaFerramenta{AcaoDaFerramenta::LimparCanteiro};
        }

        if (!canteiro->arar()) {
            return ResultadoDaFerramenta{};
        }

        contexto.mapa.sincronizarCrescimentoDoCanteiroEm(posicao);
        return ResultadoDaFerramenta{AcaoDaFerramenta::ArarTerra};
    }
};

} // namespace MiniFazenda::Dominio::Ferramentas
