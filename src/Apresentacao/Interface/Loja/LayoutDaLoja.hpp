#pragma once

#include "Apresentacao/Interface/AreaDeInteracao.hpp"
#include "Apresentacao/Interface/Loja/EstadoDaLoja.hpp"
#include "Compartilhado/ConstantesDaJanela.hpp"

#include <algorithm>
#include <array>
#include <vector>

namespace MiniFazenda::Apresentacao::Interface::Loja {

struct AreaDaAbaDaLoja {
    AbaPrincipalDaLoja aba = AbaPrincipalDaLoja::Sementes;
    AreaDeInteracao area;
};

struct AreaDoFiltroDeSementesDaLoja {
    FiltroDeSementesDaLoja filtro = FiltroDeSementesDaLoja::Todos;
    AreaDeInteracao area;
};

struct CartaoDeSementeDaLoja {
    int identificadorDaSemente = -1;
    AreaDeInteracao area;
    AreaDeInteracao areaDoIcone;
    AreaDeInteracao areaDoNome;
    AreaDeInteracao areaDoPreco;
    AreaDeInteracao botaoComprar;
};

struct LayoutCalculadoDaLoja {
    AreaDeInteracao areaDaTela;
    AreaDeInteracao painelPrincipal;
    AreaDeInteracao areaDoTitulo;
    AreaDeInteracao areaDoDinheiro;
    AreaDeInteracao botaoFechar;
    std::array<AreaDaAbaDaLoja, ABAS_PRINCIPAIS_DA_LOJA.size()> abasPrincipais{};
    std::array<AreaDoFiltroDeSementesDaLoja, FILTROS_DE_SEMENTES_DA_LOJA.size()> filtrosDeSementes{};
    AreaDeInteracao areaDeConteudo;
    std::vector<CartaoDeSementeDaLoja> cartoesDeSementes;
};

inline AreaDeInteracao criarAreaDaLoja(int x, int y, int largura, int altura) {
    return AreaDeInteracao{x, y, std::max(0, largura), std::max(0, altura)};
}

inline LayoutCalculadoDaLoja calcularLayoutDaLoja(
    int larguraDaJanela,
    int alturaDaJanela,
    const EstadoDaLoja& estado,
    const std::vector<ItemDeSementeDaLoja>& itensDeSementes
) {
    LayoutCalculadoDaLoja layout;

    constexpr int margemDaTela = 40;
    constexpr int preenchimentoHorizontal = 32;
    constexpr int larguraPreferidaDoPainel = 880;
    constexpr int alturaPreferidaDoPainel = 560;
    constexpr int larguraMinimaDoPainel = 320;
    constexpr int alturaMinimaDoPainel = 360;

    const int larguraDisponivel = std::max(larguraMinimaDoPainel, larguraDaJanela - margemDaTela * 2);
    const int alturaDisponivel = std::max(alturaMinimaDoPainel, alturaDaJanela - margemDaTela * 2);
    const int larguraDoPainel = std::clamp(larguraPreferidaDoPainel, larguraMinimaDoPainel, larguraDisponivel);
    const int alturaDoPainel = std::clamp(alturaPreferidaDoPainel, alturaMinimaDoPainel, alturaDisponivel);
    const int painelX = (larguraDaJanela - larguraDoPainel) / 2;
    const int painelY = (alturaDaJanela - alturaDoPainel) / 2;

    layout.areaDaTela = criarAreaDaLoja(0, 0, larguraDaJanela, alturaDaJanela);
    layout.painelPrincipal = criarAreaDaLoja(painelX, painelY, larguraDoPainel, alturaDoPainel);
    layout.areaDoTitulo = criarAreaDaLoja(painelX, painelY + 18, larguraDoPainel, 42);
    layout.areaDoDinheiro = criarAreaDaLoja(painelX + preenchimentoHorizontal, painelY + 28, 220, 28);
    layout.botaoFechar = criarAreaDaLoja(painelX + larguraDoPainel - 54, painelY + 18, 36, 36);

    constexpr int espacamentoDasAbas = 10;
    constexpr int alturaDaAba = 42;
    const int larguraDisponivelParaAbas = larguraDoPainel - preenchimentoHorizontal * 2;
    const int larguraDaAba = std::max(
        96,
        std::min(142, (larguraDisponivelParaAbas - espacamentoDasAbas * 3) / 4)
    );
    const int larguraTotalDasAbas = larguraDaAba * 4 + espacamentoDasAbas * 3;
    const int abasX = painelX + (larguraDoPainel - larguraTotalDasAbas) / 2;
    const int abasY = painelY + 88;

    for (std::size_t indice = 0; indice < ABAS_PRINCIPAIS_DA_LOJA.size(); ++indice) {
        layout.abasPrincipais[indice] = AreaDaAbaDaLoja{
            ABAS_PRINCIPAIS_DA_LOJA[indice],
            criarAreaDaLoja(
                abasX + static_cast<int>(indice) * (larguraDaAba + espacamentoDasAbas),
                abasY,
                larguraDaAba,
                alturaDaAba
            )
        };
    }

    constexpr int espacamentoDosFiltros = 8;
    constexpr int alturaDoFiltro = 34;
    const int larguraDoFiltro = std::max(
        86,
        std::min(116, (larguraDisponivelParaAbas - espacamentoDosFiltros * 3) / 4)
    );
    const int larguraTotalDosFiltros = larguraDoFiltro * 4 + espacamentoDosFiltros * 3;
    const int filtrosX = painelX + (larguraDoPainel - larguraTotalDosFiltros) / 2;
    const int filtrosY = abasY + alturaDaAba + 16;

    for (std::size_t indice = 0; indice < FILTROS_DE_SEMENTES_DA_LOJA.size(); ++indice) {
        layout.filtrosDeSementes[indice] = AreaDoFiltroDeSementesDaLoja{
            FILTROS_DE_SEMENTES_DA_LOJA[indice],
            criarAreaDaLoja(
                filtrosX + static_cast<int>(indice) * (larguraDoFiltro + espacamentoDosFiltros),
                filtrosY,
                larguraDoFiltro,
                alturaDoFiltro
            )
        };
    }

    const bool filtrosVisiveis = estado.abaPrincipalSelecionada() == AbaPrincipalDaLoja::Sementes;
    const int conteudoY = filtrosVisiveis ? filtrosY + alturaDoFiltro + 24 : abasY + alturaDaAba + 28;
    const int conteudoX = painelX + preenchimentoHorizontal;
    const int conteudoLargura = larguraDoPainel - preenchimentoHorizontal * 2;
    const int conteudoAltura = painelY + alturaDoPainel - 32 - conteudoY;
    layout.areaDeConteudo = criarAreaDaLoja(conteudoX, conteudoY, conteudoLargura, conteudoAltura);

    if (estado.abaPrincipalSelecionada() != AbaPrincipalDaLoja::Sementes) {
        return layout;
    }

    constexpr int larguraPreferidaDoCartao = 156;
    constexpr int alturaDoCartao = 198;
    constexpr int espacamentoDosCartoes = 16;
    const int colunas = std::max(
        1,
        (layout.areaDeConteudo.tamanhoBotaoLargura + espacamentoDosCartoes) /
            (larguraPreferidaDoCartao + espacamentoDosCartoes)
    );
    const int larguraDoCartao = std::min(
        larguraPreferidaDoCartao,
        (layout.areaDeConteudo.tamanhoBotaoLargura - espacamentoDosCartoes * (colunas - 1)) / colunas
    );

    int indiceDoCartao = 0;
    for (const ItemDeSementeDaLoja& item : itensDeSementes) {
        if (!itemDeSementePassaNoFiltro(item, estado.filtroDeSementesSelecionado())) {
            continue;
        }

        const int coluna = indiceDoCartao % colunas;
        const int linha = indiceDoCartao / colunas;
        const int cartaoX = layout.areaDeConteudo.posicaoBotaoHorizontal +
            coluna * (larguraDoCartao + espacamentoDosCartoes);
        const int cartaoY = layout.areaDeConteudo.posicaoBotaoVertical +
            linha * (alturaDoCartao + espacamentoDosCartoes);

        if (cartaoY + alturaDoCartao >
            layout.areaDeConteudo.posicaoBotaoVertical + layout.areaDeConteudo.tamanhoBotaoAltura) {
            break;
        }

        layout.cartoesDeSementes.push_back(CartaoDeSementeDaLoja{
            item.identificadorDaSemente,
            criarAreaDaLoja(cartaoX, cartaoY, larguraDoCartao, alturaDoCartao),
            criarAreaDaLoja(cartaoX + 38, cartaoY + 18, larguraDoCartao - 76, 58),
            criarAreaDaLoja(cartaoX + 12, cartaoY + 86, larguraDoCartao - 24, 28),
            criarAreaDaLoja(cartaoX + 12, cartaoY + 116, larguraDoCartao - 24, 24),
            criarAreaDaLoja(cartaoX + 18, cartaoY + alturaDoCartao - 52, larguraDoCartao - 36, 34)
        });
        ++indiceDoCartao;
    }

    return layout;
}

inline LayoutCalculadoDaLoja calcularLayoutDaLoja(
    const EstadoDaLoja& estado,
    const std::vector<ItemDeSementeDaLoja>& itensDeSementes
) {
    return calcularLayoutDaLoja(
        Compartilhado::Constantes::LARGURA_DA_JANELA,
        Compartilhado::Constantes::ALTURA_DA_JANELA,
        estado,
        itensDeSementes
    );
}

} // namespace MiniFazenda::Apresentacao::Interface::Loja
