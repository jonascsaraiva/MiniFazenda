#pragma once

#include "Tipos.hpp"

#include <cmath>

inline PosicaoNaTela converterGradeParaTela(
    int colunaGrade,
    int linhaGrade,
    int larguraDoCanteiro,
    int alturaDoCanteiro,
    int deslocamentoHorizontal,
    int deslocamentoVertical
) {
    PosicaoNaTela posicaoCalculada;
    posicaoCalculada.coordenadaHorizontal =
        (colunaGrade - linhaGrade) * (larguraDoCanteiro / 2) + deslocamentoHorizontal;
    posicaoCalculada.coordenadaVertical =
        (colunaGrade + linhaGrade) * (alturaDoCanteiro / 2) + deslocamentoVertical;

    return posicaoCalculada;
}

inline PosicaoNaGrade converterTelaParaGrade(
    int posicaoMouseHorizontal,
    int posicaoMouseVertical,
    int larguraDoCanteiro,
    int alturaDoCanteiro,
    int deslocamentoHorizontal,
    int deslocamentoVertical
) {
    const double metadeDaLargura = larguraDoCanteiro / 2.0;
    const double metadeDaAltura = alturaDoCanteiro / 2.0;
    const double posicaoHorizontalAjustada = posicaoMouseHorizontal - deslocamentoHorizontal;
    const double posicaoVerticalAjustada = posicaoMouseVertical - deslocamentoVertical;

    PosicaoNaGrade gradeCalculada;
    gradeCalculada.indiceColuna = static_cast<int>(std::floor(
        (posicaoHorizontalAjustada / metadeDaLargura + posicaoVerticalAjustada / metadeDaAltura) / 2.0
    ));
    gradeCalculada.indiceLinha = static_cast<int>(std::floor(
        (posicaoVerticalAjustada / metadeDaAltura - posicaoHorizontalAjustada / metadeDaLargura) / 2.0
    ));

    return gradeCalculada;
}

inline PosicaoNaTela converterGradeGlobalParaTela(
    PosicaoNaGrade posicaoGlobal,
    int larguraDoCanteiro,
    int alturaDoCanteiro,
    int deslocamentoHorizontal,
    int deslocamentoVertical
) {
    const int colunaLocal = posicaoGlobal.indiceColuna - Constantes::COLUNA_DE_ORIGEM_VISUAL_DA_GRADE_GLOBAL;
    const int linhaLocal = posicaoGlobal.indiceLinha - Constantes::LINHA_DE_ORIGEM_VISUAL_DA_GRADE_GLOBAL;

    return converterGradeParaTela(
        colunaLocal,
        linhaLocal,
        larguraDoCanteiro,
        alturaDoCanteiro,
        deslocamentoHorizontal,
        deslocamentoVertical
    );
}

inline PosicaoNaGrade converterTelaParaGradeGlobal(
    int posicaoMouseHorizontal,
    int posicaoMouseVertical,
    int larguraDoCanteiro,
    int alturaDoCanteiro,
    int deslocamentoHorizontal,
    int deslocamentoVertical
) {
    const PosicaoNaGrade posicaoLocal = converterTelaParaGrade(
        posicaoMouseHorizontal,
        posicaoMouseVertical,
        larguraDoCanteiro,
        alturaDoCanteiro,
        deslocamentoHorizontal,
        deslocamentoVertical
    );

    return PosicaoNaGrade{
        Constantes::COLUNA_DE_ORIGEM_VISUAL_DA_GRADE_GLOBAL + posicaoLocal.indiceColuna,
        Constantes::LINHA_DE_ORIGEM_VISUAL_DA_GRADE_GLOBAL + posicaoLocal.indiceLinha
    };
}
