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
        Compartilhado::Geometria::PosicaoNaGrade posicao
    ) const override {
        if (!posicaoPodeReceberAcaoDaFerramenta(contexto.grade, posicao, contexto.tamanhoAtualDoGrid)) {
            return ResultadoDaFerramenta{};
        }

        Grade::TileDeTerra* tile = contexto.grade.obterTile(posicao);
        if (tile == nullptr) {
            return ResultadoDaFerramenta{};
        }

        if (!tile->existeNoMapa()) {
            contexto.grade.ativarTile(posicao);
            return ResultadoDaFerramenta{AcaoDaFerramenta::CriarTerra};
        }

        Canteiros::Canteiro* canteiro = contexto.grade.obterCanteiro(posicao);
        if (canteiro == nullptr || !canteiro->arar()) {
            return ResultadoDaFerramenta{};
        }

        contexto.grade.sincronizarCrescimentoDoCanteiro(posicao);
        return ResultadoDaFerramenta{AcaoDaFerramenta::ArarTerra};
    }
};

} // namespace MiniFazenda::Dominio::Ferramentas
