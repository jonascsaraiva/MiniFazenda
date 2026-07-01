#pragma once

#include "Aplicacao/Estado/EstadoDoJogo.hpp"
#include "Aplicacao/Servicos/InicializadorDaFazenda.hpp"
#include "Aplicacao/Servicos/ServicoDeFerramentas.hpp"
#include "Aplicacao/Servicos/ServicoDeTempo.hpp"
#include "Apresentacao/Animacao/AnimadorDoPersonagem.hpp"
#include "Apresentacao/Camera/CameraDoJogo.hpp"
#include "Apresentacao/ConfiguracoesDoLayout.hpp"
#include "Apresentacao/Interface/BarraDeFerramentas/BarraDeFerramentas.hpp"
#include "Apresentacao/Interface/EstadoDaCenaFazenda.hpp"
#include "Apresentacao/Renderizacao/Cursores/CursorCustomizado.hpp"
#include "Apresentacao/Renderizacao/Mundo/DesenhoDoMundo.hpp"
#include "Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp"
#include "Apresentacao/Renderizacao/Mundo/RenderizadorDoPersonagem.hpp"
#include "Apresentacao/Renderizacao/UI/BarraDeFerramentasRenderer.hpp"
#include "Apresentacao/Renderizacao/UI/HudRenderer.hpp"
#include "Compartilhado/Constantes.hpp"
#include "Compartilhado/Geometria/Posicoes.hpp"
#include "Dominio/Ferramentas/TipoDeFerramenta.hpp"
#include "Dominio/Grade/GradeGlobalDeCanteiros.hpp"
#include "Dominio/Plantas/Planta.hpp"
#include "Infraestrutura/Assets/GerenciadorDeAtivosSDL.hpp"
#include "Infraestrutura/Assets/RecursosDaFazenda.hpp"
#include "Infraestrutura/Configuracao/LeitorDeConfiguracao.hpp"

#include <SDL.h>

#include <filesystem>
#include <memory>
#include <utility>
#include <vector>

namespace MiniFazenda::Apresentacao::Cenas {

namespace AnimacaoPersonagem = MiniFazenda::Apresentacao::Animacao;
namespace AppServicos = MiniFazenda::Aplicacao::Servicos;
namespace Assets = MiniFazenda::Infraestrutura::Assets;
namespace BarraFerramentas = MiniFazenda::Apresentacao::Interface::BarraDeFerramentas;
namespace Camera = MiniFazenda::Apresentacao::Camera;
namespace Configuracao = MiniFazenda::Infraestrutura::Configuracao;
namespace Constantes = MiniFazenda::Compartilhado::Constantes;
namespace Cursores = MiniFazenda::Apresentacao::Renderizacao::Cursores;
namespace Ferramentas = MiniFazenda::Dominio::Ferramentas;
namespace Geometria = MiniFazenda::Compartilhado::Geometria;
namespace HudRenderer = MiniFazenda::Apresentacao::Renderizacao::UI::HudRenderer;
namespace Interface = MiniFazenda::Apresentacao::Interface;
namespace Mundo = MiniFazenda::Apresentacao::Renderizacao::Mundo;
namespace UI = MiniFazenda::Apresentacao::Renderizacao::UI;

class CenaFazenda {
public:
    CenaFazenda(
        SDL_Renderer* renderizador,
        Assets::GerenciadorDeAtivosSDL& ativos,
        std::filesystem::path diretorioAssets,
        ConfiguracoesDoLayout configuracoes,
        Assets::RecursosDaFazenda recursos,
        Assets::RecursosDeHud hud,
        const std::vector<std::unique_ptr<Dominio::Plantas::Planta>>& especiesDaLoja,
        bool audioInicializado
    )
        : renderizador_(renderizador),
          ativos_(ativos),
          diretorioAssets_(std::move(diretorioAssets)),
          configuracoes_(configuracoes),
          jogo_(AppServicos::criarEstadoInicialDoJogo()),
          recursos_(std::move(recursos)),
          hud_(hud),
          botoes_(BarraFerramentas::criarBotoesDaInterface()),
          painelDaLoja_(BarraFerramentas::criarPainelDaLoja(botoes_, especiesDaLoja)),
          audioInicializado_(audioInicializado) {
        Camera::aplicarOrigemCentradaDaGrade(configuracoes_, jogo_.tamanhoAtualDoGrid());
    }

