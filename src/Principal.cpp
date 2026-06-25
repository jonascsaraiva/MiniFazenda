#include "Ativos.hpp"
#include "CaminhosDosAssets.hpp"
#include "Configuracao.hpp"
#include "Constantes.hpp"
#include "Desenho.hpp"
#include "GradeDeCanteiros.hpp"
#include "InterfaceDoJogo.hpp"
#include "Isometrico.hpp"
#include "RecursosDoJogo.hpp"
#include "SistemasDoJogo.hpp"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include <filesystem>
#include <iostream>

namespace {

struct InicializacaoSDL {
    bool sdlInicializado = false;
    bool imagemInicializada = false;
    bool fontesInicializadas = false;
    bool audioInicializado = false;

    bool inicializar() {
        SDL_SetMainReady();

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
            std::cerr << "Falha ao inicializar SDL2: " << SDL_GetError() << '\n';
            return false;
        }
        sdlInicializado = true;

        const int formatosImagem = IMG_INIT_PNG;
        if ((IMG_Init(formatosImagem) & formatosImagem) != formatosImagem) {
            std::cerr << "Falha ao inicializar SDL2_image: " << IMG_GetError() << '\n';
            return false;
        }
        imagemInicializada = true;

        if (TTF_Init() != 0) {
            std::cerr << "Falha ao inicializar SDL2_ttf: " << TTF_GetError() << '\n';
            return false;
        }
        fontesInicializadas = true;

        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == 0) {
            audioInicializado = true;
        } else {
            std::cerr << "Audio indisponivel: " << Mix_GetError() << '\n';
        }

        return true;
    }

    ~InicializacaoSDL() {
        if (audioInicializado) {
            Mix_CloseAudio();
        }

        if (fontesInicializadas) {
            TTF_Quit();
        }

        if (imagemInicializada) {
            IMG_Quit();
        }

        if (sdlInicializado) {
            SDL_Quit();
        }
    }
};

struct JanelaSDL {
    SDL_Window* ponteiro = nullptr;

    explicit JanelaSDL(SDL_Window* janela)
        : ponteiro(janela) {
    }

    JanelaSDL(const JanelaSDL&) = delete;
    JanelaSDL& operator=(const JanelaSDL&) = delete;

    ~JanelaSDL() {
        if (ponteiro != nullptr) {
            SDL_DestroyWindow(ponteiro);
        }
    }
};

struct RenderizadorSDL {
    SDL_Renderer* ponteiro = nullptr;

    explicit RenderizadorSDL(SDL_Renderer* renderizador)
        : ponteiro(renderizador) {
    }

    RenderizadorSDL(const RenderizadorSDL&) = delete;
    RenderizadorSDL& operator=(const RenderizadorSDL&) = delete;

    ~RenderizadorSDL() {
        if (ponteiro != nullptr) {
            SDL_DestroyRenderer(ponteiro);
        }
    }
};

struct CursorOculto {
    CursorOculto() {
        SDL_ShowCursor(SDL_DISABLE);
    }

    ~CursorOculto() {
        SDL_ShowCursor(SDL_ENABLE);
    }
};

