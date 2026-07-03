#pragma once

namespace MiniFazenda::Compartilhado::Geometria {

struct PosicaoNaTela {
    int coordenadaHorizontal = 0;
    int coordenadaVertical = 0;
};

struct PosicaoNaGrade {
    int indiceColuna = -1;
    int indiceLinha = -1;
};

struct PosicaoDeCanteiroNoMapa {
    int indiceColuna = -1;
    int indiceLinha = -1;
};

struct PosicaoNaGradeDecimal {
    float indiceColuna = -1.0f;
    float indiceLinha = -1.0f;
};

struct PosicaoNaGradeDeOcupacao {
    int indiceColuna = -1;
    int indiceLinha = -1;
};

struct PosicaoDecimalNaGradeDeOcupacao {
    float indiceColuna = -1.0f;
    float indiceLinha = -1.0f;
};

struct AreaNaGradeDeOcupacao {
    int indiceColuna = -1;
    int indiceLinha = -1;
    int largura = 0;
    int altura = 0;
};

struct Retangulo {
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
};

inline bool posicoesDaGradeSaoIguais(PosicaoNaGrade primeira, PosicaoNaGrade segunda) {
    return primeira.indiceColuna == segunda.indiceColuna &&
           primeira.indiceLinha == segunda.indiceLinha;
}

inline bool posicoesDeCanteiroNoMapaSaoIguais(
    PosicaoDeCanteiroNoMapa primeira,
    PosicaoDeCanteiroNoMapa segunda
) {
    return primeira.indiceColuna == segunda.indiceColuna &&
           primeira.indiceLinha == segunda.indiceLinha;
}

inline PosicaoDeCanteiroNoMapa converterPosicaoNaGradeParaCanteiroNoMapa(PosicaoNaGrade posicao) {
    return PosicaoDeCanteiroNoMapa{posicao.indiceColuna, posicao.indiceLinha};
}

inline PosicaoNaGrade converterCanteiroNoMapaParaPosicaoNaGrade(PosicaoDeCanteiroNoMapa posicao) {
    return PosicaoNaGrade{posicao.indiceColuna, posicao.indiceLinha};
}

inline bool posicoesDaGradeDeOcupacaoSaoIguais(
    PosicaoNaGradeDeOcupacao primeira,
    PosicaoNaGradeDeOcupacao segunda
) {
    return primeira.indiceColuna == segunda.indiceColuna &&
           primeira.indiceLinha == segunda.indiceLinha;
}

inline bool areasDaGradeDeOcupacaoSaoIguais(
    AreaNaGradeDeOcupacao primeira,
    AreaNaGradeDeOcupacao segunda
) {
    return primeira.indiceColuna == segunda.indiceColuna &&
           primeira.indiceLinha == segunda.indiceLinha &&
           primeira.largura == segunda.largura &&
           primeira.altura == segunda.altura;
}

} // namespace MiniFazenda::Compartilhado::Geometria
