#pragma once

#include "CameraDoJogo.hpp"
#include "Constantes.hpp"
#include "GradeDeCanteiros.hpp"
#include "Tipos.hpp"

#include <cstddef>

enum AcaoDaFerramenta {
    ACAO_NENHUMA,
    ACAO_CRIAR_TERRA,
    ACAO_ARAR_TERRA,
    ACAO_REMOVER_TERRA,
    ACAO_PLANTAR,
    ACAO_ACELERAR_CRESCIMENTO,
    ACAO_COLHER
};

struct ResultadoDaFerramenta {
    AcaoDaFerramenta acao = ACAO_NENHUMA;

    bool houveMudanca() const {
        return acao != ACAO_NENHUMA;
    }
};

struct EstadoDoJogo {
    GradeGlobalDeCanteiros grade;
    EstadoDaCamera camera;
    FerramentaSelecionada ferramentaSelecionada = FERRAMENTA_CURSOR;
    int tamanhoAtualDoGrid = Constantes::TAMANHO_INICIAL_GRID;
    int moedas = 50;
    int experiencia = 0;
    int nivel = 1;
    float acumuladorDeSegundos = 0.0f;
};

inline GradeGlobalDeCanteiros criarGradeGlobalComNucleoInicial() {
    GradeGlobalDeCanteiros grade;

    for (int linha = 0; linha < Constantes::QUANTIDADE_DE_LINHAS_DO_NUCLEO_INICIAL; ++linha) {
        for (int coluna = 0; coluna < Constantes::QUANTIDADE_DE_COLUNAS_DO_NUCLEO_INICIAL; ++coluna) {
            const PosicaoNaGrade posicao{
                Constantes::COLUNA_INICIAL_DO_NUCLEO_INICIAL + coluna,
                Constantes::LINHA_INICIAL_DO_NUCLEO_INICIAL + linha
            };

            Grade::ativarTile(grade, posicao);
        }
    }

    return grade;
}

inline EstadoDoJogo criarEstadoInicialDoJogo() {
    EstadoDoJogo jogo;
    jogo.grade = criarGradeGlobalComNucleoInicial();
    jogo.tamanhoAtualDoGrid = Constantes::TAMANHO_INICIAL_GRID;
    return jogo;
}

inline void recalcularNivel(EstadoDoJogo& jogo) {
    jogo.nivel = 1 + jogo.experiencia / 50;
}

