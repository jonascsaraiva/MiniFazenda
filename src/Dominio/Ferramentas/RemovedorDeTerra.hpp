#pragma once

#include "Dominio/Ferramentas/Ferramenta.hpp"

namespace MiniFazenda::Dominio::Ferramentas {

class RemovedorDeTerra final : public Ferramenta {
public:
    TipoDeFerramenta tipo() const override {
        return TipoDeFerramenta::RemoverTerra;
    }

    ResultadoDaFerramenta aplicar(
        ContextoDaFerramenta& contexto,
        Compartilhado::Geometria::PosicaoNaGradeDeOcupacao posicao
    ) const override {
        if (!posicaoDeOcupacaoPodeReceberAcaoDaFerramenta(contexto.mapa, posicao, contexto.tamanhoAtualDoGrid)) {
            return ResultadoDaFerramenta{};
        }

        if (!contexto.mapa.existeCanteiroEm(posicao)) {
            return ResultadoDaFerramenta{};
        }

        contexto.mapa.removerCanteiroEm(posicao);
        return ResultadoDaFerramenta{AcaoDaFerramenta::RemoverTerra};
    }
};

} // namespace MiniFazenda::Dominio::Ferramentas
