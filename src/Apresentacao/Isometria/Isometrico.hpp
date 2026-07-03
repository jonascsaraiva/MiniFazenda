#pragma once

#include "Compartilhado/ConstantesDaIsometria.hpp"
#include "Compartilhado/ConstantesDoJogo.hpp"
#include "Compartilhado/Geometria/Posicoes.hpp"

#include <algorithm>
#include <cmath>

namespace MiniFazenda::Apresentacao::Isometria {

using Compartilhado::Geometria::AreaNaGradeDeOcupacao;
using Compartilhado::Geometria::PosicaoDecimalNaGradeDeOcupacao;
using Compartilhado::Geometria::PosicaoNaGrade;
using Compartilhado::Geometria::PosicaoNaGradeDecimal;
using Compartilhado::Geometria::PosicaoNaGradeDeOcupacao;
using Compartilhado::Geometria::PosicaoNaTela;
using Compartilhado::Geometria::Retangulo;

inline int calcularMetadeDaDimensaoIsometrica(int dimensao) {
    return std::max(1, dimensao / 2);
}

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
        (colunaGrade - linhaGrade) * calcularMetadeDaDimensaoIsometrica(larguraDoCanteiro) +
        deslocamentoHorizontal;
    posicaoCalculada.coordenadaVertical =
        (colunaGrade + linhaGrade) * calcularMetadeDaDimensaoIsometrica(alturaDoCanteiro) +
        deslocamentoVertical;

    return posicaoCalculada;
}

inline PosicaoNaGradeDecimal converterTelaParaGradeDecimal(
    int posicaoMouseHorizontal,
    int posicaoMouseVertical,
    int larguraDoCanteiro,
    int alturaDoCanteiro,
    int deslocamentoHorizontal,
    int deslocamentoVertical
) {
    const double metadeDaLargura = static_cast<double>(calcularMetadeDaDimensaoIsometrica(larguraDoCanteiro));
    const double metadeDaAltura = static_cast<double>(calcularMetadeDaDimensaoIsometrica(alturaDoCanteiro));
    const double posicaoHorizontalAjustada =
        posicaoMouseHorizontal - deslocamentoHorizontal - metadeDaLargura;
    const double posicaoVerticalAjustada = posicaoMouseVertical - deslocamentoVertical;

    return PosicaoNaGradeDecimal{
        static_cast<float>(
            (posicaoHorizontalAjustada / metadeDaLargura + posicaoVerticalAjustada / metadeDaAltura) / 2.0
        ),
        static_cast<float>(
            (posicaoVerticalAjustada / metadeDaAltura - posicaoHorizontalAjustada / metadeDaLargura) / 2.0
        )
    };
}

inline PosicaoNaGrade converterTelaParaGrade(
    int posicaoMouseHorizontal,
    int posicaoMouseVertical,
    int larguraDoCanteiro,
    int alturaDoCanteiro,
    int deslocamentoHorizontal,
    int deslocamentoVertical
) {
    const PosicaoNaGradeDecimal gradeDecimal = converterTelaParaGradeDecimal(
        posicaoMouseHorizontal,
        posicaoMouseVertical,
        larguraDoCanteiro,
        alturaDoCanteiro,
        deslocamentoHorizontal,
        deslocamentoVertical
    );

    PosicaoNaGrade gradeCalculada;
    gradeCalculada.indiceColuna = static_cast<int>(std::floor(gradeDecimal.indiceColuna));
    gradeCalculada.indiceLinha = static_cast<int>(std::floor(gradeDecimal.indiceLinha));

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

inline PosicaoDecimalNaGradeDeOcupacao converterTelaParaOcupacaoDecimal(
    int posicaoMouseHorizontal,
    int posicaoMouseVertical,
    int larguraDaUnidadeDeOcupacao,
    int alturaDaUnidadeDeOcupacao,
    int deslocamentoHorizontal,
    int deslocamentoVertical
) {
    const PosicaoNaGradeDecimal posicaoLocal = converterTelaParaGradeDecimal(
        posicaoMouseHorizontal,
        posicaoMouseVertical,
        larguraDaUnidadeDeOcupacao,
        alturaDaUnidadeDeOcupacao,
        deslocamentoHorizontal,
        deslocamentoVertical
    );

    return PosicaoDecimalNaGradeDeOcupacao{posicaoLocal.indiceColuna, posicaoLocal.indiceLinha};
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

inline PosicaoNaTela converterCentroDaUnidadeDeOcupacaoGlobalParaTela(
    PosicaoNaGradeDeOcupacao posicaoGlobal,
    int larguraDaUnidadeDeOcupacao,
    int alturaDaUnidadeDeOcupacao,
    int origemHorizontal,
    int origemVertical,
    int cameraOffsetHorizontal,
    int cameraOffsetVertical
) {
    const PosicaoNaTela origemDaUnidade = converterOcupacaoGlobalParaTela(
        posicaoGlobal,
        larguraDaUnidadeDeOcupacao,
        alturaDaUnidadeDeOcupacao,
        origemHorizontal,
        origemVertical,
        cameraOffsetHorizontal,
        cameraOffsetVertical
    );

    return PosicaoNaTela{
        origemDaUnidade.coordenadaHorizontal + calcularMetadeDaDimensaoIsometrica(larguraDaUnidadeDeOcupacao),
        origemDaUnidade.coordenadaVertical + calcularMetadeDaDimensaoIsometrica(alturaDaUnidadeDeOcupacao)
    };
}

inline PosicaoNaTela converterCentroDaUnidadeDeOcupacaoGlobalParaTela(
    PosicaoDecimalNaGradeDeOcupacao posicaoGlobal,
    int larguraDaUnidadeDeOcupacao,
    int alturaDaUnidadeDeOcupacao,
    int origemHorizontal,
    int origemVertical,
    int cameraOffsetHorizontal,
    int cameraOffsetVertical
) {
    const PosicaoNaGradeDeOcupacao posicaoBase{
        static_cast<int>(std::floor(posicaoGlobal.indiceColuna)),
        static_cast<int>(std::floor(posicaoGlobal.indiceLinha))
    };
    PosicaoNaTela pontoCentral = converterCentroDaUnidadeDeOcupacaoGlobalParaTela(
        posicaoBase,
        larguraDaUnidadeDeOcupacao,
        alturaDaUnidadeDeOcupacao,
        origemHorizontal,
        origemVertical,
        cameraOffsetHorizontal,
        cameraOffsetVertical
    );
    const float deslocamentoColuna = posicaoGlobal.indiceColuna - posicaoBase.indiceColuna;
    const float deslocamentoLinha = posicaoGlobal.indiceLinha - posicaoBase.indiceLinha;

    pontoCentral.coordenadaHorizontal += static_cast<int>(std::lround(
        (deslocamentoColuna - deslocamentoLinha) *
            calcularMetadeDaDimensaoIsometrica(larguraDaUnidadeDeOcupacao)
    ));
    pontoCentral.coordenadaVertical += static_cast<int>(std::lround(
        (deslocamentoColuna + deslocamentoLinha) *
            calcularMetadeDaDimensaoIsometrica(alturaDaUnidadeDeOcupacao)
    ));

    return pontoCentral;
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
