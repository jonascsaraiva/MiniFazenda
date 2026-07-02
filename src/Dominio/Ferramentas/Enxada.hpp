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
        Compartilhado::Geometria::PosicaoDeCanteiroNoMapa posicao
    ) const override {
        if (!posicaoPodeReceberAcaoDaFerramenta(contexto.mapa, posicao, contexto.tamanhoAtualDoGrid)) {
            return ResultadoDaFerramenta{};
        }

        if (!contexto.mapa.existeCanteiroEm(posicao)) {
            if (!contexto.mapa.criarCanteiro(posicao).has_value()) {
                return ResultadoDaFerramenta{};
            }
            return ResultadoDaFerramenta{AcaoDaFerramenta::CriarTerra};
        }

        Canteiros::Canteiro* canteiro = contexto.mapa.obterCanteiroAgricola(posicao);
        if (canteiro == nullptr) {
            return ResultadoDaFerramenta{};
        }

        if (canteiro->limparPlantaMorta() || canteiro->limparRestos()) {
            contexto.mapa.sincronizarCrescimentoDoCanteiro(posicao);
            return ResultadoDaFerramenta{AcaoDaFerramenta::LimparCanteiro};
        }

        if (!canteiro->arar()) {
            return ResultadoDaFerramenta{};
        }

        contexto.mapa.sincronizarCrescimentoDoCanteiro(posicao);
        return ResultadoDaFerramenta{AcaoDaFerramenta::ArarTerra};
    }
};

} // namespace MiniFazenda::Dominio::Ferramentas