    bool deveContinuar() const {
        return executando_;
    }

    void processarEvento(const SDL_Event& evento) {
        switch (evento.type) {
            case SDL_QUIT:
                executando_ = false;
                break;
            case SDL_MOUSEMOTION:
                processarMovimentoDoMouse(evento.motion);
                break;
            case SDL_MOUSEBUTTONUP:
                Camera::finalizarPanDaCamera(camera_, evento.button.button);
                break;
            case SDL_MOUSEWHEEL:
                processarScrollDoMouse(evento.wheel);
                break;
            case SDL_KEYDOWN:
                processarTeclaPressionada(evento.key);
                break;
            case SDL_MOUSEBUTTONDOWN:
                processarCliqueDoMouse(evento.button);
                break;
            default:
                break;
        }
    }

    void atualizar(float deltaTime) {
        AppServicos::avancarTempoDoJogo(jogo_, deltaTime);
        AnimacaoPersonagem::avancarAnimacaoDoPersonagem(estadoVisualDoPersonagem_, jogo_.personagem(), deltaTime);
        Camera::atualizarInerciaDaCamera(camera_, configuracoes_, jogo_.tamanhoAtualDoGrid(), deltaTime);
    }

    void renderizar() {
        const Geometria::PosicaoNaGrade posicaoRealcada = calcularPosicaoRealcada();

        Mundo::desenharFundo(renderizador_, recursos_.texturaFundo);
        Mundo::desenharGradeAtiva(
            renderizador_,
            jogo_,
            recursos_.texturasCanteiro,
            configuracoes_,
            camera_,
            posicaoRealcada
        );
        Mundo::desenharPersonagem(
            renderizador_,
            recursos_.texturasDoPersonagem,
            estadoVisualDoPersonagem_,
            jogo_.personagem(),
            configuracoes_,
            camera_
        );
        Mundo::desenharLimiteDaGradeJogavel(renderizador_, jogo_, configuracoes_, camera_);
        Mundo::desenharPreviewDeCriacaoDeTerra(renderizador_, jogo_, configuracoes_, camera_, posicaoRealcada);
        UI::desenharInterface(
            renderizador_,
            jogo_.ferramentaSelecionada(),
            botoes_.cursor,
            botoes_.enxada,
            botoes_.removerTerra,
            botoes_.semente,
            botoes_.loja,
            recursos_.texturasDosBotoes
        );

        if (estadoDaCena_.painelDaLojaAberto()) {
            UI::desenharPainelDaLoja(
                renderizador_,
                painelDaLoja_,
                recursos_.texturasDasSementes,
                jogo_.identificadorDaSementeSelecionada()
            );
        }

        HudRenderer::desenharStatusDoJogador(renderizador_, hud_.fonte, jogo_.jogador());
        HudRenderer::desenharBotaoConfiguracoes(renderizador_, hud_.iconeConfiguracoes, false);
        if (estadoDaCena_.painelConfiguracoesAberto()) {
            retBotaoSom_ =
                HudRenderer::desenharPainelConfiguracoes(renderizador_, hud_.fonte, estadoDaCena_);
        }

        Cursores::desenharCursorCustomizado(renderizador_, mouseX_, mouseY_, jogo_.ferramentaSelecionada());
    }

private:
    static bool pontoDentroDoRetangulo(int x, int y, const SDL_Rect& retangulo) {
        const SDL_Point ponto{x, y};
        return SDL_PointInRect(&ponto, &retangulo) == SDL_TRUE;
    }

    static bool pontoDentroDaAreaDeInteracao(int x, int y, const Interface::AreaDeInteracao& area) {
        return BarraFerramentas::verificarCliqueNoBotao(x, y, area);
    }

