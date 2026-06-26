#pragma once

#include "Apresentacao/Interface/AreaDeInteracao.hpp"
#include "Compartilhado/Constantes.hpp"
#include "Dominio/Ferramentas/TipoDeFerramenta.hpp"

namespace MiniFazenda::Apresentacao::Interface::BarraDeFerramentas {

struct BotoesDaInterface {
    AreaDeInteracao cursor;
    AreaDeInteracao enxada;
    AreaDeInteracao removerTerra;
    AreaDeInteracao semente;
    AreaDeInteracao presente;
};

inline BotoesDaInterface criarBotoesDaInterface() {
    const int quantidadeDeBotoesInferiores = 5;
    const int y = Compartilhado::Constantes::ALTURA_DA_JANELA - 68;
    const int largura = Compartilhado::Constantes::TAMANHO_DO_BOTAO_DA_INTERFACE;
    const int espacamento = Compartilhado::Constantes::ESPACAMENTO_DOS_BOTOES;
    const int larguraTotal = largura * quantidadeDeBotoesInferiores + espacamento * (quantidadeDeBotoesInferiores - 1);
    const int primeiroX = Compartilhado::Constantes::LARGURA_DA_JANELA / 2 - larguraTotal / 2;

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
    Dominio::Ferramentas::TipoDeFerramenta& ferramentaSelecionada
) {
    if (verificarCliqueNoBotao(mouseX, mouseY, botoes.cursor)) {
        ferramentaSelecionada = Dominio::Ferramentas::TipoDeFerramenta::Cursor;
        return true;
    }

    if (verificarCliqueNoBotao(mouseX, mouseY, botoes.enxada)) {
        ferramentaSelecionada = Dominio::Ferramentas::TipoDeFerramenta::Enxada;
        return true;
    }

    if (verificarCliqueNoBotao(mouseX, mouseY, botoes.removerTerra)) {
        ferramentaSelecionada = Dominio::Ferramentas::TipoDeFerramenta::RemoverTerra;
        return true;
    }

    if (verificarCliqueNoBotao(mouseX, mouseY, botoes.semente)) {
        ferramentaSelecionada = Dominio::Ferramentas::TipoDeFerramenta::Semente;
        return true;
    }

    if (verificarCliqueNoBotao(mouseX, mouseY, botoes.presente)) {
        ferramentaSelecionada = Dominio::Ferramentas::TipoDeFerramenta::Presente;
        return true;
    }

    return false;
}

} // namespace MiniFazenda::Apresentacao::Interface::BarraDeFerramentas
