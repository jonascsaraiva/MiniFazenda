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
#include "Apresentacao/Interface/Loja/ControladorDaLoja.hpp"
#include "Apresentacao/Renderizacao/Cursores/CursorCustomizado.hpp"
#include "Apresentacao/Renderizacao/Mundo/DesenhoDoMundo.hpp"
#include "Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp"
#include "Apresentacao/Renderizacao/Mundo/RenderizadorDoPersonagem.hpp"
#include "Apresentacao/Renderizacao/UI/BarraDeFerramentasRenderer.hpp"
#include "Apresentacao/Renderizacao/UI/HudRenderer.hpp"
#include "Apresentacao/Renderizacao/UI/LojaRenderer.hpp"
#include "Apresentacao/Renderizacao/UI/TooltipDoCanteiroRenderer.hpp"
#include "Compartilhado/ConstantesDaJanela.hpp"
#include "Compartilhado/Geometria/Posicoes.hpp"
#include "Dominio/Ferramentas/TipoDeFerramenta.hpp"
#include "Dominio/Mapa/MapaDaFazenda.hpp"
#include "Dominio/Ocupacao/GridDeOcupacao.hpp"
#include "Dominio/Plantas/Planta.hpp"
#include "Infraestrutura/Assets/GerenciadorDeAtivosSDL.hpp"
#include "Infraestrutura/Assets/RecursosDaFazenda.hpp"
#include "Infraestrutura/Assets/RecursosDaLoja.hpp"
#include "Infraestrutura/Configuracao/LeitorDeConfiguracao.hpp"

