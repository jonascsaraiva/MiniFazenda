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

inline bool posicaoEstaDentroDaGrade(PosicaoNaGrade posicao) {
    return posicao.indiceColuna >= 0 &&
           posicao.indiceColuna < Constantes::QUANTIDADE_DE_COLUNAS_DA_GRADE &&
           posicao.indiceLinha >= 0 &&
           posicao.indiceLinha < Constantes::QUANTIDADE_DE_LINHAS_DA_GRADE;
}

