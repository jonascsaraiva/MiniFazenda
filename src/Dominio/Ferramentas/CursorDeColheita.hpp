#pragma once

#include "Dominio/Ferramentas/Ferramenta.hpp"

namespace MiniFazenda::Dominio::Ferramentas {

class CursorDeColheita final : public Ferramenta {
public:
    TipoDeFerramenta tipo() const override {
        return TipoDeFerramenta::Cursor;
    }

    ResultadoDaFerramenta aplicar(
        ContextoDaFerramenta& contexto,
        Compartilhado::Geometria::PosicaoNaGrade posicao
    ) const override {
        if (!posicaoPodeReceberAcaoDaFerramenta(contexto.grade, posicao, contexto.tamanhoAtualDoGrid)) {
            return ResultadoDaFerramenta{};
        }

        Canteiros::Canteiro* canteiro = contexto.grade.obterCanteiro(posicao);
        if (canteiro == nullptr) {
            return ResultadoDaFerramenta{};
        }

        const auto recompensa = canteiro->colher();
        if (!recompensa.has_value()) {
            return ResultadoDaFerramenta{};
        }

        contexto.jogador.adicionarRecompensa(*recompensa);
        contexto.grade.sincronizarCrescimentoDoCanteiro(posicao);
        return ResultadoDaFerramenta{AcaoDaFerramenta::Colher};
    }
};

} // namespace MiniFazenda::Dominio::Ferramentas
