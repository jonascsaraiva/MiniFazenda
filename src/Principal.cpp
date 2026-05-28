#define SDL_MAIN_HANDLED

#include "Ativos.hpp"
#include "Configuracao.hpp"
#include "Constantes.hpp"
#include "Desenho.hpp"
#include "Isometrico.hpp"
#include "Tipos.hpp"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include <array>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace {

struct BotoesDaInterface {
    AreaDeInteracao configuracao;
    AreaDeInteracao cursor;
    AreaDeInteracao enxada;
    AreaDeInteracao semente;
    AreaDeInteracao presente;
};

std::filesystem::path obterDiretorioBaseDoExecutavel() {
    char* caminhoBase = SDL_GetBasePath();
    if (caminhoBase == nullptr) {
        return std::filesystem::current_path();
    }

    std::filesystem::path diretorio(caminhoBase);
    SDL_free(caminhoBase);
    return diretorio;
}

std::filesystem::path localizarDiretorioDeAssets() {
    const std::vector<std::filesystem::path> candidatos = {
        std::filesystem::current_path() / "assets",
        obterDiretorioBaseDoExecutavel() / "assets",
        obterDiretorioBaseDoExecutavel().parent_path() / "assets"
    };

    for (const std::filesystem::path& candidato : candidatos) {
        if (std::filesystem::exists(candidato / "config.ini")) {
            return candidato;
        }
    }

    return std::filesystem::current_path() / "assets";
}

BotoesDaInterface criarBotoesDaInterface() {
    const int y = Constantes::ALTURA_DA_JANELA - 68;
    const int largura = Constantes::TAMANHO_DO_BOTAO_DA_INTERFACE;
    const int espacamento = Constantes::ESPACAMENTO_DOS_BOTOES;
    const int primeiroX = Constantes::LARGURA_DA_JANELA / 2 - (largura * 4 + espacamento * 3) / 2;

    return BotoesDaInterface{
        AreaDeInteracao{Constantes::LARGURA_DA_JANELA - largura - 20, 10, largura, largura},
        AreaDeInteracao{primeiroX, y, largura, largura},
        AreaDeInteracao{primeiroX + (largura + espacamento), y, largura, largura},
        AreaDeInteracao{primeiroX + (largura + espacamento) * 2, y, largura, largura},
        AreaDeInteracao{primeiroX + (largura + espacamento) * 3, y, largura, largura}
    };
}

bool processarCliqueNaInterface(
    int mouseX,
    int mouseY,
    const BotoesDaInterface& botoes,
    FerramentaSelecionada& ferramentaSelecionada
) {
    if (verificarCliqueNoBotao(mouseX, mouseY, botoes.cursor)) {
        ferramentaSelecionada = FERRAMENTA_CURSOR;
        return true;
    }

    if (verificarCliqueNoBotao(mouseX, mouseY, botoes.enxada)) {
        ferramentaSelecionada = FERRAMENTA_ENXADA;
        return true;
    }

    if (verificarCliqueNoBotao(mouseX, mouseY, botoes.semente)) {
        ferramentaSelecionada = FERRAMENTA_SEMENTE;
        return true;
    }

    if (verificarCliqueNoBotao(mouseX, mouseY, botoes.presente)) {
        ferramentaSelecionada = FERRAMENTA_PRESENTE;
        return true;
    }

    return false;
}

