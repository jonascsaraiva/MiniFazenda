#pragma once

#include "Apresentacao/Interface/Loja/EstadoDaLoja.hpp"
#include "Apresentacao/Interface/Loja/LayoutDaLoja.hpp"
#include "Apresentacao/Renderizacao/Primitivas/PrimitivasSDL.hpp"
#include "Apresentacao/Renderizacao/UI/IconesDasFerramentas.hpp"
#include "Infraestrutura/Assets/RecursosDaFazenda.hpp"

#include <SDL.h>
#include <SDL_ttf.h>

#include <algorithm>
#include <optional>
#include <string>
#include <vector>

namespace MiniFazenda::Apresentacao::Renderizacao::UI::LojaRenderer {

namespace Loja = MiniFazenda::Apresentacao::Interface::Loja;
namespace Primitivas = MiniFazenda::Apresentacao::Renderizacao::Primitivas;

inline SDL_Rect converterAreaParaRetangulo(Interface::AreaDeInteracao area) {
    return SDL_Rect{
        area.posicaoBotaoHorizontal,
        area.posicaoBotaoVertical,
        area.tamanhoBotaoLargura,
        area.tamanhoBotaoAltura
    };
}

inline bool fontePossuiAcentosDaLoja(TTF_Font* fonte) {
    return fonte != nullptr &&
           TTF_GlyphIsProvided(fonte, 0x00E3) != 0 &&
           TTF_GlyphIsProvided(fonte, 0x00E7) != 0 &&
           TTF_GlyphIsProvided(fonte, 0x00F5) != 0;
}

inline std::string textoDaAba(Loja::AbaPrincipalDaLoja aba, TTF_Font* fonte) {
    switch (aba) {
        case Loja::AbaPrincipalDaLoja::Sementes:
            return "Sementes";
        case Loja::AbaPrincipalDaLoja::Animais:
            return "Animais";
        case Loja::AbaPrincipalDaLoja::Construcoes:
            return fontePossuiAcentosDaLoja(fonte) ? "Constru\xC3\xA7\xC3\xB5" "es" : "Construcoes";
        case Loja::AbaPrincipalDaLoja::Decoracoes:
            return fontePossuiAcentosDaLoja(fonte) ? "Decora\xC3\xA7\xC3\xB5" "es" : "Decoracoes";
        default:
            return {};
    }
}

inline std::string textoDoFiltro(Loja::FiltroDeSementesDaLoja filtro, TTF_Font* fonte) {
    switch (filtro) {
        case Loja::FiltroDeSementesDaLoja::Todos:
            return "Todos";
        case Loja::FiltroDeSementesDaLoja::Frutas:
            return "Frutas";
        case Loja::FiltroDeSementesDaLoja::Vegetais:
            return "Vegetais";
        case Loja::FiltroDeSementesDaLoja::Graos:
            return fontePossuiAcentosDaLoja(fonte) ? "Gr\xC3\xA3" "os" : "Graos";
        default:
            return {};
    }
}

inline const Loja::ItemDeSementeDaLoja* encontrarItemDeSemente(
    const std::vector<Loja::ItemDeSementeDaLoja>& itens,
    int identificadorDaSemente
) {
    for (const Loja::ItemDeSementeDaLoja& item : itens) {
        if (item.identificadorDaSemente == identificadorDaSemente) {
            return &item;
        }
    }

    return nullptr;
}

inline void desenharTextoCentralizado(
    SDL_Renderer* renderizador,
    TTF_Font* fonte,
    const std::string& texto,
    Interface::AreaDeInteracao area,
    SDL_Color cor
) {
    if (fonte == nullptr || texto.empty()) {
        return;
    }

    int larguraDoTexto = 0;
    int alturaDoTexto = 0;
    if (TTF_SizeUTF8(fonte, texto.c_str(), &larguraDoTexto, &alturaDoTexto) != 0) {
        Primitivas::desenharTexto(
            renderizador,
            fonte,
            texto,
            area.posicaoBotaoHorizontal,
            area.posicaoBotaoVertical,
            cor
        );
        return;
    }

    Primitivas::desenharTexto(
        renderizador,
        fonte,
        texto,
        area.posicaoBotaoHorizontal + (area.tamanhoBotaoLargura - larguraDoTexto) / 2,
        area.posicaoBotaoVertical + (area.tamanhoBotaoAltura - alturaDoTexto) / 2,
        cor
    );
}

inline void desenharTextoAlinhadoEsquerda(
    SDL_Renderer* renderizador,
    TTF_Font* fonte,
    const std::string& texto,
    Interface::AreaDeInteracao area,
    SDL_Color cor
) {
    if (fonte == nullptr || texto.empty()) {
        return;
    }

    int alturaDoTexto = 0;
    int larguraDoTexto = 0;
    TTF_SizeUTF8(fonte, texto.c_str(), &larguraDoTexto, &alturaDoTexto);
    Primitivas::desenharTexto(
        renderizador,
        fonte,
        texto,
        area.posicaoBotaoHorizontal,
        area.posicaoBotaoVertical + std::max(0, (area.tamanhoBotaoAltura - alturaDoTexto) / 2),
        cor
    );
}

inline void desenharBotaoComTexto(
    SDL_Renderer* renderizador,
    TTF_Font* fonte,
    Interface::AreaDeInteracao area,
    const std::string& texto,
    bool ativo,
    bool destacado
) {
    SDL_Rect retangulo = converterAreaParaRetangulo(area);
    const SDL_Color corFundo = ativo ? SDL_Color{247, 226, 164, 235} :
        (destacado ? SDL_Color{239, 215, 167, 230} : SDL_Color{234, 208, 157, 205});
    const SDL_Color corBorda = ativo ? SDL_Color{91, 63, 35, 255} : SDL_Color{132, 97, 59, 230};
    const SDL_Color corTexto = ativo ? SDL_Color{65, 43, 23, 255} : SDL_Color{77, 55, 32, 255};

    Primitivas::preencherRetangulo(renderizador, retangulo, corFundo);
    Primitivas::definirCor(renderizador, corBorda);
    SDL_RenderDrawRect(renderizador, &retangulo);
    desenharTextoCentralizado(renderizador, fonte, texto, area, corTexto);
}

inline void desenharPainelPrincipal(
    SDL_Renderer* renderizador,
    const Loja::LayoutCalculadoDaLoja& layout,
    SDL_Texture* texturaFundo
) {
    const SDL_Rect tela = converterAreaParaRetangulo(layout.areaDaTela);
    Primitivas::preencherRetangulo(renderizador, tela, SDL_Color{20, 16, 12, 120});

    const SDL_Rect painel = converterAreaParaRetangulo(layout.painelPrincipal);
    if (texturaFundo != nullptr) {
        SDL_RenderCopy(renderizador, texturaFundo, nullptr, &painel);
    } else {
        Primitivas::preencherRetangulo(renderizador, painel, SDL_Color{222, 184, 128, 245});
    }

    Primitivas::definirCor(renderizador, SDL_Color{97, 62, 32, 255});
    SDL_RenderDrawRect(renderizador, &painel);

    SDL_Rect bordaInterna{painel.x + 6, painel.y + 6, painel.w - 12, painel.h - 12};
    Primitivas::definirCor(renderizador, SDL_Color{255, 235, 184, 155});
    SDL_RenderDrawRect(renderizador, &bordaInterna);
}

inline void desenharAbas(
    SDL_Renderer* renderizador,
    TTF_Font* fonte,
    const Loja::EstadoDaLoja& estado,
    const Loja::LayoutCalculadoDaLoja& layout
) {
    for (const Loja::AreaDaAbaDaLoja& aba : layout.abasPrincipais) {
        desenharBotaoComTexto(
            renderizador,
            fonte,
            aba.area,
            textoDaAba(aba.aba, fonte),
            aba.aba == estado.abaPrincipalSelecionada(),
            false
        );
    }
}

inline void desenharFiltrosDeSementes(
    SDL_Renderer* renderizador,
    TTF_Font* fonte,
    const Loja::EstadoDaLoja& estado,
    const Loja::LayoutCalculadoDaLoja& layout
) {
    if (estado.abaPrincipalSelecionada() != Loja::AbaPrincipalDaLoja::Sementes) {
        return;
    }

    for (const Loja::AreaDoFiltroDeSementesDaLoja& filtro : layout.filtrosDeSementes) {
        desenharBotaoComTexto(
            renderizador,
            fonte,
            filtro.area,
            textoDoFiltro(filtro.filtro, fonte),
            filtro.filtro == estado.filtroDeSementesSelecionado(),
            false
        );
    }
}

inline void desenharCartaoDeSemente(
    SDL_Renderer* renderizador,
    TTF_Font* fonte,
    const Loja::EstadoDaLoja& estado,
    const Loja::CartaoDeSementeDaLoja& cartao,
    const Loja::ItemDeSementeDaLoja& item,
    const MiniFazenda::Infraestrutura::Assets::TexturasDasSementesPorSemente& texturasDasSementes,
    std::optional<int> identificadorDaSementeSelecionadaNoJogo
) {
    const bool emHover = estado.identificadorDaSementeSobHover().has_value() &&
        *estado.identificadorDaSementeSobHover() == cartao.identificadorDaSemente;
    const bool selecionada =
        identificadorDaSementeSelecionadaNoJogo.has_value() &&
        *identificadorDaSementeSelecionadaNoJogo == cartao.identificadorDaSemente;

    SDL_Rect area = converterAreaParaRetangulo(cartao.area);
    const SDL_Color fundo = selecionada ? SDL_Color{251, 230, 155, 245} :
        (emHover ? SDL_Color{250, 226, 176, 238} : SDL_Color{244, 216, 165, 225});
    Primitivas::preencherRetangulo(renderizador, area, fundo);
    Primitivas::definirCor(renderizador, selecionada ? SDL_Color{82, 56, 31, 255} : SDL_Color{137, 95, 54, 235});
    SDL_RenderDrawRect(renderizador, &area);

    SDL_Rect areaDoIcone = converterAreaParaRetangulo(cartao.areaDoIcone);
    const auto textura = texturasDasSementes.find(cartao.identificadorDaSemente);
    if (textura != texturasDasSementes.end() && textura->second != nullptr) {
        SDL_RenderCopy(renderizador, textura->second, nullptr, &areaDoIcone);
    } else {
        desenharIconeSemente(renderizador, areaDoIcone, SDL_Color{72, 78, 48, 255});
    }

    desenharTextoCentralizado(renderizador, fonte, item.nome, cartao.areaDoNome, SDL_Color{62, 43, 25, 255});
    desenharTextoCentralizado(
        renderizador,
        fonte,
        std::to_string(item.precoEmMoedas) + " moedas",
        cartao.areaDoPreco,
        SDL_Color{90, 63, 34, 255}
    );
    desenharBotaoComTexto(renderizador, fonte, cartao.botaoComprar, "Comprar", false, emHover);
}

inline void desenharConteudoDeSementes(
    SDL_Renderer* renderizador,
    TTF_Font* fonte,
    const Loja::EstadoDaLoja& estado,
    const Loja::LayoutCalculadoDaLoja& layout,
    const std::vector<Loja::ItemDeSementeDaLoja>& itensDeSementes,
    const MiniFazenda::Infraestrutura::Assets::TexturasDasSementesPorSemente& texturasDasSementes,
    std::optional<int> identificadorDaSementeSelecionadaNoJogo
) {
    for (const Loja::CartaoDeSementeDaLoja& cartao : layout.cartoesDeSementes) {
        const Loja::ItemDeSementeDaLoja* item =
            encontrarItemDeSemente(itensDeSementes, cartao.identificadorDaSemente);
        if (item == nullptr) {
            continue;
        }

        desenharCartaoDeSemente(
            renderizador,
            fonte,
            estado,
            cartao,
            *item,
            texturasDasSementes,
            identificadorDaSementeSelecionadaNoJogo
        );
    }

    if (layout.cartoesDeSementes.empty()) {
        desenharTextoCentralizado(
            renderizador,
            fonte,
            "Em breve",
            layout.areaDeConteudo,
            SDL_Color{80, 56, 32, 255}
        );
    }
}

inline void desenharConteudoTemporario(
    SDL_Renderer* renderizador,
    TTF_Font* fonte,
    const Loja::LayoutCalculadoDaLoja& layout
) {
    SDL_Rect conteudo = converterAreaParaRetangulo(layout.areaDeConteudo);
    Primitivas::preencherRetangulo(renderizador, conteudo, SDL_Color{244, 216, 165, 120});
    Primitivas::definirCor(renderizador, SDL_Color{137, 95, 54, 180});
    SDL_RenderDrawRect(renderizador, &conteudo);
    desenharTextoCentralizado(renderizador, fonte, "Em breve", layout.areaDeConteudo, SDL_Color{80, 56, 32, 255});
}

inline void desenharLoja(
    SDL_Renderer* renderizador,
    TTF_Font* fonte,
    const Loja::EstadoDaLoja& estado,
    const Loja::LayoutCalculadoDaLoja& layout,
    SDL_Texture* texturaFundo,
    const std::vector<Loja::ItemDeSementeDaLoja>& itensDeSementes,
    const MiniFazenda::Infraestrutura::Assets::TexturasDasSementesPorSemente& texturasDasSementes,
    int moedasDoJogador,
    std::optional<int> identificadorDaSementeSelecionadaNoJogo
) {
    if (!estado.aberta()) {
        return;
    }

    desenharPainelPrincipal(renderizador, layout, texturaFundo);

    desenharTextoCentralizado(renderizador, fonte, "Loja", layout.areaDoTitulo, SDL_Color{66, 43, 24, 255});
    desenharTextoAlinhadoEsquerda(
        renderizador,
        fonte,
        "Moedas: " + std::to_string(moedasDoJogador),
        layout.areaDoDinheiro,
        SDL_Color{66, 43, 24, 255}
    );
    desenharBotaoComTexto(renderizador, fonte, layout.botaoFechar, "X", false, false);

    desenharAbas(renderizador, fonte, estado, layout);
    desenharFiltrosDeSementes(renderizador, fonte, estado, layout);

    if (estado.abaPrincipalSelecionada() == Loja::AbaPrincipalDaLoja::Sementes) {
        desenharConteudoDeSementes(
            renderizador,
            fonte,
            estado,
            layout,
            itensDeSementes,
            texturasDasSementes,
            identificadorDaSementeSelecionadaNoJogo
        );
        return;
    }

    desenharConteudoTemporario(renderizador, fonte, layout);
}

} // namespace MiniFazenda::Apresentacao::Renderizacao::UI::LojaRenderer
