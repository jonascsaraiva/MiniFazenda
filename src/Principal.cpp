#include "Aplicacao/Servicos/InicializadorDaFazenda.hpp"
#include "Aplicacao/Servicos/ServicoDeFerramentas.hpp"
#include "Aplicacao/Servicos/ServicoDeTempo.hpp"
#include "Apresentacao/Camera/CameraDoJogo.hpp"
#include "Apresentacao/ConfiguracoesDoLayout.hpp"
#include "Apresentacao/Interface/BarraDeFerramentas/BarraDeFerramentas.hpp"
#include "Apresentacao/Renderizacao/Cursores/CursorCustomizado.hpp"
#include "Apresentacao/Renderizacao/Mundo/DesenhoDoMundo.hpp"
#include "Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp"
#include "Apresentacao/Renderizacao/UI/BarraDeFerramentasRenderer.hpp"
#include "Compartilhado/Constantes.hpp"
#include "Compartilhado/Geometria/Posicoes.hpp"
#include "Dominio/Plantas/FabricaDePlantas.hpp"
#include "Infraestrutura/Assets/GerenciadorDeAtivosSDL.hpp"
#include "Infraestrutura/Assets/LocalizadorDeAssets.hpp"
#include "Infraestrutura/Assets/RecursosDaFazenda.hpp"
#include "Infraestrutura/Configuracao/LeitorDeConfiguracao.hpp"
#include "Infraestrutura/SDL/ContextoSDL.hpp"

#include <SDL.h>
#include <iostream>

namespace {

namespace AppServicos = MiniFazenda::Aplicacao::Servicos; namespace Assets = MiniFazenda::Infraestrutura::Assets;
namespace BarraFerramentas = MiniFazenda::Apresentacao::Interface::BarraDeFerramentas;
namespace Camera = MiniFazenda::Apresentacao::Camera; namespace Configuracao = MiniFazenda::Infraestrutura::Configuracao;
namespace Constantes = MiniFazenda::Compartilhado::Constantes;
namespace Cursores = MiniFazenda::Apresentacao::Renderizacao::Cursores; namespace Geometria = MiniFazenda::Compartilhado::Geometria;
namespace Mundo = MiniFazenda::Apresentacao::Renderizacao::Mundo; namespace Plantas = MiniFazenda::Dominio::Plantas;
namespace SDLInfra = MiniFazenda::Infraestrutura::SDL; namespace UI = MiniFazenda::Apresentacao::Renderizacao::UI;

} // namespace

