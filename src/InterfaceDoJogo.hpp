#pragma once

#include "Constantes.hpp"
#include "Tipos.hpp"

struct BotoesDaInterface {
    AreaDeInteracao cursor;
    AreaDeInteracao enxada;
    AreaDeInteracao removerTerra;
    AreaDeInteracao semente;
    AreaDeInteracao presente;
};

inline BotoesDaInterface criarBotoesDaInterface() {
    const int quantidadeDeBotoesInferiores = 5;
    const int y = Constantes::ALTURA_DA_JANELA - 68;
    const int largura = Constantes::TAMANHO_DO_BOTAO_DA_INTERFACE;
    const int espacamento = Constantes::ESPACAMENTO_DOS_BOTOES;
    const int larguraTotal = largura * quantidadeDeBotoesInferiores + espacamento * (quantidadeDeBotoesInferiores - 1);
    const int primeiroX = Constantes::LARGURA_DA_JANELA / 2 - larguraTotal / 2;

    return BotoesDaInterface{
        AreaDeInteracao{primeiroX, y, largura, largura},
        AreaDeInteracao{primeiroX + (largura + espacamento), y, largura, largura},
        AreaDeInteracao{primeiroX + (largura + espacamento) * 2, y, largura, largura},
        AreaDeInteracao{primeiroX + (largura + espacamento) * 3, y, largura, largura},
        AreaDeInteracao{primeiroX + (largura + espacamento) * 4, y, largura, largura}
    };
}

inline bool verificarCliqueNoBotao(int cliqueMouseHorizontal, int cliqueMouseVertical, AreaDeInteracao limitesDoBotao) {
    const bool colidiuHorizontalmente =
        cliqueMouseHorizontal >= limitesDoBotao.posicaoBotaoHorizontal &&
        cliqueMouseHorizontal <= (limitesDoBotao.posicaoBotaoHorizontal + limitesDoBotao.tamanhoBotaoLargura);

    const bool colidiuVerticalmente =
        cliqueMouseVertical >= limitesDoBotao.posicaoBotaoVertical &&
        cliqueMouseVertical <= (limitesDoBotao.posicaoBotaoVertical + limitesDoBotao.tamanhoBotaoAltura);

    return colidiuHorizontalmente && colidiuVerticalmente;
}

inline bool processarCliqueNaInterface(
    int mouseX,
    int mouseY,
    const BotoesDaInterface& botoes,
    FerramentaSelecionada& ferramentaSelecionada
) {
    if (verificarCliqueNoBotao(mouseX, mouseY, botoes.cursor)) {
        ferramentaSelecionada = FERRAMENTA_CURSOR;
        return true;
    }

    if (verificarCliqueNoBotao(mouseX, mouseY, botoes.enxada)) {
        ferramentaSelecionada = FERRAMENTA_ENXADA;
        return true;
    }

    if (verificarCliqueNoBotao(mouseX, mouseY, botoes.removerTerra)) {
        ferramentaSelecionada = FERRAMENTA_REMOVER_TERRA;
        return true;
    }

    if (verificarCliqueNoBotao(mouseX, mouseY, botoes.semente)) {
        ferramentaSelecionada = FERRAMENTA_SEMENTE;
        return true;
    }

    if (verificarCliqueNoBotao(mouseX, mouseY, botoes.presente)) {
        ferramentaSelecionada = FERRAMENTA_PRESENTE;
        return true;
    }

    return false;
}
