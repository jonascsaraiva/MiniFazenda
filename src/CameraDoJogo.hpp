#pragma once

#include "Constantes.hpp"
#include "GradeDeCanteiros.hpp"
#include "Tipos.hpp"

#include <algorithm>
#include <cmath>

struct DimensoesDoCanteiroRenderizado {
    int largura = Constantes::LARGURA_DO_CANTEIRO;
    int altura = Constantes::ALTURA_DO_CANTEIRO;
};

struct EstadoDaCamera {
    int offsetHorizontal = 0;
    int offsetVertical = 0;
    float zoomAtual = Constantes::ZOOM_INICIAL;
    bool panAtivo = false;
    int botaoDoPan = 0;
    unsigned int ultimoMovimentoMs = 0;
    float velocidadeHorizontal = 0.0f;
    float velocidadeVertical = 0.0f;
};

struct RetanguloDeGradeRenderizada {
    int x = 0;
    int y = 0;
    int largura = 0;
    int altura = 0;
};

inline DimensoesDoCanteiroRenderizado calcularDimensoesDoCanteiroRenderizado(float zoomAtual) {
    const float zoomLimitado = std::clamp(zoomAtual, Constantes::ZOOM_MINIMO, Constantes::ZOOM_MAXIMO);
    return DimensoesDoCanteiroRenderizado{
        std::max(1, static_cast<int>(std::round(Constantes::LARGURA_DO_CANTEIRO * zoomLimitado))),
        std::max(1, static_cast<int>(std::round(Constantes::ALTURA_DO_CANTEIRO * zoomLimitado)))
    };
}

inline PosicaoNaTela calcularDeslocamentoCentradoPara(
    int tamanhoGrid,
    int centroVisualX,
    int centroVisualY,
    int larguraDoCanteiro = Constantes::LARGURA_DO_CANTEIRO,
    int alturaDoCanteiro = Constantes::ALTURA_DO_CANTEIRO
) {
    const int tamanhoNormalizado = Grade::normalizarTamanhoDaGradeAtual(tamanhoGrid);
    const int colunaInicial = Grade::calcularColunaInicialDaGradeAtual(tamanhoNormalizado);
    const int linhaInicial = Grade::calcularLinhaInicialDaGradeAtual(tamanhoNormalizado);
    const int colunaLocal = colunaInicial - Constantes::COLUNA_DE_ORIGEM_VISUAL_DA_GRADE_GLOBAL;
    const int linhaLocal = linhaInicial - Constantes::LINHA_DE_ORIGEM_VISUAL_DA_GRADE_GLOBAL;

    const int xDesejadoDoPrimeiroTile = centroVisualX - larguraDoCanteiro / 2;
    const int yDesejadoDoPrimeiroTile = centroVisualY - (tamanhoNormalizado * alturaDoCanteiro) / 2;

    return PosicaoNaTela{
        xDesejadoDoPrimeiroTile - (colunaLocal - linhaLocal) * (larguraDoCanteiro / 2),
        yDesejadoDoPrimeiroTile - (colunaLocal + linhaLocal) * (alturaDoCanteiro / 2)
    };
}

inline void aplicarOrigemCentradaDaGrade(ConfiguracoesDoLayout& configuracoes, int tamanhoGrid) {
    const PosicaoNaTela origem = calcularDeslocamentoCentradoPara(
        tamanhoGrid,
        configuracoes.centroVisualBackgroundX,
        configuracoes.centroVisualBackgroundY
    );
    configuracoes.origemGradeHorizontal = origem.coordenadaHorizontal;
    configuracoes.origemGradeVertical = origem.coordenadaVertical;
}

