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

inline bool posicoesDaGradeSaoIguais(PosicaoNaGrade primeira, PosicaoNaGrade segunda) {
    return primeira.indiceColuna == segunda.indiceColuna &&
           primeira.indiceLinha == segunda.indiceLinha;
}

} // namespace MiniFazenda::Compartilhado::Geometria