inline ResultadoDaFerramenta aplicarFerramentaNoJogo(EstadoDoJogo& jogo, PosicaoNaGrade posicao) {
    if (!Grade::posicaoEstaDentroDaGradeGlobal(posicao)) {
        return ResultadoDaFerramenta{};
    }

    if (!Grade::posicaoEstaDentroDaGradeAtual(posicao, jogo.tamanhoAtualDoGrid)) {
        return ResultadoDaFerramenta{};
    }

    TileDeTerra* tile = Grade::obterTile(jogo.grade, posicao);
    if (tile == nullptr) {
        return ResultadoDaFerramenta{};
    }

    if (jogo.ferramentaSelecionada == FERRAMENTA_ENXADA && !tile->existeNoMapa) {
        Grade::ativarTile(jogo.grade, posicao);
        return ResultadoDaFerramenta{ACAO_CRIAR_TERRA};
    }

    if (jogo.ferramentaSelecionada == FERRAMENTA_REMOVER_TERRA) {
        if (!tile->existeNoMapa) {
            return ResultadoDaFerramenta{};
        }

        Grade::removerTile(jogo.grade, posicao);
        return ResultadoDaFerramenta{ACAO_REMOVER_TERRA};
    }

    if (!tile->existeNoMapa) {
        return ResultadoDaFerramenta{};
    }

    DadosDoCanteiro& canteiro = tile->canteiro;
    AcaoDaFerramenta acao = ACAO_NENHUMA;

    switch (jogo.ferramentaSelecionada) {
        case FERRAMENTA_ENXADA:
            if (canteiro.estadoVisualAtual == ESTADO_TERRA_VAZIA ||
                canteiro.estadoVisualAtual == ESTADO_PLANTA_MORTA) {
                canteiro.estadoVisualAtual = ESTADO_TERRA_ARADA;
                canteiro.tempoDePlantioEmSegundos = 0;
                canteiro.identificadorDaSemente = -1;
                acao = ACAO_ARAR_TERRA;
            }
            break;

        case FERRAMENTA_SEMENTE:
            if (canteiro.estadoVisualAtual == ESTADO_TERRA_ARADA && jogo.moedas >= 2) {
                jogo.moedas -= 2;
                canteiro.estadoVisualAtual = ESTADO_SEMENTE_PLANTADA;
                canteiro.tempoDePlantioEmSegundos = 0;
                canteiro.identificadorDaSemente = 1;
                acao = ACAO_PLANTAR;
            }
            break;

        case FERRAMENTA_PRESENTE:
            if (canteiro.estadoVisualAtual == ESTADO_SEMENTE_PLANTADA ||
                canteiro.estadoVisualAtual == ESTADO_PLANTA_CRESCENDO) {
                canteiro.estadoVisualAtual = ESTADO_PLANTA_MADURA;
                canteiro.tempoDePlantioEmSegundos = Constantes::TEMPO_PARA_MADURAR;
                acao = ACAO_ACELERAR_CRESCIMENTO;
            }
            break;

        case FERRAMENTA_CURSOR:
            if (canteiro.estadoVisualAtual == ESTADO_PLANTA_MADURA) {
                jogo.moedas += 8;
                jogo.experiencia += 5;
                canteiro.estadoVisualAtual = ESTADO_TERRA_VAZIA;
                canteiro.tempoDePlantioEmSegundos = 0;
                canteiro.identificadorDaSemente = -1;
                recalcularNivel(jogo);
                acao = ACAO_COLHER;
            }
            break;

        case FERRAMENTA_REMOVER_TERRA:
        default:
            break;
    }

    if (acao != ACAO_NENHUMA) {
        Grade::sincronizarCrescimentoDoCanteiro(jogo.grade, posicao);
    }

    return ResultadoDaFerramenta{acao};
}

inline void avancarCrescimentoDosCanteiros(GradeGlobalDeCanteiros& grade, int tamanhoAtualDoGrid) {
    std::size_t indice = 0;

    while (indice < grade.posicoesDeCanteirosEmCrescimento.size()) {
        const PosicaoNaGrade posicao = grade.posicoesDeCanteirosEmCrescimento[indice];
        TileDeTerra* tile = Grade::obterTile(grade, posicao);

        if (!Grade::posicaoEstaDentroDaGradeAtual(posicao, tamanhoAtualDoGrid) ||
            tile == nullptr ||
            !tile->existeNoMapa ||
            !Grade::estadoPrecisaAvancarCrescimento(tile->canteiro.estadoVisualAtual)) {
            if (tile == nullptr || !tile->existeNoMapa || !Grade::estadoPrecisaAvancarCrescimento(tile->canteiro.estadoVisualAtual)) {
                Grade::removerCanteiroDaListaDeCrescimento(grade, posicao);
            } else {
                ++indice;
            }
            continue;
        }

        DadosDoCanteiro& canteiro = tile->canteiro;
        ++canteiro.tempoDePlantioEmSegundos;

        if (canteiro.tempoDePlantioEmSegundos >= Constantes::TEMPO_PARA_MORRER) {
            canteiro.estadoVisualAtual = ESTADO_PLANTA_MORTA;
            Grade::removerCanteiroDaListaDeCrescimento(grade, posicao);
            continue;
        }

        if (canteiro.tempoDePlantioEmSegundos >= Constantes::TEMPO_PARA_MADURAR) {
            canteiro.estadoVisualAtual = ESTADO_PLANTA_MADURA;
        } else if (canteiro.tempoDePlantioEmSegundos >= Constantes::TEMPO_PARA_CRESCER) {
            canteiro.estadoVisualAtual = ESTADO_PLANTA_CRESCENDO;
        }

        ++indice;
    }
}

inline void avancarTempoDoJogo(EstadoDoJogo& jogo, float deltaTime) {
    jogo.acumuladorDeSegundos += deltaTime;

    while (jogo.acumuladorDeSegundos >= 1.0f) {
        avancarCrescimentoDosCanteiros(jogo.grade, jogo.tamanhoAtualDoGrid);
        jogo.acumuladorDeSegundos -= 1.0f;
    }
}