    static bool posicaoEstaDentroDaAreaJogavel(
        Geometria::PosicaoNaGrade posicao,
        int tamanhoAtualDoGrid
    ) {
        return Dominio::Grade::GradeGlobalDeCanteiros::posicaoEstaDentroDaGradeGlobal(posicao) &&
               Dominio::Grade::GradeGlobalDeCanteiros::posicaoEstaDentroDaGradeAtual(posicao, tamanhoAtualDoGrid);
    }

    void processarMovimentoDoMouse(const SDL_MouseMotionEvent& movimento) {
        mouseX_ = movimento.x;
        mouseY_ = movimento.y;

        if (camera_.panAtivo) {
            Camera::moverPanDaCamera(
                camera_,
                configuracoes_,
                jogo_.tamanhoAtualDoGrid(),
                movimento.xrel,
                movimento.yrel,
                movimento.timestamp
            );
        }
    }

    void processarScrollDoMouse(const SDL_MouseWheelEvent& scroll) {
        int mouseXNoScroll = 0;
        int mouseYNoScroll = 0;
        SDL_GetMouseState(&mouseXNoScroll, &mouseYNoScroll);
        Camera::aplicarZoomNoPonto(
            camera_,
            configuracoes_,
            jogo_.tamanhoAtualDoGrid(),
            mouseXNoScroll,
            mouseYNoScroll,
            scroll.y
        );
    }

    void processarTeclaPressionada(const SDL_KeyboardEvent& tecla) {
        if (tecla.keysym.sym == SDLK_ESCAPE) {
            executando_ = false;
        }

        if (tecla.keysym.sym == SDLK_F5) {
            recarregarConfiguracoesVisuais();
        }

        if (tecla.keysym.sym == SDLK_HOME) {
            centralizarCamera();
        }
    }

    void processarCliqueDoMouse(const SDL_MouseButtonEvent& clique) {
        if (clique.button == SDL_BUTTON_MIDDLE || clique.button == SDL_BUTTON_RIGHT) {
            Camera::iniciarPanDaCamera(camera_, clique.button, clique.timestamp);
            return;
        }

        if (clique.button == SDL_BUTTON_LEFT) {
            processarCliqueEsquerdo(clique.x, clique.y);
        }
    }

    void processarCliqueEsquerdo(int x, int y) {
        if (camera_.panAtivo) {
            return;
        }

        mouseX_ = x;
        mouseY_ = y;

        if (pontoDentroDoRetangulo(mouseX_, mouseY_, HudRenderer::calcularAreaDoBotaoConfiguracoes())) {
            estadoDaCena_.alternarPainelConfiguracoes();
            return;
        }

        if (estadoDaCena_.painelConfiguracoesAberto()) {
            processarCliqueNoPainelConfiguracoes();
            return;
        }

        if (estadoDaCena_.painelDaLojaAberto() && processarCliqueNaLoja()) {
            return;
        }

        if (processarCliqueNaBarraDeFerramentas()) {
            return;
        }

        processarCliqueNoMundo();
    }

    void processarCliqueNoPainelConfiguracoes() {
        if (pontoDentroDoRetangulo(mouseX_, mouseY_, retBotaoSom_)) {
            estadoDaCena_.alternarAudioMutado();
            if (audioInicializado_) {
                ativos_.definirAudioMutado(estadoDaCena_.audioMutado());
            }
            return;
        }

        if (!pontoDentroDoRetangulo(mouseX_, mouseY_, HudRenderer::calcularAreaDoPainelConfiguracoes())) {
            estadoDaCena_.fecharPainelConfiguracoes();
        }
    }