SDL_Renderer* criarRenderizador(SDL_Window* janela) {
    SDL_Renderer* renderizador = SDL_CreateRenderer(
        janela,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (renderizador == nullptr) {
        renderizador = SDL_CreateRenderer(janela, -1, SDL_RENDERER_SOFTWARE);
    }

    return renderizador;
}

PosicaoNaGrade converterMouseParaGradeGlobal(
    int mouseX,
    int mouseY,
    const ConfiguracoesDoLayout& configuracoes,
    const EstadoDaCamera& camera
) {
    const DimensoesDoCanteiroRenderizado dimensoes = calcularDimensoesDoCanteiroRenderizado(camera.zoomAtual);

    return converterTelaParaGradeGlobal(
        mouseX,
        mouseY,
        dimensoes.largura,
        dimensoes.altura,
        configuracoes.origemGradeHorizontal,
        configuracoes.origemGradeVertical,
        camera.offsetHorizontal,
        camera.offsetVertical
    );
}

SDL_Rect calcularDestinoDoCanteiro(
    PosicaoNaGrade posicao,
    const ConfiguracoesDoLayout& configuracoes,
    const EstadoDaCamera& camera
) {
    const DimensoesDoCanteiroRenderizado dimensoes = calcularDimensoesDoCanteiroRenderizado(camera.zoomAtual);
    const PosicaoNaTela posicaoNaTela = converterGradeGlobalParaTela(
        posicao,
        dimensoes.largura,
        dimensoes.altura,
        configuracoes.origemGradeHorizontal,
        configuracoes.origemGradeVertical,
        camera.offsetHorizontal,
        camera.offsetVertical
    );

    return SDL_Rect{
        posicaoNaTela.coordenadaHorizontal,
        posicaoNaTela.coordenadaVertical,
        dimensoes.largura,
        dimensoes.altura
    };
}

bool retanguloApareceNaTela(SDL_Rect retangulo) {
    return retangulo.x < Constantes::LARGURA_DA_JANELA &&
           retangulo.x + retangulo.w > 0 &&
           retangulo.y < Constantes::ALTURA_DA_JANELA &&
           retangulo.y + retangulo.h > 0;
}

void desenharGradeAtiva(
    SDL_Renderer* renderizador,
    const EstadoDoJogo& jogo,
    const TexturasDosCanteiros& texturasCanteiro,
    const ConfiguracoesDoLayout& configuracoes,
    PosicaoNaGrade posicaoRealcada
) {
    for (const PosicaoNaGrade& posicaoDoTile : jogo.grade.posicoesDeTilesExistentes) {
        if (!Grade::posicaoEstaDentroDaGradeAtual(posicaoDoTile, jogo.tamanhoAtualDoGrid)) {
            continue;
        }

        const TileDeTerra* tile = Grade::obterTile(jogo.grade, posicaoDoTile);
        if (tile == nullptr || !tile->existeNoMapa) {
            continue;
        }

        const SDL_Rect destino = calcularDestinoDoCanteiro(posicaoDoTile, configuracoes, jogo.camera);
        if (!retanguloApareceNaTela(destino)) {
            continue;
        }

        Desenho::desenharCanteiro(
            renderizador,
            texturasCanteiro.paraEstado(tile->canteiro.estadoVisualAtual),
            tile->canteiro,
            destino,
            posicoesDaGradeSaoIguais(posicaoRealcada, posicaoDoTile)
        );
    }
}

void desenharPreviewDeCriacaoDeTerra(
    SDL_Renderer* renderizador,
    const EstadoDoJogo& jogo,
    const ConfiguracoesDoLayout& configuracoes,
    PosicaoNaGrade posicaoRealcada
) {
    const TileDeTerra* tileRealcado = Grade::obterTile(jogo.grade, posicaoRealcada);
    if (jogo.ferramentaSelecionada != FERRAMENTA_ENXADA ||
        !Grade::posicaoEstaDentroDaGradeGlobal(posicaoRealcada) ||
        !Grade::posicaoEstaDentroDaGradeAtual(posicaoRealcada, jogo.tamanhoAtualDoGrid) ||
        (tileRealcado != nullptr && tileRealcado->existeNoMapa)) {
        return;
    }

    const SDL_Rect destino = calcularDestinoDoCanteiro(posicaoRealcada, configuracoes, jogo.camera);
    if (!retanguloApareceNaTela(destino)) {
        return;
    }

    Desenho::desenharContornoLosango(renderizador, destino, SDL_Color{255, 244, 169, 160});
}

void desenharLimiteDaGradeJogavel(
    SDL_Renderer* renderizador,
    const EstadoDoJogo& jogo,
    const ConfiguracoesDoLayout& configuracoes
) {
    const RetanguloDeGradeRenderizada retangulo = calcularRetanguloDaGradeRenderizada(
        configuracoes,
        jogo.camera,
        jogo.tamanhoAtualDoGrid
    );

    SDL_Rect destino{
        retangulo.x,
        retangulo.y,
        retangulo.largura,
        retangulo.altura
    };

    if (retanguloApareceNaTela(destino)) {
        Desenho::desenharContornoLosango(renderizador, destino, SDL_Color{255, 244, 169, 160});
    }
}

void tocarSomDaAcaoSePossivel(
    bool audioInicializado,
    GerenciadorDeAtivos& ativos,
    const std::filesystem::path& diretorioAssets,
    ResultadoDaFerramenta resultado
) {
    if (audioInicializado) {
        tocarSomDaAcao(ativos, diretorioAssets, resultado);
    }
}

} // namespace

