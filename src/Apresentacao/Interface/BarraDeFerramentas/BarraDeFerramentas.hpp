#pragma once

#include "Apresentacao/Interface/AreaDeInteracao.hpp"
#include "Compartilhado/ConstantesDaInterface.hpp"
#include "Compartilhado/ConstantesDaJanela.hpp"
#include "Dominio/Ferramentas/TipoDeFerramenta.hpp"

#include <algorithm>

namespace MiniFazenda::Apresentacao::Interface::BarraDeFerramentas {

struct BotoesDaInterface {
    AreaDeInteracao areaTotalDoHud;
    AreaDeInteracao ajuda;
    AreaDeInteracao zoomMais;
    AreaDeInteracao zoomMenos;
    AreaDeInteracao loja;
    AreaDeInteracao cursor;
    AreaDeInteracao enxada;
    AreaDeInteracao removerTerra;
    AreaDeInteracao estrela;
    AreaDeInteracao presente;
    AreaDeInteracao foto;
};

enum class AcaoDoCliqueNaInterface {
    Nenhuma,
    SelecionarFerramenta,
    AbrirLoja,
    AumentarZoom,
    DiminuirZoom,
    ConsumirClique
};

struct ResultadoDoCliqueNaInterface {
    bool cliqueConsumido = false;
    AcaoDoCliqueNaInterface acao = AcaoDoCliqueNaInterface::Nenhuma;
};

inline ResultadoDoCliqueNaInterface cliqueConsumidoComo(AcaoDoCliqueNaInterface acao) {
    ResultadoDoCliqueNaInterface resultado;
    resultado.cliqueConsumido = true;
    resultado.acao = acao;
    return resultado;
}

inline BotoesDaInterface criarBotoesDaInterface() {
    const int tamanhoBotaoPequeno = Compartilhado::Constantes::TAMANHO_DO_BOTAO_DA_INTERFACE;
    const int espacamento = Compartilhado::Constantes::ESPACAMENTO_DOS_BOTOES;
    const int larguraDaLoja = Compartilhado::Constantes::LARGURA_DO_BOTAO_DA_LOJA;
    const int alturaDaLoja = Compartilhado::Constantes::ALTURA_DO_BOTAO_DA_LOJA;
    const int larguraBotaoSuperior = Compartilhado::Constantes::LARGURA_DO_BOTAO_SUPERIOR_DO_HUD;
    const int alturaBotaoSuperior = Compartilhado::Constantes::ALTURA_DO_BOTAO_SUPERIOR_DO_HUD;

    const int larguraDaGradePequena = tamanhoBotaoPequeno * 3 + espacamento * 2;
    const int alturaDaGradePequena = tamanhoBotaoPequeno * 2 + espacamento;
    const int larguraDoBlocoPrincipal = larguraDaLoja + espacamento + larguraDaGradePequena;
    const int alturaDoBlocoPrincipal = std::max(alturaDaLoja, alturaDaGradePequena);
    const int blocoPrincipalX =
        Compartilhado::Constantes::LARGURA_DA_JANELA -
        Compartilhado::Constantes::MARGEM_DIREITA_DO_HUD_FERRAMENTAS -
        larguraDoBlocoPrincipal;
    const int blocoPrincipalY =
        Compartilhado::Constantes::ALTURA_DA_JANELA -
        Compartilhado::Constantes::MARGEM_INFERIOR_DO_HUD_FERRAMENTAS -
        alturaDoBlocoPrincipal;

    const int lojaY = blocoPrincipalY + (alturaDoBlocoPrincipal - alturaDaLoja) / 2;
    const int gradeX = blocoPrincipalX + larguraDaLoja + espacamento;
    const int gradeY = blocoPrincipalY + (alturaDoBlocoPrincipal - alturaDaGradePequena) / 2;
    const int linhaInferiorY = gradeY + tamanhoBotaoPequeno + espacamento;
    const int botoesSuperioresY = blocoPrincipalY - espacamento - alturaBotaoSuperior;
    const int recuoHorizontalBotaoSuperior = (tamanhoBotaoPequeno - larguraBotaoSuperior) / 2;

    const AreaDeInteracao ajuda{
        gradeX + recuoHorizontalBotaoSuperior,
        botoesSuperioresY,
        larguraBotaoSuperior,
        alturaBotaoSuperior
    };
    const AreaDeInteracao zoomMais{
        gradeX + tamanhoBotaoPequeno + espacamento + recuoHorizontalBotaoSuperior,
        botoesSuperioresY,
        larguraBotaoSuperior,
        alturaBotaoSuperior
    };
    const AreaDeInteracao zoomMenos{
        gradeX + (tamanhoBotaoPequeno + espacamento) * 2 + recuoHorizontalBotaoSuperior,
        botoesSuperioresY,
        larguraBotaoSuperior,
        alturaBotaoSuperior
    };

    return BotoesDaInterface{
        AreaDeInteracao{
            blocoPrincipalX,
            botoesSuperioresY,
            larguraDoBlocoPrincipal,
            blocoPrincipalY + alturaDoBlocoPrincipal - botoesSuperioresY
        },
        ajuda,
        zoomMais,
        zoomMenos,
        AreaDeInteracao{blocoPrincipalX, lojaY, larguraDaLoja, alturaDaLoja},
        AreaDeInteracao{gradeX, gradeY, tamanhoBotaoPequeno, tamanhoBotaoPequeno},
        AreaDeInteracao{gradeX + tamanhoBotaoPequeno + espacamento, gradeY, tamanhoBotaoPequeno, tamanhoBotaoPequeno},
        AreaDeInteracao{
            gradeX + (tamanhoBotaoPequeno + espacamento) * 2,
            gradeY,
            tamanhoBotaoPequeno,
            tamanhoBotaoPequeno
        },
        AreaDeInteracao{gradeX, linhaInferiorY, tamanhoBotaoPequeno, tamanhoBotaoPequeno},
        AreaDeInteracao{
            gradeX + tamanhoBotaoPequeno + espacamento,
            linhaInferiorY,
            tamanhoBotaoPequeno,
            tamanhoBotaoPequeno
        },
        AreaDeInteracao{
            gradeX + (tamanhoBotaoPequeno + espacamento) * 2,
            linhaInferiorY,
            tamanhoBotaoPequeno,
            tamanhoBotaoPequeno
        }
    };
}

inline bool verificarCliqueNoBotao(
    int cliqueMouseHorizontal,
    int cliqueMouseVertical,
    const AreaDeInteracao& limitesDoBotao
) {
    return pontoEstaNaAreaDeInteracao(cliqueMouseHorizontal, cliqueMouseVertical, limitesDoBotao);
}

inline bool pontoEstaNaAreaDoHud(int mouseX, int mouseY, const BotoesDaInterface& botoes) {
    return pontoEstaNaAreaDeInteracao(mouseX, mouseY, botoes.areaTotalDoHud);
}

inline ResultadoDoCliqueNaInterface processarCliqueNaInterface(
    int mouseX,
    int mouseY,
    const BotoesDaInterface& botoes,
    Dominio::Ferramentas::TipoDeFerramenta& ferramentaSelecionada
) {
    if (verificarCliqueNoBotao(mouseX, mouseY, botoes.cursor)) {
        ferramentaSelecionada = Dominio::Ferramentas::TipoDeFerramenta::Cursor;
        return cliqueConsumidoComo(AcaoDoCliqueNaInterface::SelecionarFerramenta);
    }

    if (verificarCliqueNoBotao(mouseX, mouseY, botoes.enxada)) {
        ferramentaSelecionada = Dominio::Ferramentas::TipoDeFerramenta::Enxada;
        return cliqueConsumidoComo(AcaoDoCliqueNaInterface::SelecionarFerramenta);
    }

    if (verificarCliqueNoBotao(mouseX, mouseY, botoes.removerTerra)) {
        ferramentaSelecionada = Dominio::Ferramentas::TipoDeFerramenta::RemoverTerra;
        return cliqueConsumidoComo(AcaoDoCliqueNaInterface::SelecionarFerramenta);
    }

    if (verificarCliqueNoBotao(mouseX, mouseY, botoes.loja)) {
        ferramentaSelecionada = Dominio::Ferramentas::TipoDeFerramenta::Loja;
        return cliqueConsumidoComo(AcaoDoCliqueNaInterface::AbrirLoja);
    }

    if (verificarCliqueNoBotao(mouseX, mouseY, botoes.zoomMais)) {
        return cliqueConsumidoComo(AcaoDoCliqueNaInterface::AumentarZoom);
    }

    if (verificarCliqueNoBotao(mouseX, mouseY, botoes.zoomMenos)) {
        return cliqueConsumidoComo(AcaoDoCliqueNaInterface::DiminuirZoom);
    }

    if (verificarCliqueNoBotao(mouseX, mouseY, botoes.ajuda) ||
        verificarCliqueNoBotao(mouseX, mouseY, botoes.estrela) ||
        verificarCliqueNoBotao(mouseX, mouseY, botoes.presente) ||
        verificarCliqueNoBotao(mouseX, mouseY, botoes.foto) ||
        pontoEstaNaAreaDoHud(mouseX, mouseY, botoes)) {
        return cliqueConsumidoComo(AcaoDoCliqueNaInterface::ConsumirClique);
    }

    return ResultadoDoCliqueNaInterface{};
}

} // namespace MiniFazenda::Apresentacao::Interface::BarraDeFerramentas
