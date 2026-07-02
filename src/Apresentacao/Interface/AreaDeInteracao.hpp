#pragma once

namespace MiniFazenda::Apresentacao::Interface {

struct AreaDeInteracao {
    int posicaoBotaoHorizontal = 0;
    int posicaoBotaoVertical = 0;
    int tamanhoBotaoLargura = 0;
    int tamanhoBotaoAltura = 0;
};

// Areas clicaveis usam retangulos semiabertos: [x, x + largura) e [y, y + altura).
inline bool pontoEstaNoRetanguloSemiaberto(
    int pontoX,
    int pontoY,
    int retanguloX,
    int retanguloY,
    int retanguloLargura,
    int retanguloAltura
) {
    return pontoX >= retanguloX &&
           pontoX < retanguloX + retanguloLargura &&
           pontoY >= retanguloY &&
           pontoY < retanguloY + retanguloAltura;
}

inline bool pontoEstaNaAreaDeInteracao(int pontoX, int pontoY, const AreaDeInteracao& area) {
    return pontoEstaNoRetanguloSemiaberto(
        pontoX,
        pontoY,
        area.posicaoBotaoHorizontal,
        area.posicaoBotaoVertical,
        area.tamanhoBotaoLargura,
        area.tamanhoBotaoAltura
    );
}

} // namespace MiniFazenda::Apresentacao::Interface
