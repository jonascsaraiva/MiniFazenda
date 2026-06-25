#pragma once

#include "Constantes.hpp"

#include <cstddef>
#include <string>

enum EstadoDoCanteiro {
    ESTADO_TERRA_VAZIA,
    ESTADO_TERRA_ARADA,
    ESTADO_SEMENTE_PLANTADA,
    ESTADO_PLANTA_CRESCENDO,
    ESTADO_PLANTA_MADURA,
    ESTADO_PLANTA_MORTA
};

constexpr std::size_t QUANTIDADE_DE_ESTADOS_DO_CANTEIRO = 6;

enum FerramentaSelecionada {
    FERRAMENTA_CURSOR,
    FERRAMENTA_ENXADA,
    FERRAMENTA_REMOVER_TERRA,
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
    int centroVisualBackgroundX = Constantes::CENTRO_VISUAL_BACKGROUND_X;
    int centroVisualBackgroundY = Constantes::CENTRO_VISUAL_BACKGROUND_Y;
    int origemGradeHorizontal = Constantes::CENTRO_VISUAL_BACKGROUND_X;
    int origemGradeVertical = Constantes::CENTRO_VISUAL_BACKGROUND_Y;

    std::string arquivoBackgroundPrincipal = "background.png";
};

struct AreaDeInteracao {
    int posicaoBotaoHorizontal = 0;
    int posicaoBotaoVertical = 0;
    int tamanhoBotaoLargura = 0;
    int tamanhoBotaoAltura = 0;
};

inline bool posicoesDaGradeSaoIguais(PosicaoNaGrade primeira, PosicaoNaGrade segunda) {
    return primeira.indiceColuna == segunda.indiceColuna &&
           primeira.indiceLinha == segunda.indiceLinha;
}