void aplicarFerramentaNoCanteiro(
    DadosDoCanteiro& canteiro,
    FerramentaSelecionada ferramentaSelecionada,
    int& moedas,
    int& experiencia
) {
    switch (ferramentaSelecionada) {
        case FERRAMENTA_ENXADA:
            if (canteiro.estadoVisualAtual == ESTADO_TERRA_VAZIA ||
                canteiro.estadoVisualAtual == ESTADO_PLANTA_MORTA) {
                canteiro.estadoVisualAtual = ESTADO_TERRA_ARADA;
                canteiro.tempoDePlantioEmSegundos = 0;
                canteiro.identificadorDaSemente = -1;
            }
            break;

        case FERRAMENTA_SEMENTE:
            if (canteiro.estadoVisualAtual == ESTADO_TERRA_ARADA && moedas >= 2) {
                moedas -= 2;
                canteiro.estadoVisualAtual = ESTADO_SEMENTE_PLANTADA;
                canteiro.tempoDePlantioEmSegundos = 0;
                canteiro.identificadorDaSemente = 1;
            }
            break;

        case FERRAMENTA_PRESENTE:
            if (canteiro.estadoVisualAtual == ESTADO_SEMENTE_PLANTADA ||
                canteiro.estadoVisualAtual == ESTADO_PLANTA_CRESCENDO) {
                canteiro.estadoVisualAtual = ESTADO_PLANTA_MADURA;
                canteiro.tempoDePlantioEmSegundos = Constantes::TEMPO_PARA_MADURAR;
            }
            break;

        case FERRAMENTA_CURSOR:
        default:
            if (canteiro.estadoVisualAtual == ESTADO_PLANTA_MADURA) {
                moedas += 8;
                experiencia += 5;
                canteiro.estadoVisualAtual = ESTADO_TERRA_VAZIA;
                canteiro.tempoDePlantioEmSegundos = 0;
                canteiro.identificadorDaSemente = -1;
            }
            break;
    }
}

void avancarCrescimentoDosCanteiros(MatrizDeCanteiros& canteiros) {
    for (LinhaDeCanteiros& linha : canteiros) {
        for (DadosDoCanteiro& canteiro : linha) {
            if (canteiro.estadoVisualAtual != ESTADO_SEMENTE_PLANTADA &&
                canteiro.estadoVisualAtual != ESTADO_PLANTA_CRESCENDO &&
                canteiro.estadoVisualAtual != ESTADO_PLANTA_MADURA) {
                continue;
            }

            ++canteiro.tempoDePlantioEmSegundos;

            if (canteiro.tempoDePlantioEmSegundos >= Constantes::TEMPO_PARA_MORRER) {
                canteiro.estadoVisualAtual = ESTADO_PLANTA_MORTA;
            } else if (canteiro.tempoDePlantioEmSegundos >= Constantes::TEMPO_PARA_MADURAR) {
                canteiro.estadoVisualAtual = ESTADO_PLANTA_MADURA;
            } else if (canteiro.tempoDePlantioEmSegundos >= Constantes::TEMPO_PARA_CRESCER) {
                canteiro.estadoVisualAtual = ESTADO_PLANTA_CRESCENDO;
            }
        }
    }
}

SDL_Texture* texturaParaEstado(
    EstadoDoCanteiro estado,
    const std::array<SDL_Texture*, 6>& texturasCanteiro
) {
    return texturasCanteiro.at(static_cast<std::size_t>(estado));
}

} // namespace