inline RetanguloDeGradeRenderizada calcularRetanguloDaGradeRenderizada(
    const ConfiguracoesDoLayout& configuracoes,
    const EstadoDaCamera& camera,
    int tamanhoGrid
) {
    const int tamanhoNormalizado = Grade::normalizarTamanhoDaGradeAtual(tamanhoGrid);
    const DimensoesDoCanteiroRenderizado dimensoes = calcularDimensoesDoCanteiroRenderizado(camera.zoomAtual);
    const int colunaInicial = Grade::calcularColunaInicialDaGradeAtual(tamanhoNormalizado);
    const int linhaInicial = Grade::calcularLinhaInicialDaGradeAtual(tamanhoNormalizado);
    const int colunaLocal = colunaInicial - Constantes::COLUNA_DE_ORIGEM_VISUAL_DA_GRADE_GLOBAL;
    const int linhaLocal = linhaInicial - Constantes::LINHA_DE_ORIGEM_VISUAL_DA_GRADE_GLOBAL;

    const int primeiroTileX =
        (colunaLocal - linhaLocal) * (dimensoes.largura / 2) +
        configuracoes.origemGradeHorizontal +
        camera.offsetHorizontal;
    const int primeiroTileY =
        (colunaLocal + linhaLocal) * (dimensoes.altura / 2) +
        configuracoes.origemGradeVertical +
        camera.offsetVertical;

    return RetanguloDeGradeRenderizada{
        primeiroTileX - (tamanhoNormalizado - 1) * (dimensoes.largura / 2),
        primeiroTileY,
        tamanhoNormalizado * dimensoes.largura,
        tamanhoNormalizado * dimensoes.altura
    };
}

inline void limitarPanAosLimitesDoGrid(
    EstadoDaCamera& camera,
    const ConfiguracoesDoLayout& configuracoes,
    int tamanhoGrid
) {
    const RetanguloDeGradeRenderizada retangulo = calcularRetanguloDaGradeRenderizada(
        configuracoes,
        camera,
        tamanhoGrid
    );
    const int margemHorizontal = static_cast<int>(Constantes::LARGURA_DA_JANELA * Constantes::FRACAO_MINIMA_VISIVEL_AO_PAN);
    const int margemVertical = static_cast<int>(Constantes::ALTURA_DA_JANELA * Constantes::FRACAO_MINIMA_VISIVEL_AO_PAN);

    if (retangulo.x > Constantes::LARGURA_DA_JANELA - margemHorizontal) {
        camera.offsetHorizontal -= retangulo.x - (Constantes::LARGURA_DA_JANELA - margemHorizontal);
    }

    const int direita = retangulo.x + retangulo.largura;
    if (direita < margemHorizontal) {
        camera.offsetHorizontal += margemHorizontal - direita;
    }

    if (retangulo.y > Constantes::ALTURA_DA_JANELA - margemVertical) {
        camera.offsetVertical -= retangulo.y - (Constantes::ALTURA_DA_JANELA - margemVertical);
    }

    const int base = retangulo.y + retangulo.altura;
    if (base < margemVertical) {
        camera.offsetVertical += margemVertical - base;
    }
}

inline bool aplicarZoomNoPonto(
    EstadoDaCamera& camera,
    const ConfiguracoesDoLayout& configuracoes,
    int tamanhoGrid,
    int mouseX,
    int mouseY,
    int passosDoScroll
) {
    const float zoomAnterior = camera.zoomAtual;
    camera.zoomAtual = std::clamp(
        camera.zoomAtual + passosDoScroll * Constantes::PASSO_DO_ZOOM,
        Constantes::ZOOM_MINIMO,
        Constantes::ZOOM_MAXIMO
    );

    if (zoomAnterior == camera.zoomAtual) {
        return false;
    }

    const float fatorEscala = camera.zoomAtual / zoomAnterior;
    const float distanciaMouseDaOrigemX =
        static_cast<float>(mouseX - configuracoes.origemGradeHorizontal - camera.offsetHorizontal);
    const float distanciaMouseDaOrigemY =
        static_cast<float>(mouseY - configuracoes.origemGradeVertical - camera.offsetVertical);

    camera.offsetHorizontal += static_cast<int>(std::round(distanciaMouseDaOrigemX * (1.0f - fatorEscala)));
    camera.offsetVertical += static_cast<int>(std::round(distanciaMouseDaOrigemY * (1.0f - fatorEscala)));
    limitarPanAosLimitesDoGrid(camera, configuracoes, tamanhoGrid);
    return true;
}