int main(int, char**) {
    InicializacaoSDL sdl;
    if (!sdl.inicializar()) {
        return 1;
    }

    JanelaSDL janela(SDL_CreateWindow(
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

    RenderizadorSDL renderizador(criarRenderizador(janela.ponteiro));
    if (renderizador.ponteiro == nullptr) {
        std::cerr << "Falha ao criar renderizador: " << SDL_GetError() << '\n';
        return 1;
    }

    SDL_SetRenderDrawBlendMode(renderizador.ponteiro, SDL_BLENDMODE_BLEND);
    CursorOculto cursorOculto;

    const std::filesystem::path diretorioAssets = CaminhosDosAssets::localizarDiretorioDeAssets();
    ConfiguracoesDoLayout configuracoes;
    Configuracao::carregarConfiguracoesDoLayout(diretorioAssets / "config.ini", configuracoes);

    GerenciadorDeAtivos ativos(renderizador.ponteiro);
    SDL_Texture* texturaFundo = carregarTexturaDeFundoPrincipal(ativos, diretorioAssets, configuracoes);
    const TexturasDosCanteiros texturasCanteiro = carregarTexturasDosCanteiros(ativos, diretorioAssets);

    if (sdl.audioInicializado) {
        ativos.tocarMusica(caminhoDaMusicaAmbiente(diretorioAssets));
    }

    EstadoDoJogo jogo = criarEstadoInicialDoJogo();
    aplicarOrigemCentradaDaGrade(configuracoes, jogo.tamanhoAtualDoGrid);
    const BotoesDaInterface botoes = criarBotoesDaInterface();
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
            if (evento.type == SDL_QUIT) {
                executando = false;
            }

            if (evento.type == SDL_MOUSEMOTION) {
                mouseX = evento.motion.x;
                mouseY = evento.motion.y;

                if (jogo.camera.panAtivo) {
                    moverPanDaCamera(
                        jogo.camera,
                        configuracoes,
                        jogo.tamanhoAtualDoGrid,
                        evento.motion.xrel,
                        evento.motion.yrel,
                        evento.motion.timestamp
                    );
                }
            }

            if (evento.type == SDL_MOUSEBUTTONDOWN &&
                (evento.button.button == SDL_BUTTON_MIDDLE || evento.button.button == SDL_BUTTON_RIGHT)) {
                iniciarPanDaCamera(jogo.camera, evento.button.button, evento.button.timestamp);
                continue;
            }

            if (evento.type == SDL_MOUSEBUTTONUP) {
                finalizarPanDaCamera(jogo.camera, evento.button.button);
            }

            if (evento.type == SDL_MOUSEWHEEL) {
                int mouseXNoScroll = 0;
                int mouseYNoScroll = 0;
                SDL_GetMouseState(&mouseXNoScroll, &mouseYNoScroll);
                aplicarZoomNoPonto(
                    jogo.camera,
                    configuracoes,
                    jogo.tamanhoAtualDoGrid,
                    mouseXNoScroll,
                    mouseYNoScroll,
                    evento.wheel.y
                );
            }

            if (evento.type == SDL_KEYDOWN) {
                if (evento.key.keysym.sym == SDLK_ESCAPE) {
                    executando = false;
                }

                if (evento.key.keysym.sym == SDLK_F5) {
                    Configuracao::carregarConfiguracoesDoLayout(diretorioAssets / "config.ini", configuracoes);
                    aplicarOrigemCentradaDaGrade(configuracoes, jogo.tamanhoAtualDoGrid);
                    limitarPanAosLimitesDoGrid(jogo.camera, configuracoes, jogo.tamanhoAtualDoGrid);
                    texturaFundo = carregarTexturaDeFundoPrincipal(ativos, diretorioAssets, configuracoes);
                }

                if (evento.key.keysym.sym == SDLK_HOME) {
                    centralizarCamera(jogo.camera);
                    aplicarOrigemCentradaDaGrade(configuracoes, jogo.tamanhoAtualDoGrid);
                }
            }

            if (evento.type == SDL_MOUSEBUTTONDOWN && evento.button.button == SDL_BUTTON_LEFT) {
                if (jogo.camera.panAtivo) {
                    continue;
                }

                mouseX = evento.button.x;
                mouseY = evento.button.y;

                if (processarCliqueNaInterface(mouseX, mouseY, botoes, jogo.ferramentaSelecionada)) {
                    if (sdl.audioInicializado) {
                        ativos.tocarSom(caminhoDoSomDeCliqueDaInterface(diretorioAssets));
                    }
                    continue;
                }

                const PosicaoNaGrade posicaoNaGrade = converterMouseParaGradeGlobal(
                    mouseX,
                    mouseY,
                    configuracoes,
                    jogo.camera
                );
                const ResultadoDaFerramenta resultado = aplicarFerramentaNoJogo(jogo, posicaoNaGrade);
                tocarSomDaAcaoSePossivel(sdl.audioInicializado, ativos, diretorioAssets, resultado);
            }
        }

        avancarTempoDoJogo(jogo, deltaTime);
        atualizarInerciaDaCamera(jogo.camera, configuracoes, jogo.tamanhoAtualDoGrid, deltaTime);

        const PosicaoNaGrade posicaoRealcada = jogo.camera.panAtivo
            ? PosicaoNaGrade{-1, -1}
            : converterMouseParaGradeGlobal(mouseX, mouseY, configuracoes, jogo.camera);

        SDL_SetRenderDrawColor(renderizador.ponteiro, 0, 0, 0, 255);
        SDL_RenderClear(renderizador.ponteiro);
        Desenho::desenharFundo(renderizador.ponteiro, texturaFundo);

        desenharGradeAtiva(renderizador.ponteiro, jogo, texturasCanteiro, configuracoes, posicaoRealcada);
        desenharLimiteDaGradeJogavel(renderizador.ponteiro, jogo, configuracoes);
        desenharPreviewDeCriacaoDeTerra(renderizador.ponteiro, jogo, configuracoes, posicaoRealcada);

        Desenho::desenharInterface(
            renderizador.ponteiro,
            jogo.ferramentaSelecionada,
            botoes.cursor,
            botoes.enxada,
            botoes.removerTerra,
            botoes.semente,
            botoes.presente
        );
        Desenho::desenharCursorCustomizado(renderizador.ponteiro, mouseX, mouseY, jogo.ferramentaSelecionada);

        SDL_RenderPresent(renderizador.ponteiro);

        const Uint32 duracaoDoQuadro = SDL_GetTicks() - inicioDoQuadro;
        if (duracaoDoQuadro < Constantes::MILISSEGUNDOS_POR_QUADRO) {
            SDL_Delay(Constantes::MILISSEGUNDOS_POR_QUADRO - duracaoDoQuadro);
        }
    }

    return 0;
}