int main(int, char**) {
    SDL_SetMainReady();

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::cerr << "Falha ao inicializar SDL2: " << SDL_GetError() << '\n';
        return 1;
    }

    const int formatosImagem = IMG_INIT_PNG;
    if ((IMG_Init(formatosImagem) & formatosImagem) != formatosImagem) {
        std::cerr << "Falha ao inicializar SDL2_image: " << IMG_GetError() << '\n';
        SDL_Quit();
        return 1;
    }

    if (TTF_Init() != 0) {
        std::cerr << "Falha ao inicializar SDL2_ttf: " << TTF_GetError() << '\n';
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    bool audioInicializado = true;
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0) {
        audioInicializado = false;
        std::cerr << "Audio indisponivel: " << Mix_GetError() << '\n';
    }

    SDL_Window* janela = SDL_CreateWindow(
        Constantes::TITULO_DA_JANELA,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        Constantes::LARGURA_DA_JANELA,
        Constantes::ALTURA_DA_JANELA,
        SDL_WINDOW_SHOWN
    );

    if (janela == nullptr) {
        std::cerr << "Falha ao criar janela: " << SDL_GetError() << '\n';
        if (audioInicializado) {
            Mix_CloseAudio();
        }
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderizador = SDL_CreateRenderer(
        janela,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (renderizador == nullptr) {
        renderizador = SDL_CreateRenderer(janela, -1, SDL_RENDERER_SOFTWARE);
    }

    if (renderizador == nullptr) {
        std::cerr << "Falha ao criar renderizador: " << SDL_GetError() << '\n';
        SDL_DestroyWindow(janela);
        if (audioInicializado) {
            Mix_CloseAudio();
        }
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderDrawBlendMode(renderizador, SDL_BLENDMODE_BLEND);
    SDL_ShowCursor(SDL_DISABLE);

    {
        const std::filesystem::path diretorioAssets = localizarDiretorioDeAssets();
        ConfiguracoesDoLayout configuracoes;
        Configuracao::carregarConfiguracoesDoLayout(diretorioAssets / "config.ini", configuracoes);

        GerenciadorDeAtivos ativos(renderizador);
        SDL_Texture* texturaFundo = ativos.carregarTextura(diretorioAssets / "background" / "fazenda.png");
        SDL_Texture* texturaCasa = ativos.carregarTextura(diretorioAssets / "sprites" / "casa.png");
        SDL_Texture* texturaCasinha = ativos.carregarTextura(diretorioAssets / "sprites" / "casinha_cachorro.png");
        std::array<SDL_Texture*, 6> texturasCanteiro{
            ativos.carregarTextura(diretorioAssets / "sprites" / "terra_vazia.png"),
            ativos.carregarTextura(diretorioAssets / "sprites" / "terra_arada.png"),
            ativos.carregarTextura(diretorioAssets / "sprites" / "semente_plantada.png"),
            ativos.carregarTextura(diretorioAssets / "sprites" / "planta_crescendo.png"),
            ativos.carregarTextura(diretorioAssets / "sprites" / "planta_madura.png"),
            ativos.carregarTextura(diretorioAssets / "sprites" / "planta_morta.png")
        };
        TTF_Font* fonteInterface = ativos.carregarFonte(diretorioAssets / "fonts" / "interface.ttf", 22);

        if (audioInicializado) {
            ativos.tocarMusica(diretorioAssets / "sounds" / "ambiente.ogg");
        }

        MatrizDeCanteiros canteiros{};
        BotoesDaInterface botoes = criarBotoesDaInterface();
        FerramentaSelecionada ferramentaSelecionada = FERRAMENTA_CURSOR;
        bool executando = true;
        int moedas = 50;
        int experiencia = 0;
        int nivel = 1;
        int mouseX = Constantes::LARGURA_DA_JANELA / 2;
        int mouseY = Constantes::ALTURA_DA_JANELA / 2;
        float acumuladorDeSegundos = 0.0f;
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
                }

                if (evento.type == SDL_KEYDOWN) {
                    if (evento.key.keysym.sym == SDLK_ESCAPE) {
                        executando = false;
                    }

                    if (evento.key.keysym.sym == SDLK_F5) {
                        Configuracao::carregarConfiguracoesDoLayout(diretorioAssets / "config.ini", configuracoes);
                    }
                }

                if (evento.type == SDL_MOUSEBUTTONDOWN && evento.button.button == SDL_BUTTON_LEFT) {
                    mouseX = evento.button.x;
                    mouseY = evento.button.y;

                    if (verificarCliqueNoBotao(mouseX, mouseY, botoes.configuracao)) {
                        Configuracao::carregarConfiguracoesDoLayout(diretorioAssets / "config.ini", configuracoes);
                        if (audioInicializado) {
                            ativos.tocarSom(diretorioAssets / "sounds" / "clique.wav");
                        }
                        continue;
                    }

                    if (processarCliqueNaInterface(mouseX, mouseY, botoes, ferramentaSelecionada)) {
                        if (audioInicializado) {
                            ativos.tocarSom(diretorioAssets / "sounds" / "clique.wav");
                        }
                        continue;
                    }

                    PosicaoNaGrade posicaoNaGrade = converterTelaParaGrade(
                        mouseX,
                        mouseY,
                        Constantes::LARGURA_DO_CANTEIRO,
                        Constantes::ALTURA_DO_CANTEIRO,
                        configuracoes.deslocamentoGradeHorizontal,
                        configuracoes.deslocamentoGradeVertical
                    );

                    if (posicaoEstaDentroDaGrade(posicaoNaGrade)) {
                        DadosDoCanteiro& canteiro =
                            canteiros[posicaoNaGrade.indiceLinha][posicaoNaGrade.indiceColuna];
                        aplicarFerramentaNoCanteiro(canteiro, ferramentaSelecionada, moedas, experiencia);

                        if (audioInicializado) {
                            ativos.tocarSom(diretorioAssets / "sounds" / "interacao.wav");
                        }
                    }
                }
            }

            acumuladorDeSegundos += deltaTime;
            while (acumuladorDeSegundos >= 1.0f) {
                avancarCrescimentoDosCanteiros(canteiros);
                acumuladorDeSegundos -= 1.0f;
            }

            nivel = 1 + experiencia / 50;

            PosicaoNaGrade posicaoRealcada = converterTelaParaGrade(
                mouseX,
                mouseY,
                Constantes::LARGURA_DO_CANTEIRO,
                Constantes::ALTURA_DO_CANTEIRO,
                configuracoes.deslocamentoGradeHorizontal,
                configuracoes.deslocamentoGradeVertical
            );

            SDL_SetRenderDrawColor(renderizador, 0, 0, 0, 255);
            SDL_RenderClear(renderizador);
            Desenho::desenharFundo(renderizador, texturaFundo);

            for (int linha = 0; linha < Constantes::QUANTIDADE_DE_LINHAS_DA_GRADE; ++linha) {
                for (int coluna = 0; coluna < Constantes::QUANTIDADE_DE_COLUNAS_DA_GRADE; ++coluna) {
                    PosicaoNaTela posicaoNaTela = converterGradeParaTela(
                        coluna,
                        linha,
                        Constantes::LARGURA_DO_CANTEIRO,
                        Constantes::ALTURA_DO_CANTEIRO,
                        configuracoes.deslocamentoGradeHorizontal,
                        configuracoes.deslocamentoGradeVertical
                    );

                    SDL_Rect destino{
                        posicaoNaTela.coordenadaHorizontal,
                        posicaoNaTela.coordenadaVertical,
                        Constantes::LARGURA_DO_CANTEIRO,
                        Constantes::ALTURA_DO_CANTEIRO
                    };

                    const bool destacado = posicaoEstaDentroDaGrade(posicaoRealcada) &&
                        posicaoRealcada.indiceColuna == coluna &&
                        posicaoRealcada.indiceLinha == linha;

                    const DadosDoCanteiro& canteiro = canteiros[linha][coluna];
                    Desenho::desenharCanteiro(
                        renderizador,
                        texturaParaEstado(canteiro.estadoVisualAtual, texturasCanteiro),
                        canteiro,
                        destino,
                        destacado
                    );
                }
            }

            Desenho::desenharCasa(
                renderizador,
                texturaCasa,
                SDL_Rect{
                    configuracoes.posicaoCasaHorizontal,
                    configuracoes.posicaoCasaVertical,
                    configuracoes.tamanhoLarguraCasa,
                    configuracoes.tamanhoAlturaCasa
                }
            );

            Desenho::desenharCasinha(
                renderizador,
                texturaCasinha,
                SDL_Rect{
                    configuracoes.posicaoCasinhaHorizontal,
                    configuracoes.posicaoCasinhaVertical,
                    configuracoes.tamanhoLarguraCasinha,
                    configuracoes.tamanhoAlturaCasinha
                }
            );

            Desenho::desenharInterface(
                renderizador,
                fonteInterface,
                moedas,
                experiencia,
                nivel,
                ferramentaSelecionada,
                botoes.configuracao,
                botoes.cursor,
                botoes.enxada,
                botoes.semente,
                botoes.presente
            );
            Desenho::desenharCursorCustomizado(renderizador, mouseX, mouseY, ferramentaSelecionada);

            SDL_RenderPresent(renderizador);

            const Uint32 duracaoDoQuadro = SDL_GetTicks() - inicioDoQuadro;
            if (duracaoDoQuadro < Constantes::MILISSEGUNDOS_POR_QUADRO) {
                SDL_Delay(Constantes::MILISSEGUNDOS_POR_QUADRO - duracaoDoQuadro);
            }
        }
    }

    SDL_ShowCursor(SDL_ENABLE);
    SDL_DestroyRenderer(renderizador);
    SDL_DestroyWindow(janela);

    if (audioInicializado) {
        Mix_CloseAudio();
    }

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