inline void iniciarPanDaCamera(EstadoDaCamera& camera, int botao, unsigned int timestampMs) {
    camera.panAtivo = true;
    camera.botaoDoPan = botao;
    camera.ultimoMovimentoMs = timestampMs;
    camera.velocidadeHorizontal = 0.0f;
    camera.velocidadeVertical = 0.0f;
}

inline void moverPanDaCamera(
    EstadoDaCamera& camera,
    const ConfiguracoesDoLayout& configuracoes,
    int tamanhoGrid,
    int deltaX,
    int deltaY,
    unsigned int timestampMs
) {
    camera.offsetHorizontal += deltaX;
    camera.offsetVertical += deltaY;

    if (camera.ultimoMovimentoMs != 0u && timestampMs > camera.ultimoMovimentoMs) {
        const float tempoDesdeUltimoMovimento = (timestampMs - camera.ultimoMovimentoMs) / 1000.0f;
        if (tempoDesdeUltimoMovimento > 0.0f) {
            camera.velocidadeHorizontal = deltaX / tempoDesdeUltimoMovimento;
            camera.velocidadeVertical = deltaY / tempoDesdeUltimoMovimento;
        }
    }

    camera.ultimoMovimentoMs = timestampMs;
    limitarPanAosLimitesDoGrid(camera, configuracoes, tamanhoGrid);
}

inline void finalizarPanDaCamera(EstadoDaCamera& camera, int botao) {
    if (!camera.panAtivo || camera.botaoDoPan != botao) {
        return;
    }

    camera.panAtivo = false;
    camera.botaoDoPan = 0;
    camera.ultimoMovimentoMs = 0;
}

inline void atualizarInerciaDaCamera(
    EstadoDaCamera& camera,
    const ConfiguracoesDoLayout& configuracoes,
    int tamanhoGrid,
    float deltaTime
) {
    if (camera.panAtivo || (camera.velocidadeHorizontal == 0.0f && camera.velocidadeVertical == 0.0f)) {
        return;
    }

    camera.offsetHorizontal += static_cast<int>(camera.velocidadeHorizontal * deltaTime);
    camera.offsetVertical += static_cast<int>(camera.velocidadeVertical * deltaTime);

    if (camera.velocidadeHorizontal > 0.0f) {
        camera.velocidadeHorizontal = std::max(
            0.0f,
            camera.velocidadeHorizontal - Constantes::DESACELERACAO_DO_PAN * deltaTime
        );
    } else {
        camera.velocidadeHorizontal = std::min(
            0.0f,
            camera.velocidadeHorizontal + Constantes::DESACELERACAO_DO_PAN * deltaTime
        );
    }

    if (camera.velocidadeVertical > 0.0f) {
        camera.velocidadeVertical = std::max(
            0.0f,
            camera.velocidadeVertical - Constantes::DESACELERACAO_DO_PAN * deltaTime
        );
    } else {
        camera.velocidadeVertical = std::min(
            0.0f,
            camera.velocidadeVertical + Constantes::DESACELERACAO_DO_PAN * deltaTime
        );
    }

    limitarPanAosLimitesDoGrid(camera, configuracoes, tamanhoGrid);
}

inline void centralizarCamera(EstadoDaCamera& camera) {
    camera.offsetHorizontal = 0;
    camera.offsetVertical = 0;
    camera.zoomAtual = Constantes::ZOOM_INICIAL;
    camera.panAtivo = false;
    camera.botaoDoPan = 0;
    camera.ultimoMovimentoMs = 0;
    camera.velocidadeHorizontal = 0.0f;
    camera.velocidadeVertical = 0.0f;
}