int main(int, char**) {
    SDLInfra::InicializacaoSDL sdl;
    if (!sdl.inicializar()) { return 1; }

    SDLInfra::JanelaSDL janela(SDL_CreateWindow(
        Constantes::TITULO_DA_JANELA, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        Constantes::LARGURA_DA_JANELA, Constantes::ALTURA_DA_JANELA, SDL_WINDOW_SHOWN
    ));
    if (janela.ponteiro == nullptr) { std::cerr << "Falha ao criar janela: " << SDL_GetError() << '\n'; return 1; }

    SDLInfra::RenderizadorSDL renderizador(SDLInfra::criarRenderizador(janela.ponteiro));
    if (renderizador.ponteiro == nullptr) { std::cerr << "Falha ao criar renderizador: " << SDL_GetError() << '\n'; return 1; }

    SDL_SetRenderDrawBlendMode(renderizador.ponteiro, SDL_BLENDMODE_BLEND);
    SDLInfra::CursorOculto cursorOculto;

    const auto diretorioAssets = Assets::localizarDiretorioDeAssets();
    MiniFazenda::Apresentacao::ConfiguracoesDoLayout configuracoes;
    Configuracao::carregarConfiguracoesDoLayout(diretorioAssets / "config.ini", configuracoes);

    auto jogo = AppServicos::criarEstadoInicialDoJogo();
    Assets::GerenciadorDeAtivosSDL ativos(renderizador.ponteiro);
    const Plantas::FabricaDePlantas fabricaDePlantas;
    Assets::RecursosDaFazenda recursos = Assets::carregarRecursosDaFazenda(
        ativos, diretorioAssets, configuracoes, fabricaDePlantas
    );
    if (sdl.audioInicializado) { ativos.tocarMusica(Assets::caminhoDaMusicaAmbiente(diretorioAssets)); }

    Camera::EstadoDaCamera camera;
    Camera::aplicarOrigemCentradaDaGrade(configuracoes, jogo.tamanhoAtualDoGrid());
    const BarraFerramentas::BotoesDaInterface botoes = BarraFerramentas::criarBotoesDaInterface();

    bool executando = true;
    int mouseX = Constantes::LARGURA_DA_JANELA / 2;
    int mouseY = Constantes::ALTURA_DA_JANELA / 2;
    Uint32 ticksAnteriores = SDL_GetTicks();

    while (executando) {
        const Uint32 inicioDoQuadro = SDL_GetTicks();
        const float deltaTime = (inicioDoQuadro - ticksAnteriores) / 1000.0f;
        ticksAnteriores = inicioDoQuadro;

        SDL_Event evento;
        while (SDL_PollEvent(&evento) != 0) {
            if (evento.type == SDL_QUIT) { executando = false; }
            if (evento.type == SDL_MOUSEMOTION) {
                mouseX = evento.motion.x;
                mouseY = evento.motion.y;
                if (camera.panAtivo) {
                    Camera::moverPanDaCamera(camera, configuracoes, jogo.tamanhoAtualDoGrid(), evento.motion.xrel, evento.motion.yrel, evento.motion.timestamp);
                }
            }
            if (evento.type == SDL_MOUSEBUTTONUP) { Camera::finalizarPanDaCamera(camera, evento.button.button); }
            if (evento.type == SDL_MOUSEWHEEL) {
                int mouseXNoScroll = 0;
                int mouseYNoScroll = 0;
                SDL_GetMouseState(&mouseXNoScroll, &mouseYNoScroll);
                Camera::aplicarZoomNoPonto(camera, configuracoes, jogo.tamanhoAtualDoGrid(), mouseXNoScroll, mouseYNoScroll, evento.wheel.y);
            }
            if (evento.type == SDL_KEYDOWN) {
                if (evento.key.keysym.sym == SDLK_ESCAPE) { executando = false; }
                if (evento.key.keysym.sym == SDLK_F5) {
                    Configuracao::carregarConfiguracoesDoLayout(diretorioAssets / "config.ini", configuracoes);
                    Camera::aplicarOrigemCentradaDaGrade(configuracoes, jogo.tamanhoAtualDoGrid());
                    Camera::limitarPanAosLimitesDoGrid(camera, configuracoes, jogo.tamanhoAtualDoGrid());
                    recursos.texturaFundo = Assets::carregarTexturaDeFundoPrincipal(ativos, diretorioAssets, configuracoes);
                }
                if (evento.key.keysym.sym == SDLK_HOME) {
                    Camera::centralizarCamera(camera);
                    Camera::aplicarOrigemCentradaDaGrade(configuracoes, jogo.tamanhoAtualDoGrid());
                }
            }
            if (evento.type == SDL_MOUSEBUTTONDOWN) {
                if (evento.button.button == SDL_BUTTON_MIDDLE || evento.button.button == SDL_BUTTON_RIGHT) {
                    Camera::iniciarPanDaCamera(camera, evento.button.button, evento.button.timestamp);
                    continue;
                } else if (evento.button.button == SDL_BUTTON_LEFT) {
                    if (camera.panAtivo) { continue; }
                    mouseX = evento.button.x;
                    mouseY = evento.button.y;

                    auto ferramentaSelecionada = jogo.ferramentaSelecionada();
                    if (BarraFerramentas::processarCliqueNaInterface(mouseX, mouseY, botoes, ferramentaSelecionada)) {
                        jogo.selecionarFerramenta(ferramentaSelecionada);
                        if (sdl.audioInicializado) {
                            ativos.tocarSom(Assets::caminhoDoSomDeCliqueDaInterface(diretorioAssets));
                        }
                        continue;
                    }

                    const auto posicaoNaGrade = Mundo::converterMouseParaGradeGlobal(mouseX, mouseY, configuracoes, camera);
                    const auto resultado = AppServicos::aplicarFerramentaNoJogo(jogo, posicaoNaGrade);
                    if (sdl.audioInicializado) { Assets::tocarSomDaAcao(ativos, diretorioAssets, resultado); }
                }
            }
        }

        AppServicos::avancarTempoDoJogo(jogo, deltaTime);
        Camera::atualizarInerciaDaCamera(camera, configuracoes, jogo.tamanhoAtualDoGrid(), deltaTime);
        const auto posicaoRealcada = camera.panAtivo
            ? Geometria::PosicaoNaGrade{-1, -1}
            : Mundo::converterMouseParaGradeGlobal(mouseX, mouseY, configuracoes, camera);

        SDL_SetRenderDrawColor(renderizador.ponteiro, 0, 0, 0, 255);
        SDL_RenderClear(renderizador.ponteiro);
        Mundo::desenharFundo(renderizador.ponteiro, recursos.texturaFundo);
        Mundo::desenharGradeAtiva(renderizador.ponteiro, jogo, recursos.texturasCanteiro, configuracoes, camera, posicaoRealcada);
        Mundo::desenharLimiteDaGradeJogavel(renderizador.ponteiro, jogo, configuracoes, camera);
        Mundo::desenharPreviewDeCriacaoDeTerra(renderizador.ponteiro, jogo, configuracoes, camera, posicaoRealcada);
        UI::desenharInterface(renderizador.ponteiro, jogo.ferramentaSelecionada(), botoes.cursor, botoes.enxada, botoes.removerTerra, botoes.semente, botoes.presente, recursos.texturasDosBotoes);
        Cursores::desenharCursorCustomizado(renderizador.ponteiro, mouseX, mouseY, jogo.ferramentaSelecionada());
        SDL_RenderPresent(renderizador.ponteiro);

        const Uint32 duracaoDoQuadro = SDL_GetTicks() - inicioDoQuadro;
        if (duracaoDoQuadro < Constantes::MILISSEGUNDOS_POR_QUADRO) { SDL_Delay(Constantes::MILISSEGUNDOS_POR_QUADRO - duracaoDoQuadro); }
    }

    return 0;
}
