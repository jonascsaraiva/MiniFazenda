#pragma once

#include "Dominio/Ferramentas/Ferramenta.hpp"

namespace MiniFazenda::Dominio::Ferramentas {

class PresenteAcelerador final : public Ferramenta {
public:
    TipoDeFerramenta tipo() const override {
        return TipoDeFerramenta::Presente;
    }

    ResultadoDaFerramenta aplicar(
        ContextoDaFerramenta& contexto,
        Compartilhado::Geometria::PosicaoNaGrade posicao
    ) const override {
        if (!posicaoPodeReceberAcaoDaFerramenta(contexto.grade, posicao, contexto.tamanhoAtualDoGrid)) {
            return ResultadoDaFerramenta{};
        }

        Canteiros::Canteiro* canteiro = contexto.grade.obterCanteiro(posicao);
        if (canteiro == nullptr || !canteiro->acelerarParaMadura()) {
            return ResultadoDaFerramenta{};
        }

        contexto.grade.sincronizarCrescimentoDoCanteiro(posicao);
        return ResultadoDaFerramenta{AcaoDaFerramenta::AcelerarCrescimento};
    }
};

} // namespace MiniFazenda::Dominio::Ferramentas
