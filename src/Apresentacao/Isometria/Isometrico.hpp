#pragma once

#include "Compartilhado/Constantes.hpp"
#include "Compartilhado/Geometria/Posicoes.hpp"

#include <cmath>

namespace MiniFazenda::Apresentacao::Isometria {

using Compartilhado::Geometria::PosicaoNaGrade;
using Compartilhado::Geometria::PosicaoNaTela;

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
    const double metadeDaLargura = static_cast<double>(larguraDoCanteiro / 2);
    const double metadeDaAltura = static_cast<double>(alturaDoCanteiro / 2);
    const double posicaoHorizontalAjustada =
        posicaoMouseHorizontal - deslocamentoHorizontal - metadeDaLargura;
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
    int origemHorizontal,
    int origemVertical,
    int cameraOffsetHorizontal,
    int cameraOffsetVertical
) {
    const int colunaLocal = posicaoGlobal.indiceColuna - Compartilhado::Constantes::COLUNA_DE_ORIGEM_VISUAL_DA_GRADE_GLOBAL;
    const int linhaLocal = posicaoGlobal.indiceLinha - Compartilhado::Constantes::LINHA_DE_ORIGEM_VISUAL_DA_GRADE_GLOBAL;
    const int deslocamentoHorizontal = origemHorizontal + cameraOffsetHorizontal;
    const int deslocamentoVertical = origemVertical + cameraOffsetVertical;

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
    int origemHorizontal,
    int origemVertical,
    int cameraOffsetHorizontal,
    int cameraOffsetVertical
) {
    const int deslocamentoHorizontal = origemHorizontal + cameraOffsetHorizontal;
    const int deslocamentoVertical = origemVertical + cameraOffsetVertical;
    const PosicaoNaGrade posicaoLocal = converterTelaParaGrade(
        posicaoMouseHorizontal,
        posicaoMouseVertical,
        larguraDoCanteiro,
        alturaDoCanteiro,
        deslocamentoHorizontal,
        deslocamentoVertical
    );

    return PosicaoNaGrade{
        Compartilhado::Constantes::COLUNA_DE_ORIGEM_VISUAL_DA_GRADE_GLOBAL + posicaoLocal.indiceColuna,
        Compartilhado::Constantes::LINHA_DE_ORIGEM_VISUAL_DA_GRADE_GLOBAL + posicaoLocal.indiceLinha
    };
}

} // namespace MiniFazenda::Apresentacao::Isometria