#include <SDL.h>

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace MiniFazenda::Apresentacao::Cenas {

namespace AnimacaoPersonagem = MiniFazenda::Apresentacao::Animacao;
namespace AppServicos = MiniFazenda::Aplicacao::Servicos;
namespace Assets = MiniFazenda::Infraestrutura::Assets;
namespace BarraFerramentas = MiniFazenda::Apresentacao::Interface::BarraDeFerramentas;
namespace Camera = MiniFazenda::Apresentacao::Camera;
namespace Canteiros = MiniFazenda::Dominio::Canteiros;
namespace Configuracao = MiniFazenda::Infraestrutura::Configuracao;
namespace Constantes = MiniFazenda::Compartilhado::Constantes;
namespace Cursores = MiniFazenda::Apresentacao::Renderizacao::Cursores;
namespace Ferramentas = MiniFazenda::Dominio::Ferramentas;
namespace Geometria = MiniFazenda::Compartilhado::Geometria;
namespace HudRenderer = MiniFazenda::Apresentacao::Renderizacao::UI::HudRenderer;
namespace Interface = MiniFazenda::Apresentacao::Interface;
namespace LojaInterface = MiniFazenda::Apresentacao::Interface::Loja;
namespace LojaRenderer = MiniFazenda::Apresentacao::Renderizacao::UI::LojaRenderer;
namespace Mundo = MiniFazenda::Apresentacao::Renderizacao::Mundo;
namespace Ocupacao = MiniFazenda::Dominio::Ocupacao;
namespace TooltipRenderer = MiniFazenda::Apresentacao::Renderizacao::UI::TooltipDoCanteiroRenderer;
namespace UI = MiniFazenda::Apresentacao::Renderizacao::UI;

struct EstadoDoTooltipDoCanteiro {
    static constexpr float TEMPO_NECESSARIO_PARA_EXIBIR = 0.5f;

    float tempoAcumuladoDeHover = 0.0f;
    Geometria::PosicaoNaGradeDeOcupacao ultimaCelulaConsultada{-1, -1};
    Ocupacao::IdentificadorDeEntidadeDeMapa ultimoIdentificadorObservado =
        Ocupacao::IDENTIFICADOR_INVALIDO_DE_ENTIDADE_DE_MAPA;
    int cursorX = Constantes::LARGURA_DA_JANELA / 2;
    int cursorY = Constantes::ALTURA_DA_JANELA / 2;
    bool temAlvoValido = false;
    bool aptoParaExibicao = false;
    std::string texto;

    void reiniciar(Geometria::PosicaoNaGradeDeOcupacao celula, int novoCursorX, int novoCursorY) {
        tempoAcumuladoDeHover = 0.0f;
        ultimaCelulaConsultada = celula;
        ultimoIdentificadorObservado = Ocupacao::IDENTIFICADOR_INVALIDO_DE_ENTIDADE_DE_MAPA;
        cursorX = novoCursorX;
        cursorY = novoCursorY;
        temAlvoValido = false;
        aptoParaExibicao = false;
        texto.clear();
    }
};

class CenaFazenda {
public:
    CenaFazenda(
        SDL_Renderer* renderizador,
        Assets::GerenciadorDeAtivosSDL& ativos,
        std::filesystem::path diretorioAssets,
        ConfiguracoesDoLayout configuracoes,
        Assets::RecursosDaFazenda recursos,
        Assets::RecursosDaLoja recursosDaLoja,
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
          recursosDaLoja_(std::move(recursosDaLoja)),
          hud_(hud),
          botoes_(BarraFerramentas::criarBotoesDaInterface()),
          itensDeSementesDaLoja_(LojaInterface::criarItensDeSementesDaLoja(especiesDaLoja)),
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
        atualizarTooltipDoCanteiro(deltaTime);
    }

    void renderizar() {
        const Geometria::PosicaoNaGradeDeOcupacao posicaoRealcada = calcularPosicaoRealcada();

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
            botoes_,
            recursos_.texturasDosBotoes
        );

        HudRenderer::desenharStatusDoJogador(renderizador_, hud_.fonte, jogo_.jogador());
        HudRenderer::desenharBotaoConfiguracoes(renderizador_, hud_.iconeConfiguracoes, false);
        if (estadoDaCena_.painelConfiguracoesAberto()) {
            retBotaoSom_ =
                HudRenderer::desenharPainelConfiguracoes(renderizador_, hud_.fonte, estadoDaCena_);
        }

        if (estadoDaLoja_.aberta()) {
            const LojaInterface::LayoutCalculadoDaLoja layoutDaLoja = calcularLayoutAtualDaLoja();
            LojaRenderer::desenharLoja(
                renderizador_,
                hud_.fonte,
                estadoDaLoja_,
                layoutDaLoja,
                recursosDaLoja_.texturaFundo,
                itensDeSementesDaLoja_,
                recursos_.texturasDasSementes,
                jogo_.jogador().moedas(),
                jogo_.identificadorDaSementeSelecionada()
            );
        }

        if (!estadoDaLoja_.aberta() && estadoDoTooltipDoCanteiro_.aptoParaExibicao) {
            TooltipRenderer::desenharTooltipDoCanteiro(
                renderizador_,
                hud_.fonte,
                estadoDoTooltipDoCanteiro_.texto,
                estadoDoTooltipDoCanteiro_.cursorX,
                estadoDoTooltipDoCanteiro_.cursorY
            );
        }

        Cursores::desenharCursorCustomizado(renderizador_, mouseX_, mouseY_, jogo_.ferramentaSelecionada());
    }

private:
    static bool pontoDentroDoRetangulo(int x, int y, const SDL_Rect& retangulo) {
        return Interface::pontoEstaNoRetanguloSemiaberto(x, y, retangulo.x, retangulo.y, retangulo.w, retangulo.h);
    }

    static bool pontoDentroDaAreaDeInteracao(int x, int y, const Interface::AreaDeInteracao& area) {
        return Interface::pontoEstaNaAreaDeInteracao(x, y, area);
    }

    static bool posicaoEstaDentroDaAreaJogavel(
        Geometria::PosicaoNaGradeDeOcupacao posicao,
        int tamanhoAtualDoGrid
    ) {
        return Dominio::Mapa::MapaDaFazenda::posicaoEstaDentroDoMapaGlobal(posicao) &&
               Dominio::Mapa::MapaDaFazenda::posicaoDeOcupacaoEstaDentroDaAreaJogavel(
                   posicao,
                   tamanhoAtualDoGrid
               );
    }

    bool mouseEstaSobreInterface() const {
        if (estadoDaLoja_.aberta()) {
            return true;
        }

        if (estadoDaCena_.painelConfiguracoesAberto()) {
            return true;
        }

        if (pontoDentroDoRetangulo(mouseX_, mouseY_, HudRenderer::calcularAreaDoBotaoConfiguracoes())) {
            return true;
        }

        return BarraFerramentas::pontoEstaNaAreaDoHud(mouseX_, mouseY_, botoes_);
    }

    std::string construirTextoDoTooltipDoCanteiro(const Canteiros::Canteiro& canteiro) const {
        const std::optional<std::string> nomeDaPlanta = canteiro.nomeDaPlantaAtual();
        if (!nomeDaPlanta.has_value()) {
            return {};
        }

        switch (canteiro.estadoLogicoDaPlantaAtual()) {
            case Canteiros::EstadoLogicoDaPlantaNoCanteiro::Crescendo: {
                const std::optional<int> percentual = canteiro.percentualDeCrescimentoAteColheita();
                return *nomeDaPlanta + " - " + std::to_string(percentual.value_or(0)) + "%";
            }
            case Canteiros::EstadoLogicoDaPlantaNoCanteiro::ProntaParaColheita:
                return *nomeDaPlanta + " - Pronto";
            case Canteiros::EstadoLogicoDaPlantaNoCanteiro::Morta:
                return *nomeDaPlanta + " - Morta";
            case Canteiros::EstadoLogicoDaPlantaNoCanteiro::Ausente:
            default:
                return {};
        }
    }

    void atualizarAlvoDoTooltipDoCanteiro() {
        const Geometria::PosicaoNaGradeDeOcupacao posicaoDeOcupacao =
            Mundo::converterMouseParaGradeDeOcupacaoGlobal(mouseX_, mouseY_, configuracoes_, camera_);

        if (camera_.panAtivo || mouseEstaSobreInterface()) {
            estadoDoTooltipDoCanteiro_.reiniciar(posicaoDeOcupacao, mouseX_, mouseY_);
            return;
        }

        const Dominio::Mapa::EntidadeDoMapa* entidade = jogo_.mapa().entidadeEm(posicaoDeOcupacao);
        if (entidade == nullptr || !entidade->ehCanteiroAgricola()) {
            estadoDoTooltipDoCanteiro_.reiniciar(posicaoDeOcupacao, mouseX_, mouseY_);
            return;
        }

        const Canteiros::Canteiro* canteiro = entidade->canteiroAgricola();
        if (canteiro == nullptr || !canteiro->possuiPlanta()) {
            estadoDoTooltipDoCanteiro_.reiniciar(posicaoDeOcupacao, mouseX_, mouseY_);
            return;
        }

        std::string texto = construirTextoDoTooltipDoCanteiro(*canteiro);
        if (texto.empty()) {
            estadoDoTooltipDoCanteiro_.reiniciar(posicaoDeOcupacao, mouseX_, mouseY_);
            return;
        }

        const Ocupacao::IdentificadorDeEntidadeDeMapa identificador = entidade->identificador();
        if (!estadoDoTooltipDoCanteiro_.temAlvoValido ||
            estadoDoTooltipDoCanteiro_.ultimoIdentificadorObservado != identificador) {
            estadoDoTooltipDoCanteiro_.tempoAcumuladoDeHover = 0.0f;
            estadoDoTooltipDoCanteiro_.aptoParaExibicao = false;
        }

        estadoDoTooltipDoCanteiro_.ultimaCelulaConsultada = posicaoDeOcupacao;
        estadoDoTooltipDoCanteiro_.ultimoIdentificadorObservado = identificador;
        estadoDoTooltipDoCanteiro_.cursorX = mouseX_;
        estadoDoTooltipDoCanteiro_.cursorY = mouseY_;
        estadoDoTooltipDoCanteiro_.temAlvoValido = true;
        estadoDoTooltipDoCanteiro_.texto = std::move(texto);
    }

    void atualizarTooltipDoCanteiro(float deltaTime) {
        atualizarAlvoDoTooltipDoCanteiro();

        if (!estadoDoTooltipDoCanteiro_.temAlvoValido) {
            return;
        }

        estadoDoTooltipDoCanteiro_.tempoAcumuladoDeHover += deltaTime;
        estadoDoTooltipDoCanteiro_.aptoParaExibicao =
            estadoDoTooltipDoCanteiro_.tempoAcumuladoDeHover >=
            EstadoDoTooltipDoCanteiro::TEMPO_NECESSARIO_PARA_EXIBIR;
    }

    void processarMovimentoDoMouse(const SDL_MouseMotionEvent& movimento) {
        mouseX_ = movimento.x;
        mouseY_ = movimento.y;

        if (estadoDaLoja_.aberta()) {
            atualizarHoverDaLoja();
            return;
        }

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

        atualizarAlvoDoTooltipDoCanteiro();
    }

    void processarScrollDoMouse(const SDL_MouseWheelEvent& scroll) {
        int mouseXNoScroll = 0;
        int mouseYNoScroll = 0;
        SDL_GetMouseState(&mouseXNoScroll, &mouseYNoScroll);
        mouseX_ = mouseXNoScroll;
        mouseY_ = mouseYNoScroll;

        if (estadoDaLoja_.aberta()) {
            atualizarHoverDaLoja();
            return;
        }

        Camera::aplicarZoomNoPonto(
            camera_,
            configuracoes_,
            jogo_.tamanhoAtualDoGrid(),
            mouseXNoScroll,
            mouseYNoScroll,
            scroll.y
        );
        atualizarAlvoDoTooltipDoCanteiro();
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
        mouseX_ = clique.x;
        mouseY_ = clique.y;

        if (estadoDaLoja_.aberta()) {
            if (clique.button == SDL_BUTTON_LEFT) {
                processarCliqueEsquerdo(clique.x, clique.y);
            }
            return;
        }

        if ((clique.button == SDL_BUTTON_MIDDLE || clique.button == SDL_BUTTON_RIGHT) &&
            BarraFerramentas::pontoEstaNaAreaDoHud(clique.x, clique.y, botoes_)) {
            return;
        }

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

        if (estadoDaLoja_.aberta()) {
            processarCliqueNaLoja();
            return;
        }

        if (pontoDentroDoRetangulo(mouseX_, mouseY_, HudRenderer::calcularAreaDoBotaoConfiguracoes())) {
            estadoDaCena_.alternarPainelConfiguracoes();
            return;
        }

        if (estadoDaCena_.painelConfiguracoesAberto()) {
            processarCliqueNoPainelConfiguracoes();
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
        const LojaInterface::LayoutCalculadoDaLoja layoutDaLoja = calcularLayoutAtualDaLoja();
        const LojaInterface::ResultadoDoCliqueDaLoja resultado =
            LojaInterface::processarCliqueDaLoja(mouseX_, mouseY_, estadoDaLoja_, layoutDaLoja);

        if (!resultado.cliqueConsumido) {
            return false;
        }

        if (resultado.identificadorDaSementeSelecionada.has_value()) {
            jogo_.selecionarSemente(*resultado.identificadorDaSementeSelecionada);
            jogo_.selecionarFerramenta(Ferramentas::TipoDeFerramenta::Semente);
        }

        if (resultado.acao != LojaInterface::AcaoDoCliqueDaLoja::ConsumirClique &&
            resultado.acao != LojaInterface::AcaoDoCliqueDaLoja::Nenhuma) {
            tocarSomDeCliqueDaInterface();
        }

        return true;
    }

    bool processarCliqueNaBarraDeFerramentas() {
        auto ferramentaSelecionada = jogo_.ferramentaSelecionada();
        const BarraFerramentas::ResultadoDoCliqueNaInterface resultadoDoClique =
            BarraFerramentas::processarCliqueNaInterface(
                mouseX_,
                mouseY_,
                botoes_,
                ferramentaSelecionada
            );
        if (!resultadoDoClique.cliqueConsumido) {
            return false;
        }

        switch (resultadoDoClique.acao) {
            case BarraFerramentas::AcaoDoCliqueNaInterface::SelecionarFerramenta:
                jogo_.selecionarFerramenta(ferramentaSelecionada);
                tocarSomDeCliqueDaInterface();
                break;
            case BarraFerramentas::AcaoDoCliqueNaInterface::AbrirLoja:
                jogo_.selecionarFerramenta(ferramentaSelecionada);
                estadoDaCena_.fecharPainelConfiguracoes();
                estadoDaLoja_.abrir();
                atualizarHoverDaLoja();
                tocarSomDeCliqueDaInterface();
                break;
            case BarraFerramentas::AcaoDoCliqueNaInterface::AumentarZoom:
                Camera::aplicarZoomNoPonto(
                    camera_,
                    configuracoes_,
                    jogo_.tamanhoAtualDoGrid(),
                    Constantes::LARGURA_DA_JANELA / 2,
                    Constantes::ALTURA_DA_JANELA / 2,
                    1
                );
                tocarSomDeCliqueDaInterface();
                break;
            case BarraFerramentas::AcaoDoCliqueNaInterface::DiminuirZoom:
                Camera::aplicarZoomNoPonto(
                    camera_,
                    configuracoes_,
                    jogo_.tamanhoAtualDoGrid(),
                    Constantes::LARGURA_DA_JANELA / 2,
                    Constantes::ALTURA_DA_JANELA / 2,
                    -1
                );
                tocarSomDeCliqueDaInterface();
                break;
            case BarraFerramentas::AcaoDoCliqueNaInterface::ConsumirClique:
            case BarraFerramentas::AcaoDoCliqueNaInterface::Nenhuma:
                break;
        }

        return true;
    }

    void processarCliqueNoMundo() {
        const Geometria::PosicaoNaGradeDeOcupacao posicaoDeOcupacao =
            Mundo::converterMouseParaGradeDeOcupacaoGlobal(mouseX_, mouseY_, configuracoes_, camera_);

        if (posicaoEstaDentroDaAreaJogavel(posicaoDeOcupacao, jogo_.tamanhoAtualDoGrid())) {
            jogo_.personagem().caminharAte(posicaoDeOcupacao);
        }

        const auto resultado = AppServicos::aplicarFerramentaNoJogo(jogo_, posicaoDeOcupacao);
        if (audioInicializado_) {
            Assets::tocarSomDaAcao(ativos_, diretorioAssets_, resultado);
        }
    }

    Geometria::PosicaoNaGradeDeOcupacao calcularPosicaoRealcada() const {
        if (camera_.panAtivo || mouseEstaSobreInterface()) {
            return Geometria::PosicaoNaGradeDeOcupacao{-1, -1};
        }

        return Mundo::converterMouseParaGradeDeOcupacaoGlobal(mouseX_, mouseY_, configuracoes_, camera_);
    }

    void recarregarConfiguracoesVisuais() {
        Configuracao::carregarConfiguracoesDoLayout(diretorioAssets_ / "config.ini", configuracoes_);
        Camera::aplicarOrigemCentradaDaGrade(configuracoes_, jogo_.tamanhoAtualDoGrid());
        Camera::limitarPanAosLimitesDoGrid(camera_, configuracoes_, jogo_.tamanhoAtualDoGrid());
        recursos_.texturaFundo = Assets::carregarTexturaDeFundoPrincipal(ativos_, diretorioAssets_, configuracoes_);
        recursosDaLoja_ = Assets::carregarRecursosDaLoja(ativos_, diretorioAssets_);
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

    LojaInterface::LayoutCalculadoDaLoja calcularLayoutAtualDaLoja() const {
        return LojaInterface::calcularLayoutDaLoja(estadoDaLoja_, itensDeSementesDaLoja_);
    }

    void atualizarHoverDaLoja() {
        const LojaInterface::LayoutCalculadoDaLoja layoutDaLoja = calcularLayoutAtualDaLoja();
        LojaInterface::atualizarHoverDaLoja(mouseX_, mouseY_, estadoDaLoja_, layoutDaLoja);
    }

    SDL_Renderer* renderizador_ = nullptr;
    Assets::GerenciadorDeAtivosSDL& ativos_;
    std::filesystem::path diretorioAssets_;
    ConfiguracoesDoLayout configuracoes_;
    Aplicacao::Estado::EstadoDoJogo jogo_;
    Interface::EstadoDaCenaFazenda estadoDaCena_;
    EstadoDoTooltipDoCanteiro estadoDoTooltipDoCanteiro_;
    AnimacaoPersonagem::EstadoVisualDoPersonagem estadoVisualDoPersonagem_;
    Assets::RecursosDaFazenda recursos_;
    Assets::RecursosDaLoja recursosDaLoja_;
    Assets::RecursosDeHud hud_;
    Camera::EstadoDaCamera camera_;
    BarraFerramentas::BotoesDaInterface botoes_;
    LojaInterface::EstadoDaLoja estadoDaLoja_;
    std::vector<LojaInterface::ItemDeSementeDaLoja> itensDeSementesDaLoja_;
    bool audioInicializado_ = false;
    bool executando_ = true;
    int mouseX_ = Constantes::LARGURA_DA_JANELA / 2;
    int mouseY_ = Constantes::ALTURA_DA_JANELA / 2;
    SDL_Rect retBotaoSom_{0, 0, 0, 0};
};

} // namespace MiniFazenda::Apresentacao::Cenas
