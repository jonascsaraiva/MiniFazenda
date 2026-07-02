#pragma once

#include "Compartilhado/ConstantesDaIsometria.hpp"
#include "Compartilhado/ConstantesDoJogo.hpp"
#include "Compartilhado/Geometria/Posicoes.hpp"

#include <algorithm>
#include <cmath>

namespace MiniFazenda::Apresentacao::Isometria {

using Compartilhado::Geometria::AreaNaGradeDeOcupacao;
using Compartilhado::Geometria::PosicaoNaGrade;
using Compartilhado::Geometria::PosicaoNaGradeDeOcupacao;
using Compartilhado::Geometria::PosicaoNaTela;
using Compartilhado::Geometria::Retangulo;

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

inline PosicaoNaTela converterOcupacaoParaTela(
    int colunaDeOcupacao,
    int linhaDeOcupacao,
    int larguraDaUnidadeDeOcupacao,
    int alturaDaUnidadeDeOcupacao,
    int deslocamentoHorizontal,
    int deslocamentoVertical
) {
    return converterGradeParaTela(
        colunaDeOcupacao,
        linhaDeOcupacao,
        larguraDaUnidadeDeOcupacao,
        alturaDaUnidadeDeOcupacao,
        deslocamentoHorizontal,
        deslocamentoVertical
    );
}

inline PosicaoNaGradeDeOcupacao converterTelaParaOcupacao(
    int posicaoMouseHorizontal,
    int posicaoMouseVertical,
    int larguraDaUnidadeDeOcupacao,
    int alturaDaUnidadeDeOcupacao,
    int deslocamentoHorizontal,
    int deslocamentoVertical
) {
    const PosicaoNaGrade posicaoLocal = converterTelaParaGrade(
        posicaoMouseHorizontal,
        posicaoMouseVertical,
        larguraDaUnidadeDeOcupacao,
        alturaDaUnidadeDeOcupacao,
        deslocamentoHorizontal,
        deslocamentoVertical
    );

    return PosicaoNaGradeDeOcupacao{posicaoLocal.indiceColuna, posicaoLocal.indiceLinha};
}

inline PosicaoNaTela converterOcupacaoGlobalParaTela(
    PosicaoNaGradeDeOcupacao posicaoGlobal,
    int larguraDaUnidadeDeOcupacao,
    int alturaDaUnidadeDeOcupacao,
    int origemHorizontal,
    int origemVertical,
    int cameraOffsetHorizontal,
    int cameraOffsetVertical
) {
    const int colunaLocal =
        posicaoGlobal.indiceColuna -
        Compartilhado::Constantes::COLUNA_DE_ORIGEM_VISUAL_DA_GRADE_GLOBAL *
            Compartilhado::Constantes::UNIDADES_DE_OCUPACAO_POR_CANTEIRO;
    const int linhaLocal =
        posicaoGlobal.indiceLinha -
        Compartilhado::Constantes::LINHA_DE_ORIGEM_VISUAL_DA_GRADE_GLOBAL *
            Compartilhado::Constantes::UNIDADES_DE_OCUPACAO_POR_CANTEIRO;
    const int deslocamentoHorizontal = origemHorizontal + cameraOffsetHorizontal;
    const int deslocamentoVertical = origemVertical + cameraOffsetVertical;

    return converterOcupacaoParaTela(
        colunaLocal,
        linhaLocal,
        larguraDaUnidadeDeOcupacao,
        alturaDaUnidadeDeOcupacao,
        deslocamentoHorizontal,
        deslocamentoVertical
    );
}

inline PosicaoNaGradeDeOcupacao converterTelaParaOcupacaoGlobal(
    int posicaoMouseHorizontal,
    int posicaoMouseVertical,
    int larguraDaUnidadeDeOcupacao,
    int alturaDaUnidadeDeOcupacao,
    int origemHorizontal,
    int origemVertical,
    int cameraOffsetHorizontal,
    int cameraOffsetVertical
) {
    const int deslocamentoHorizontal = origemHorizontal + cameraOffsetHorizontal;
    const int deslocamentoVertical = origemVertical + cameraOffsetVertical;
    const PosicaoNaGradeDeOcupacao posicaoLocal = converterTelaParaOcupacao(
        posicaoMouseHorizontal,
        posicaoMouseVertical,
        larguraDaUnidadeDeOcupacao,
        alturaDaUnidadeDeOcupacao,
        deslocamentoHorizontal,
        deslocamentoVertical
    );

    return PosicaoNaGradeDeOcupacao{
        Compartilhado::Constantes::COLUNA_DE_ORIGEM_VISUAL_DA_GRADE_GLOBAL *
                Compartilhado::Constantes::UNIDADES_DE_OCUPACAO_POR_CANTEIRO +
            posicaoLocal.indiceColuna,
        Compartilhado::Constantes::LINHA_DE_ORIGEM_VISUAL_DA_GRADE_GLOBAL *
                Compartilhado::Constantes::UNIDADES_DE_OCUPACAO_POR_CANTEIRO +
            posicaoLocal.indiceLinha
    };
}

inline Retangulo calcularRetanguloDaAreaDeOcupacaoGlobal(
    AreaNaGradeDeOcupacao area,
    int larguraDaUnidadeDeOcupacao,
    int alturaDaUnidadeDeOcupacao,
    int origemHorizontal,
    int origemVertical,
    int cameraOffsetHorizontal,
    int cameraOffsetVertical
) {
    const PosicaoNaTela origem = converterOcupacaoGlobalParaTela(
        PosicaoNaGradeDeOcupacao{area.indiceColuna, area.indiceLinha},
        larguraDaUnidadeDeOcupacao,
        alturaDaUnidadeDeOcupacao,
        origemHorizontal,
        origemVertical,
        cameraOffsetHorizontal,
        cameraOffsetVertical
    );
    const int somaDasDimensoes = std::max(1, area.largura + area.altura);

    return Retangulo{
        origem.coordenadaHorizontal,
        origem.coordenadaVertical,
        std::max(1, (somaDasDimensoes * larguraDaUnidadeDeOcupacao) / 2),
        std::max(1, (somaDasDimensoes * alturaDaUnidadeDeOcupacao) / 2)
    };
}

} // namespace MiniFazenda::Apresentacao::Isometria