    bool processarCliqueNaLoja() {
        const auto sementeClicada =
            BarraFerramentas::sementeClicadaNoPainelDaLoja(mouseX_, mouseY_, painelDaLoja_);
        if (sementeClicada.has_value()) {
            jogo_.selecionarSemente(*sementeClicada);
            jogo_.selecionarFerramenta(Ferramentas::TipoDeFerramenta::Semente);
            estadoDaCena_.fecharPainelDaLoja();
            tocarSomDeCliqueDaInterface();
            return true;
        }

        return pontoDentroDaAreaDeInteracao(mouseX_, mouseY_, painelDaLoja_.fundo);
    }

    bool processarCliqueNaBarraDeFerramentas() {
        auto ferramentaSelecionada = jogo_.ferramentaSelecionada();
        if (!BarraFerramentas::processarCliqueNaInterface(
                mouseX_,
                mouseY_,
                botoes_,
                ferramentaSelecionada,
                estadoDaCena_
            )) {
            return false;
        }

        jogo_.selecionarFerramenta(ferramentaSelecionada);
        tocarSomDeCliqueDaInterface();
        return true;
    }

    void processarCliqueNoMundo() {
        const Geometria::PosicaoNaGrade posicaoNaGrade =
            Mundo::converterMouseParaGradeGlobal(mouseX_, mouseY_, configuracoes_, camera_);
        if (posicaoEstaDentroDaAreaJogavel(posicaoNaGrade, jogo_.tamanhoAtualDoGrid())) {
            jogo_.personagem().caminharAte(posicaoNaGrade);
        }

        const auto resultado = AppServicos::aplicarFerramentaNoJogo(jogo_, posicaoNaGrade);
        if (audioInicializado_) {
            Assets::tocarSomDaAcao(ativos_, diretorioAssets_, resultado);
        }
    }

    Geometria::PosicaoNaGrade calcularPosicaoRealcada() const {
        if (camera_.panAtivo) {
            return Geometria::PosicaoNaGrade{-1, -1};
        }

        return Mundo::converterMouseParaGradeGlobal(mouseX_, mouseY_, configuracoes_, camera_);
    }

    void recarregarConfiguracoesVisuais() {
        Configuracao::carregarConfiguracoesDoLayout(diretorioAssets_ / "config.ini", configuracoes_);
        Camera::aplicarOrigemCentradaDaGrade(configuracoes_, jogo_.tamanhoAtualDoGrid());
        Camera::limitarPanAosLimitesDoGrid(camera_, configuracoes_, jogo_.tamanhoAtualDoGrid());
        recursos_.texturaFundo = Assets::carregarTexturaDeFundoPrincipal(ativos_, diretorioAssets_, configuracoes_);
    }

    void centralizarCamera() {
        Camera::centralizarCamera(camera_);
        Camera::aplicarOrigemCentradaDaGrade(configuracoes_, jogo_.tamanhoAtualDoGrid());
    }

    void tocarSomDeCliqueDaInterface() {
        if (audioInicializado_) {
            ativos_.tocarSom(Assets::caminhoDoSomDeCliqueDaInterface(diretorioAssets_));
        }
    }

    SDL_Renderer* renderizador_ = nullptr;
    Assets::GerenciadorDeAtivosSDL& ativos_;
    std::filesystem::path diretorioAssets_;
    ConfiguracoesDoLayout configuracoes_;
    Aplicacao::Estado::EstadoDoJogo jogo_;
    Interface::EstadoDaCenaFazenda estadoDaCena_;
    AnimacaoPersonagem::EstadoVisualDoPersonagem estadoVisualDoPersonagem_;
    Assets::RecursosDaFazenda recursos_;
    Assets::RecursosDeHud hud_;
    Camera::EstadoDaCamera camera_;
    BarraFerramentas::BotoesDaInterface botoes_;
    BarraFerramentas::PainelDaLoja painelDaLoja_;
    bool audioInicializado_ = false;
    bool executando_ = true;
    int mouseX_ = Constantes::LARGURA_DA_JANELA / 2;
    int mouseY_ = Constantes::ALTURA_DA_JANELA / 2;
    SDL_Rect retBotaoSom_{0, 0, 0, 0};
};

} // namespace MiniFazenda::Apresentacao::Cenas
