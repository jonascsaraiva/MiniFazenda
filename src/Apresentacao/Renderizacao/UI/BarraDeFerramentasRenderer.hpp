#pragma once

#include "Apresentacao/Interface/AreaDeInteracao.hpp"
#include "Apresentacao/Interface/BarraDeFerramentas/BarraDeFerramentas.hpp"
#include "Apresentacao/Renderizacao/Primitivas/PrimitivasSDL.hpp"
#include "Apresentacao/Renderizacao/UI/IconesDasFerramentas.hpp"
#include "Compartilhado/Constantes.hpp"
#include "Dominio/Ferramentas/TipoDeFerramenta.hpp"
#include "Infraestrutura/Assets/RecursosDaFazenda.hpp"

#include <SDL.h>

#include <optional>

namespace MiniFazenda::Apresentacao::Renderizacao::UI {

inline void desenharIconeDaFerramenta(
    SDL_Renderer* renderizador,
    SDL_Rect area,
    SDL_Color corIcone,
    Dominio::Ferramentas::TipoDeFerramenta ferramenta
) {
    switch (ferramenta) {
        case Dominio::Ferramentas::TipoDeFerramenta::Cursor:
            desenharIconeCursor(renderizador, area, corIcone);
            break;
        case Dominio::Ferramentas::TipoDeFerramenta::Enxada:
            desenharIconeEnxada(renderizador, area, corIcone);
            break;
        case Dominio::Ferramentas::TipoDeFerramenta::RemoverTerra:
            desenharIconeRemoverTerra(renderizador, area, corIcone);
            break;
        case Dominio::Ferramentas::TipoDeFerramenta::Semente:
            desenharIconeSemente(renderizador, area, corIcone);
            break;
        case Dominio::Ferramentas::TipoDeFerramenta::Loja:
            desenharIconeLoja(renderizador, area, corIcone);
            break;
    }
}

inline void desenharBotaoFerramenta(
    SDL_Renderer* renderizador,
    Interface::AreaDeInteracao areaDeInteracao,
    Dominio::Ferramentas::TipoDeFerramenta ferramentaDoBotao,
    Dominio::Ferramentas::TipoDeFerramenta ferramentaSelecionada,
    SDL_Texture* texturaDoIcone = nullptr
) {
    SDL_Rect area{
        areaDeInteracao.posicaoBotaoHorizontal,
        areaDeInteracao.posicaoBotaoVertical,
        areaDeInteracao.tamanhoBotaoLargura,
        areaDeInteracao.tamanhoBotaoAltura
    };

    const bool selecionado = ferramentaDoBotao == ferramentaSelecionada;
    Primitivas::preencherRetangulo(
        renderizador,
        area,
        selecionado ? SDL_Color{248, 226, 148, 255} : SDL_Color{238, 226, 203, 255}
    );
    Primitivas::definirCor(renderizador, selecionado ? SDL_Color{94, 75, 45, 255} : SDL_Color{117, 104, 87, 255});
    SDL_RenderDrawRect(renderizador, &area);

    const SDL_Color corIcone = selecionado ? SDL_Color{65, 49, 31, 255} : SDL_Color{81, 76, 67, 255};
    if (texturaDoIcone != nullptr) {
        constexpr int margemDoIcone = 6;
        SDL_Rect areaDoIcone{
            area.x + margemDoIcone,
            area.y + margemDoIcone,
            area.w - margemDoIcone * 2,
            area.h - margemDoIcone * 2
        };
        SDL_RenderCopy(renderizador, texturaDoIcone, nullptr, &areaDoIcone);
        return;
    }

    desenharIconeDaFerramenta(renderizador, area, corIcone, ferramentaDoBotao);
}

inline void desenharInterface(
    SDL_Renderer* renderizador,
    Dominio::Ferramentas::TipoDeFerramenta ferramentaSelecionada,
    Interface::AreaDeInteracao botaoCursor,
    Interface::AreaDeInteracao botaoEnxada,
    Interface::AreaDeInteracao botaoRemoverTerra,
    Interface::AreaDeInteracao botaoSemente,
    Interface::AreaDeInteracao botaoLoja,
    const MiniFazenda::Infraestrutura::Assets::TexturasDosBotoes& texturasDosBotoes
) {
    Primitivas::preencherRetangulo(
        renderizador,
        SDL_Rect{0, Compartilhado::Constantes::ALTURA_DA_JANELA - 84, Compartilhado::Constantes::LARGURA_DA_JANELA, 84},
        SDL_Color{61, 65, 57, 210}
    );

    const auto texturaDoIcone = [&texturasDosBotoes](Dominio::Ferramentas::TipoDeFerramenta ferramenta) {
        return texturasDosBotoes[Dominio::Ferramentas::indiceDaFerramenta(ferramenta)];
    };

    desenharBotaoFerramenta(
        renderizador,
        botaoCursor,
        Dominio::Ferramentas::TipoDeFerramenta::Cursor,
        ferramentaSelecionada,
        texturaDoIcone(Dominio::Ferramentas::TipoDeFerramenta::Cursor)
    );
    desenharBotaoFerramenta(
        renderizador,
        botaoEnxada,
        Dominio::Ferramentas::TipoDeFerramenta::Enxada,
        ferramentaSelecionada,
        texturaDoIcone(Dominio::Ferramentas::TipoDeFerramenta::Enxada)
    );
    desenharBotaoFerramenta(
        renderizador,
        botaoRemoverTerra,
        Dominio::Ferramentas::TipoDeFerramenta::RemoverTerra,
        ferramentaSelecionada,
        texturaDoIcone(Dominio::Ferramentas::TipoDeFerramenta::RemoverTerra)
    );
    desenharBotaoFerramenta(
        renderizador,
        botaoSemente,
        Dominio::Ferramentas::TipoDeFerramenta::Semente,
        ferramentaSelecionada,
        texturaDoIcone(Dominio::Ferramentas::TipoDeFerramenta::Semente)
    );
    desenharBotaoFerramenta(
        renderizador,
        botaoLoja,
        Dominio::Ferramentas::TipoDeFerramenta::Loja,
        ferramentaSelecionada,
        texturaDoIcone(Dominio::Ferramentas::TipoDeFerramenta::Loja)
    );
}

inline void desenharPainelDaLoja(
    SDL_Renderer* renderizador,
    const Interface::BarraDeFerramentas::PainelDaLoja& painel,
    const MiniFazenda::Infraestrutura::Assets::TexturasDasSementesPorSemente& texturasDasSementes,
    std::optional<int> identificadorDaSementeSelecionada
) {
    if (painel.opcoes.empty()) {
        return;
    }

    SDL_Rect areaDoFundo{
        painel.fundo.posicaoBotaoHorizontal,
        painel.fundo.posicaoBotaoVertical,
        painel.fundo.tamanhoBotaoLargura,
        painel.fundo.tamanhoBotaoAltura
    };
    Primitivas::preencherRetangulo(renderizador, areaDoFundo, SDL_Color{61, 65, 57, 235});
    Primitivas::definirCor(renderizador, SDL_Color{238, 226, 203, 255});
    SDL_RenderDrawRect(renderizador, &areaDoFundo);

    for (const Interface::BarraDeFerramentas::OpcaoDeSementeDaLoja& opcao : painel.opcoes) {
        SDL_Rect areaDaOpcao{
            opcao.area.posicaoBotaoHorizontal,
            opcao.area.posicaoBotaoVertical,
            opcao.area.tamanhoBotaoLargura,
            opcao.area.tamanhoBotaoAltura
        };
        const bool selecionada =
            identificadorDaSementeSelecionada.has_value() &&
            *identificadorDaSementeSelecionada == opcao.identificadorDaSemente;

        Primitivas::preencherRetangulo(
            renderizador,
            areaDaOpcao,
            selecionada ? SDL_Color{248, 226, 148, 255} : SDL_Color{238, 226, 203, 255}
        );
        Primitivas::definirCor(renderizador, selecionada ? SDL_Color{94, 75, 45, 255} : SDL_Color{117, 104, 87, 255});
        SDL_RenderDrawRect(renderizador, &areaDaOpcao);

        constexpr int margemDoIcone = 5;
        SDL_Rect areaDoIcone{
            areaDaOpcao.x + margemDoIcone,
            areaDaOpcao.y + margemDoIcone,
            areaDaOpcao.w - margemDoIcone * 2,
            areaDaOpcao.h - margemDoIcone * 2
        };

        const auto encontrada = texturasDasSementes.find(opcao.identificadorDaSemente);
        if (encontrada != texturasDasSementes.end() && encontrada->second != nullptr) {
            SDL_RenderCopy(renderizador, encontrada->second, nullptr, &areaDoIcone);
            continue;
        }

        desenharIconeSemente(
            renderizador,
            areaDoIcone,
            selecionada ? SDL_Color{65, 49, 31, 255} : SDL_Color{81, 76, 67, 255}
        );
    }
}

} // namespace MiniFazenda::Apresentacao::Renderizacao::UI
