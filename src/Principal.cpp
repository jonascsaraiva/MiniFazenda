#include "Apresentacao/Cenas/CenaFazenda.hpp"
#include "Apresentacao/ConfiguracoesDoLayout.hpp"
#include "Compartilhado/ConstantesDaJanela.hpp"
#include "Dominio/Plantas/FabricaDePlantas.hpp"
#include "Infraestrutura/Assets/GerenciadorDeAtivosSDL.hpp"
#include "Infraestrutura/Assets/LocalizadorDeAssets.hpp"
#include "Infraestrutura/Assets/RecursosDaFazenda.hpp"
#include "Infraestrutura/Configuracao/LeitorDeConfiguracao.hpp"
#include "Infraestrutura/SDL/ContextoSDL.hpp"

#include <SDL.h>

#include <iostream>
#include <utility>

namespace {

namespace Assets = MiniFazenda::Infraestrutura::Assets;
namespace Cenas = MiniFazenda::Apresentacao::Cenas;
namespace Configuracao = MiniFazenda::Infraestrutura::Configuracao;
namespace Constantes = MiniFazenda::Compartilhado::Constantes;
namespace Plantas = MiniFazenda::Dominio::Plantas;
namespace SDLInfra = MiniFazenda::Infraestrutura::SDL;

} // namespace

int main(int, char**) {
    SDLInfra::InicializacaoSDL sdl;
    if (!sdl.inicializar()) {
        return 1;
    }

    SDLInfra::JanelaSDL janela(SDL_CreateWindow(
        Constantes::TITULO_DA_JANELA,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        Constantes::LARGURA_DA_JANELA,
        Constantes::ALTURA_DA_JANELA,
        SDL_WINDOW_SHOWN
    ));
    if (janela.ponteiro == nullptr) {
        std::cerr << "Falha ao criar janela: " << SDL_GetError() << '\n';
        return 1;
    }

    SDLInfra::RenderizadorSDL renderizador(SDLInfra::criarRenderizador(janela.ponteiro));
    if (renderizador.ponteiro == nullptr) {
        std::cerr << "Falha ao criar renderizador: " << SDL_GetError() << '\n';
        return 1;
    }

    SDL_SetRenderDrawBlendMode(renderizador.ponteiro, SDL_BLENDMODE_BLEND);
    SDLInfra::CursorOculto cursorOculto;

    const auto diretorioAssets = Assets::localizarDiretorioDeAssets();
    MiniFazenda::Apresentacao::ConfiguracoesDoLayout configuracoes;
    Configuracao::carregarConfiguracoesDoLayout(diretorioAssets / "config.ini", configuracoes);

    Assets::GerenciadorDeAtivosSDL ativos(renderizador.ponteiro);
    const Plantas::FabricaDePlantas fabricaDePlantas;
    const auto especiesDaLoja = fabricaDePlantas.todasAsEspecies();
    Assets::RecursosDaFazenda recursos = Assets::carregarRecursosDaFazenda(
        ativos,
        diretorioAssets,
        configuracoes,
        fabricaDePlantas
    );
    Assets::RecursosDeHud hud = Assets::carregarRecursosDeHud(ativos, diretorioAssets);
    if (sdl.audioInicializado) {
        ativos.tocarMusica(Assets::caminhoDaMusicaAmbiente(diretorioAssets));
    }

    Cenas::CenaFazenda cena(
        renderizador.ponteiro,
        ativos,
        diretorioAssets,
        std::move(configuracoes),
        std::move(recursos),
        std::move(hud),
        especiesDaLoja,
        sdl.audioInicializado
    );

    Uint32 ticksAnteriores = SDL_GetTicks();
    while (cena.deveContinuar()) {
        const Uint32 inicioDoQuadro = SDL_GetTicks();
        const float deltaTime = (inicioDoQuadro - ticksAnteriores) / 1000.0f;
        ticksAnteriores = inicioDoQuadro;

        SDL_Event evento;
        while (SDL_PollEvent(&evento) != 0) {
            cena.processarEvento(evento);
        }

        cena.atualizar(deltaTime);

        SDL_SetRenderDrawColor(renderizador.ponteiro, 0, 0, 0, 255);
        SDL_RenderClear(renderizador.ponteiro);
        cena.renderizar();
        SDL_RenderPresent(renderizador.ponteiro);

        const Uint32 duracaoDoQuadro = SDL_GetTicks() - inicioDoQuadro;
        if (duracaoDoQuadro < Constantes::MILISSEGUNDOS_POR_QUADRO) {
            SDL_Delay(Constantes::MILISSEGUNDOS_POR_QUADRO - duracaoDoQuadro);
        }
    }

    return 0;
}
