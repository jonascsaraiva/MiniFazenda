#pragma once

#include "Apresentacao/Interface/AreaDeInteracao.hpp"
#include "Apresentacao/Interface/Loja/LayoutDaLoja.hpp"

#include <optional>

namespace MiniFazenda::Apresentacao::Interface::Loja {

enum class AcaoDoCliqueDaLoja {
    Nenhuma,
    ConsumirClique,
    Fechar,
    TrocarAba,
    TrocarFiltro,
    SelecionarSemente
};

struct ResultadoDoCliqueDaLoja {
    bool cliqueConsumido = false;
    AcaoDoCliqueDaLoja acao = AcaoDoCliqueDaLoja::Nenhuma;
    std::optional<int> identificadorDaSementeSelecionada;
};

inline ResultadoDoCliqueDaLoja cliqueDaLojaConsumidoComo(AcaoDoCliqueDaLoja acao) {
    ResultadoDoCliqueDaLoja resultado;
    resultado.cliqueConsumido = true;
    resultado.acao = acao;
    return resultado;
}

inline bool pontoEstaNaAreaDaLoja(int x, int y, const AreaDeInteracao& area) {
    return pontoEstaNaAreaDeInteracao(x, y, area);
}

inline void atualizarHoverDaLoja(
    int mouseX,
    int mouseY,
    EstadoDaLoja& estado,
    const LayoutCalculadoDaLoja& layout
) {
    if (!estado.aberta()) {
        estado.definirSementeSobHover(std::nullopt);
        return;
    }

    for (const CartaoDeSementeDaLoja& cartao : layout.cartoesDeSementes) {
        if (pontoEstaNaAreaDaLoja(mouseX, mouseY, cartao.area)) {
            estado.definirSementeSobHover(cartao.identificadorDaSemente);
            return;
        }
    }

    estado.definirSementeSobHover(std::nullopt);
}

inline ResultadoDoCliqueDaLoja processarCliqueDaLoja(
    int mouseX,
    int mouseY,
    EstadoDaLoja& estado,
    const LayoutCalculadoDaLoja& layout
) {
    if (!estado.aberta()) {
        return ResultadoDoCliqueDaLoja{};
    }

    if (!pontoEstaNaAreaDaLoja(mouseX, mouseY, layout.painelPrincipal)) {
        estado.fechar();
        return cliqueDaLojaConsumidoComo(AcaoDoCliqueDaLoja::Fechar);
    }

    if (pontoEstaNaAreaDaLoja(mouseX, mouseY, layout.botaoFechar)) {
        estado.fechar();
        return cliqueDaLojaConsumidoComo(AcaoDoCliqueDaLoja::Fechar);
    }

    for (const AreaDaAbaDaLoja& aba : layout.abasPrincipais) {
        if (pontoEstaNaAreaDaLoja(mouseX, mouseY, aba.area)) {
            estado.selecionarAbaPrincipal(aba.aba);
            return cliqueDaLojaConsumidoComo(AcaoDoCliqueDaLoja::TrocarAba);
        }
    }

    if (estado.abaPrincipalSelecionada() == AbaPrincipalDaLoja::Sementes) {
        for (const AreaDoFiltroDeSementesDaLoja& filtro : layout.filtrosDeSementes) {
            if (pontoEstaNaAreaDaLoja(mouseX, mouseY, filtro.area)) {
                estado.selecionarFiltroDeSementes(filtro.filtro);
                return cliqueDaLojaConsumidoComo(AcaoDoCliqueDaLoja::TrocarFiltro);
            }
        }

        for (const CartaoDeSementeDaLoja& cartao : layout.cartoesDeSementes) {
            if (pontoEstaNaAreaDaLoja(mouseX, mouseY, cartao.area) ||
                pontoEstaNaAreaDaLoja(mouseX, mouseY, cartao.botaoComprar)) {
                estado.definirSementeSelecionadaVisualmente(cartao.identificadorDaSemente);
                estado.fechar();

                ResultadoDoCliqueDaLoja resultado = cliqueDaLojaConsumidoComo(AcaoDoCliqueDaLoja::SelecionarSemente);
                resultado.identificadorDaSementeSelecionada = cartao.identificadorDaSemente;
                return resultado;
            }
        }
    }

    return cliqueDaLojaConsumidoComo(AcaoDoCliqueDaLoja::ConsumirClique);
}

} // namespace MiniFazenda::Apresentacao::Interface::Loja
