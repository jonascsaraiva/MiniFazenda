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
        Compartilhado::Geometria::PosicaoNaGrade posicao
    ) const override {
        if (!posicaoPodeReceberAcaoDaFerramenta(contexto.grade, posicao, contexto.tamanhoAtualDoGrid)) {
            return ResultadoDaFerramenta{};
        }

        const Grade::TileDeTerra* tile = contexto.grade.obterTile(posicao);
        if (tile == nullptr || !tile->existeNoMapa()) {
            return ResultadoDaFerramenta{};
        }

        contexto.grade.removerTile(posicao);
        return ResultadoDaFerramenta{AcaoDaFerramenta::RemoverTerra};
    }
};

} // namespace MiniFazenda::Dominio::Ferramentas
