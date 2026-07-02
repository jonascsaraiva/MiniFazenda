#pragma once

#include "Apresentacao/Interface/AreaDeInteracao.hpp"
#include "Apresentacao/Interface/EstadoDaCenaFazenda.hpp"
#include "Compartilhado/ConstantesDaInterface.hpp"
#include "Compartilhado/ConstantesDaJanela.hpp"
#include "Dominio/Ferramentas/TipoDeFerramenta.hpp"
#include "Dominio/Plantas/Planta.hpp"

#include <algorithm>
#include <memory>
#include <optional>
#include <vector>

namespace MiniFazenda::Apresentacao::Interface::BarraDeFerramentas {

struct BotoesDaInterface {
    AreaDeInteracao cursor;
    AreaDeInteracao enxada;
    AreaDeInteracao removerTerra;
    AreaDeInteracao semente;
    AreaDeInteracao loja;
};

struct OpcaoDeSementeDaLoja {
    AreaDeInteracao area;
    int identificadorDaSemente = -1;
};

struct PainelDaLoja {
    AreaDeInteracao fundo;
    std::vector<OpcaoDeSementeDaLoja> opcoes;
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

inline PainelDaLoja criarPainelDaLoja(
    const BotoesDaInterface& botoes,
    const std::vector<std::unique_ptr<Dominio::Plantas::Planta>>& especiesDisponiveis
) {
    PainelDaLoja painel;

    int quantidadeDeEspecies = 0;
    for (const std::unique_ptr<Dominio::Plantas::Planta>& especie : especiesDisponiveis) {
        if (especie != nullptr) {
            ++quantidadeDeEspecies;
        }
    }

    if (quantidadeDeEspecies == 0) {
        return painel;
    }

    constexpr int tamanhoDaOpcao = 48;
    constexpr int preenchimento = 8;
    constexpr int espacamento = 8;
    const int larguraDoPainel =
        preenchimento * 2 + tamanhoDaOpcao * quantidadeDeEspecies + espacamento * (quantidadeDeEspecies - 1);
    const int alturaDoPainel = preenchimento * 2 + tamanhoDaOpcao;
    const int xCentralizadoNaLoja =
        botoes.loja.posicaoBotaoHorizontal + botoes.loja.tamanhoBotaoLargura / 2 - larguraDoPainel / 2;
    const int limiteMaximoX = std::max(
        preenchimento,
        Compartilhado::Constantes::LARGURA_DA_JANELA - larguraDoPainel - preenchimento
    );
    const int x = std::clamp(
        xCentralizadoNaLoja,
        preenchimento,
        limiteMaximoX
    );
    const int y = botoes.loja.posicaoBotaoVertical - alturaDoPainel - preenchimento;

    painel.fundo = AreaDeInteracao{x, y, larguraDoPainel, alturaDoPainel};

    int xDaOpcao = x + preenchimento;
    for (const std::unique_ptr<Dominio::Plantas::Planta>& especie : especiesDisponiveis) {
        if (especie == nullptr) {
            continue;
        }

        painel.opcoes.push_back(OpcaoDeSementeDaLoja{
            AreaDeInteracao{xDaOpcao, y + preenchimento, tamanhoDaOpcao, tamanhoDaOpcao},
            especie->identificadorDaSemente()
        });
        xDaOpcao += tamanhoDaOpcao + espacamento;
    }

    return painel;
}

inline bool verificarCliqueNoBotao(
    int cliqueMouseHorizontal,
    int cliqueMouseVertical,
    const AreaDeInteracao& limitesDoBotao
) {
    return pontoEstaNaAreaDeInteracao(cliqueMouseHorizontal, cliqueMouseVertical, limitesDoBotao);
}

inline std::optional<int> sementeClicadaNoPainelDaLoja(
    int mouseX,
    int mouseY,
    const PainelDaLoja& painel
) {
    for (const OpcaoDeSementeDaLoja& opcao : painel.opcoes) {
        if (verificarCliqueNoBotao(mouseX, mouseY, opcao.area)) {
            return opcao.identificadorDaSemente;
        }
    }

    return std::nullopt;
}

inline bool processarCliqueNaInterface(
    int mouseX,
    int mouseY,
    const BotoesDaInterface& botoes,
    Dominio::Ferramentas::TipoDeFerramenta& ferramentaSelecionada,
    EstadoDaCenaFazenda& estadoDaCena
) {
    if (verificarCliqueNoBotao(mouseX, mouseY, botoes.cursor)) {
        ferramentaSelecionada = Dominio::Ferramentas::TipoDeFerramenta::Cursor;
        estadoDaCena.fecharPainelDaLoja();
        return true;
    }

    if (verificarCliqueNoBotao(mouseX, mouseY, botoes.enxada)) {
        ferramentaSelecionada = Dominio::Ferramentas::TipoDeFerramenta::Enxada;
        estadoDaCena.fecharPainelDaLoja();
        return true;
    }

    if (verificarCliqueNoBotao(mouseX, mouseY, botoes.removerTerra)) {
        ferramentaSelecionada = Dominio::Ferramentas::TipoDeFerramenta::RemoverTerra;
        estadoDaCena.fecharPainelDaLoja();
        return true;
    }

    if (verificarCliqueNoBotao(mouseX, mouseY, botoes.semente)) {
        ferramentaSelecionada = Dominio::Ferramentas::TipoDeFerramenta::Semente;
        estadoDaCena.fecharPainelDaLoja();
        return true;
    }

    if (verificarCliqueNoBotao(mouseX, mouseY, botoes.loja)) {
        ferramentaSelecionada = Dominio::Ferramentas::TipoDeFerramenta::Loja;
        estadoDaCena.alternarPainelDaLoja();
        return true;
    }

    return false;
}

} // namespace MiniFazenda::Apresentacao::Interface::BarraDeFerramentas
