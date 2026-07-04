#pragma once

#include "Apresentacao/Interface/AreaDeInteracao.hpp"
#include "Apresentacao/Interface/BarraDeFerramentas/BarraDeFerramentas.hpp"
#include "Apresentacao/Renderizacao/Primitivas/PrimitivasSDL.hpp"
#include "Apresentacao/Renderizacao/UI/IconesDasFerramentas.hpp"
#include "Dominio/Ferramentas/TipoDeFerramenta.hpp"
#include "Infraestrutura/Assets/RecursosDaFazenda.hpp"

#include <SDL.h>

#include <algorithm>

namespace MiniFazenda::Apresentacao::Renderizacao::UI {

inline SDL_Rect converterAreaParaRetangulo(Interface::AreaDeInteracao areaDeInteracao) {
    return SDL_Rect{
        areaDeInteracao.posicaoBotaoHorizontal,
        areaDeInteracao.posicaoBotaoVertical,
        areaDeInteracao.tamanhoBotaoLargura,
        areaDeInteracao.tamanhoBotaoAltura
    };
}

inline SDL_Rect centralizarQuadrado(SDL_Rect area, int tamanho) {
    return SDL_Rect{
        area.x + (area.w - tamanho) / 2,
        area.y + (area.h - tamanho) / 2,
        tamanho,
        tamanho
    };
}

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

enum class IconeDoHud {
    Ajuda,
    ZoomMais,
    ZoomMenos,
    Estrela,
    Presente,
    Foto
};

inline void desenharIconeDoHud(SDL_Renderer* renderizador, SDL_Rect area, SDL_Color corIcone, IconeDoHud icone) {
    switch (icone) {
        case IconeDoHud::Ajuda:
            desenharIconeAjuda(renderizador, area, corIcone);
            break;
        case IconeDoHud::ZoomMais:
            desenharIconeZoomMais(renderizador, area, corIcone);
            break;
        case IconeDoHud::ZoomMenos:
            desenharIconeZoomMenos(renderizador, area, corIcone);
            break;
        case IconeDoHud::Estrela:
            desenharIconeEstrela(renderizador, area, corIcone);
            break;
        case IconeDoHud::Presente:
            desenharIconePresente(renderizador, area, corIcone);
            break;
        case IconeDoHud::Foto:
            desenharIconeFoto(renderizador, area, corIcone);
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
    SDL_Rect area = converterAreaParaRetangulo(areaDeInteracao);

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
        const int tamanhoDoIcone = std::max(1, std::min(area.w, area.h) - margemDoIcone * 2);
        SDL_Rect areaDoIcone = centralizarQuadrado(area, tamanhoDoIcone);
        SDL_RenderCopy(renderizador, texturaDoIcone, nullptr, &areaDoIcone);
        return;
    }

    const int tamanhoDoIconeVetorial = std::min(52, std::min(area.w, area.h));
    desenharIconeDaFerramenta(
        renderizador,
        centralizarQuadrado(area, tamanhoDoIconeVetorial),
        corIcone,
        ferramentaDoBotao
    );
}

inline void desenharBotaoDoHud(
    SDL_Renderer* renderizador,
    Interface::AreaDeInteracao areaDeInteracao,
    IconeDoHud icone,
    bool ativo
) {
    SDL_Rect area = converterAreaParaRetangulo(areaDeInteracao);

    Primitivas::preencherRetangulo(
        renderizador,
        area,
        ativo ? SDL_Color{238, 226, 203, 255} : SDL_Color{238, 226, 203, 115}
    );
    Primitivas::definirCor(
        renderizador,
        ativo ? SDL_Color{117, 104, 87, 255} : SDL_Color{117, 104, 87, 120}
    );
    SDL_RenderDrawRect(renderizador, &area);

    desenharIconeDoHud(
        renderizador,
        area,
        ativo ? SDL_Color{81, 76, 67, 255} : SDL_Color{81, 76, 67, 120},
        icone
    );
}

inline void desenharInterface(
    SDL_Renderer* renderizador,
    Dominio::Ferramentas::TipoDeFerramenta ferramentaSelecionada,
    const Interface::BarraDeFerramentas::BotoesDaInterface& botoes,
    const MiniFazenda::Infraestrutura::Assets::TexturasDosBotoes& texturasDosBotoes
) {
    const auto texturaDoIcone = [&texturasDosBotoes](Dominio::Ferramentas::TipoDeFerramenta ferramenta) {
        return texturasDosBotoes[Dominio::Ferramentas::indiceDaFerramenta(ferramenta)];
    };

    desenharBotaoFerramenta(
        renderizador,
        botoes.loja,
        Dominio::Ferramentas::TipoDeFerramenta::Loja,
        ferramentaSelecionada,
        texturaDoIcone(Dominio::Ferramentas::TipoDeFerramenta::Loja)
    );
    desenharBotaoFerramenta(
        renderizador,
        botoes.cursor,
        Dominio::Ferramentas::TipoDeFerramenta::Cursor,
        ferramentaSelecionada,
        texturaDoIcone(Dominio::Ferramentas::TipoDeFerramenta::Cursor)
    );
    desenharBotaoFerramenta(
        renderizador,
        botoes.enxada,
        Dominio::Ferramentas::TipoDeFerramenta::Enxada,
        ferramentaSelecionada,
        texturaDoIcone(Dominio::Ferramentas::TipoDeFerramenta::Enxada)
    );
    desenharBotaoFerramenta(
        renderizador,
        botoes.removerTerra,
        Dominio::Ferramentas::TipoDeFerramenta::RemoverTerra,
        ferramentaSelecionada,
        texturaDoIcone(Dominio::Ferramentas::TipoDeFerramenta::RemoverTerra)
    );

    desenharBotaoDoHud(renderizador, botoes.estrela, IconeDoHud::Estrela, false);
    desenharBotaoDoHud(renderizador, botoes.presente, IconeDoHud::Presente, false);
    desenharBotaoDoHud(renderizador, botoes.foto, IconeDoHud::Foto, false);
    desenharBotaoDoHud(renderizador, botoes.ajuda, IconeDoHud::Ajuda, false);
    desenharBotaoDoHud(renderizador, botoes.zoomMais, IconeDoHud::ZoomMais, true);
    desenharBotaoDoHud(renderizador, botoes.zoomMenos, IconeDoHud::ZoomMenos, true);
}

} // namespace MiniFazenda::Apresentacao::Renderizacao::UI
