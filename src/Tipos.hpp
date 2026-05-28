#pragma once

#include "Constantes.hpp"

#include <array>

enum EstadoDoCanteiro {
    ESTADO_TERRA_VAZIA,
    ESTADO_TERRA_ARADA,
    ESTADO_SEMENTE_PLANTADA,
    ESTADO_PLANTA_CRESCENDO,
    ESTADO_PLANTA_MADURA,
    ESTADO_PLANTA_MORTA
};

enum FerramentaSelecionada {
    FERRAMENTA_CURSOR,
    FERRAMENTA_ENXADA,
    FERRAMENTA_SEMENTE,
    FERRAMENTA_PRESENTE
};

struct PosicaoNaTela {
    int coordenadaHorizontal = 0;
    int coordenadaVertical = 0;
};

struct PosicaoNaGrade {
    int indiceColuna = -1;
    int indiceLinha = -1;
};

struct DadosDoCanteiro {
    EstadoDoCanteiro estadoVisualAtual = ESTADO_TERRA_VAZIA;
    int tempoDePlantioEmSegundos = 0;
    int identificadorDaSemente = -1;
};

struct ConfiguracoesDoLayout {
    int deslocamentoGradeHorizontal = 520;
    int deslocamentoGradeVertical = 110;

    int posicaoCasaHorizontal = 855;
    int posicaoCasaVertical = 170;
    int tamanhoLarguraCasa = 230;
    int tamanhoAlturaCasa = 210;

    int posicaoCasinhaHorizontal = 245;
    int posicaoCasinhaVertical = 420;
    int tamanhoLarguraCasinha = 130;
    int tamanhoAlturaCasinha = 110;
};

struct AreaDeInteracao {
    int posicaoBotaoHorizontal = 0;
    int posicaoBotaoVertical = 0;
    int tamanhoBotaoLargura = 0;
    int tamanhoBotaoAltura = 0;
};

using LinhaDeCanteiros = std::array<DadosDoCanteiro, Constantes::QUANTIDADE_DE_COLUNAS_DA_GRADE>;
using MatrizDeCanteiros = std::array<LinhaDeCanteiros, Constantes::QUANTIDADE_DE_LINHAS_DA_GRADE>;

inline bool verificarCliqueNoBotao(int cliqueMouseHorizontal, int cliqueMouseVertical, AreaDeInteracao limitesDoBotao) {
    const bool colidiuHorizontalmente =
        cliqueMouseHorizontal >= limitesDoBotao.posicaoBotaoHorizontal &&
        cliqueMouseHorizontal <= (limitesDoBotao.posicaoBotaoHorizontal + limitesDoBotao.tamanhoBotaoLargura);

    const bool colidiuVerticalmente =
        cliqueMouseVertical >= limitesDoBotao.posicaoBotaoVertical &&
        cliqueMouseVertical <= (limitesDoBotao.posicaoBotaoVertical + limitesDoBotao.tamanhoBotaoAltura);

    return colidiuHorizontalmente && colidiuVerticalmente;
}

