#pragma once

#include "Apresentacao/Interface/AreaDeInteracao.hpp"
#include "Apresentacao/Renderizacao/Primitivas/PrimitivasSDL.hpp"
#include "Apresentacao/Renderizacao/UI/IconesDasFerramentas.hpp"
#include "Compartilhado/Constantes.hpp"
#include "Dominio/Ferramentas/TipoDeFerramenta.hpp"
#include "Infraestrutura/Assets/RecursosDaFazenda.hpp"

#include <SDL.h>

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
        case Dominio::Ferramentas::TipoDeFerramenta::Presente:
            desenharIconePresente(renderizador, area, corIcone);
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
    Interface::AreaDeInteracao botaoPresente,
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
        botaoPresente,
        Dominio::Ferramentas::TipoDeFerramenta::Presente,
        ferramentaSelecionada,
        texturaDoIcone(Dominio::Ferramentas::TipoDeFerramenta::Presente)
    );
}

} // namespace MiniFazenda::Apresentacao::Renderizacao::UI
