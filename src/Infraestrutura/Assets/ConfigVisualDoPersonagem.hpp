#pragma once

#include "Compartilhado/Geometria/Posicoes.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cmath>

namespace MiniFazenda::Infraestrutura::Assets::ConfigVisualDoPersonagem {

// Ajustes manuais do sprite do personagem. Mantenha recorte, escala e ancora
// visual aqui para o renderizador nao depender de numeros magicos.
// Para adicionar novas animacoes, crie uma nova funcao de configuracao e inclua
// a entrada em CONFIGURACOES_POR_ANIMACAO.
enum class AnimacaoVisualDoPersonagem {
    Idle,
    WalkBaixoDireita,
    WalkBaixoEsquerda,
    WalkCimaDireita,
    WalkCimaEsquerda
};

enum class ModoDeReproducaoDaAnimacao {
    FrameFixo,
    LoopContinuo,
    IdleComPiscadas
};

constexpr const char* caminhoTextura = "sprites/personagem/Boneco_piscando_olhos.png";

constexpr int quantidadeFramesIdle = 5;
constexpr int frameOrigemX = 0;
constexpr int frameOrigemY = 0;
constexpr int frameLargura = 250;
constexpr int frameAltura = 250;
constexpr int frameEspacamentoX = 0;
constexpr int frameEspacamentoY = 0;
constexpr float duracaoPorFrame = 0.065f;

constexpr int destinoLargura = 63;
constexpr int destinoAltura = 96;
constexpr int offsetPeX = 0;
constexpr int offsetPeY = 0;
constexpr int ajusteTelaX = 0;
constexpr int ajusteTelaY = 0;
constexpr int pontoDosPesX = (destinoLargura + 1) / 2;
constexpr int pontoDosPesY = destinoAltura;

constexpr bool debugDesenhoDoPersonagem = false;

struct ConfiguracaoDaAnimacao {
    const char* caminhoTextura = "";
    int quantidadeFrames = 1;
    int frameOrigemX = 0;
    int frameOrigemY = 0;
    int frameLargura = 1;
    int frameAltura = 1;
    int frameEspacamentoX = 0;
    int frameEspacamentoY = 0;
    float duracaoPorFrame = 0.1f;
    int destinoLargura = 1;
    int destinoAltura = 1;
    int offsetPeX = 0;
    int offsetPeY = 0;
    int ajusteTelaX = 0;
    int ajusteTelaY = 0;
    int pontoDosPesX = 0;
    int pontoDosPesY = 0;
    ModoDeReproducaoDaAnimacao modoDeReproducao = ModoDeReproducaoDaAnimacao::FrameFixo;
};

constexpr ConfiguracaoDaAnimacao configuracaoIdle() {
    return ConfiguracaoDaAnimacao{
        caminhoTextura,
        quantidadeFramesIdle,
        frameOrigemX,
        frameOrigemY,
        frameLargura,
        frameAltura,
        frameEspacamentoX,
        frameEspacamentoY,
        duracaoPorFrame,
        destinoLargura,
        destinoAltura,
        offsetPeX,
        offsetPeY,
        ajusteTelaX,
        ajusteTelaY,
        pontoDosPesX,
        pontoDosPesY,
        ModoDeReproducaoDaAnimacao::IdleComPiscadas
    };
}

// Por enquanto as caminhadas usam o idle como fallback. Para adicionar
// spritesheets futuras, crie constantes equivalentes acima e retorne outra
// ConfiguracaoDaAnimacao na funcao correspondente.
constexpr ConfiguracaoDaAnimacao configuracaoWalkBaixoDireita() {
    ConfiguracaoDaAnimacao configuracao = configuracaoIdle();
    configuracao.modoDeReproducao = ModoDeReproducaoDaAnimacao::FrameFixo;
    return configuracao;
}

constexpr ConfiguracaoDaAnimacao configuracaoWalkBaixoEsquerda() {
    ConfiguracaoDaAnimacao configuracao = configuracaoIdle();
    configuracao.modoDeReproducao = ModoDeReproducaoDaAnimacao::FrameFixo;
    return configuracao;
}

constexpr ConfiguracaoDaAnimacao configuracaoWalkCimaDireita() {
    ConfiguracaoDaAnimacao configuracao = configuracaoIdle();
    configuracao.modoDeReproducao = ModoDeReproducaoDaAnimacao::FrameFixo;
    return configuracao;
}

constexpr ConfiguracaoDaAnimacao configuracaoWalkCimaEsquerda() {
    ConfiguracaoDaAnimacao configuracao = configuracaoIdle();
    configuracao.modoDeReproducao = ModoDeReproducaoDaAnimacao::FrameFixo;
    return configuracao;
}

constexpr std::size_t QUANTIDADE_DE_ANIMACOES = 5;

inline constexpr std::array<ConfiguracaoDaAnimacao, QUANTIDADE_DE_ANIMACOES> CONFIGURACOES_POR_ANIMACAO = {{
    configuracaoIdle(),
    configuracaoWalkBaixoDireita(),
    configuracaoWalkBaixoEsquerda(),
    configuracaoWalkCimaDireita(),
    configuracaoWalkCimaEsquerda()
}};

inline std::size_t indiceDaAnimacao(AnimacaoVisualDoPersonagem animacao) {
    switch (animacao) {
        case AnimacaoVisualDoPersonagem::Idle:
            return 0;
        case AnimacaoVisualDoPersonagem::WalkBaixoDireita:
            return 1;
        case AnimacaoVisualDoPersonagem::WalkBaixoEsquerda:
            return 2;
        case AnimacaoVisualDoPersonagem::WalkCimaDireita:
            return 3;
        case AnimacaoVisualDoPersonagem::WalkCimaEsquerda:
            return 4;
        default:
            return 0;
    }
}

inline const ConfiguracaoDaAnimacao& configuracaoParaAnimacao(
    AnimacaoVisualDoPersonagem animacao
) {
    return CONFIGURACOES_POR_ANIMACAO[indiceDaAnimacao(animacao)];
}

inline Compartilhado::Geometria::Retangulo calcularRetanguloDeOrigem(
    const ConfiguracaoDaAnimacao& configuracao,
    int indiceFrame
) {
    const int frameNormalizado = indiceFrame < 0 ? 0 : indiceFrame;
    const int frameSeguro = configuracao.quantidadeFrames > 0
        ? frameNormalizado % configuracao.quantidadeFrames
        : 0;

    return Compartilhado::Geometria::Retangulo{
        configuracao.frameOrigemX + frameSeguro * (configuracao.frameLargura + configuracao.frameEspacamentoX),
        configuracao.frameOrigemY,
        configuracao.frameLargura,
        configuracao.frameAltura
    };
}

inline Compartilhado::Geometria::Retangulo calcularRetanguloDeDestino(
    const ConfiguracaoDaAnimacao& configuracao,
    Compartilhado::Geometria::PosicaoNaTela posicaoDosPesNaTela,
    float escalaVisual = 1.0f
) {
    const int largura = std::max(1, static_cast<int>(std::lround(configuracao.destinoLargura * escalaVisual)));
    const int altura = std::max(1, static_cast<int>(std::lround(configuracao.destinoAltura * escalaVisual)));
    const int pontoPesX = static_cast<int>(std::lround(configuracao.pontoDosPesX * escalaVisual));
    const int pontoPesY = static_cast<int>(std::lround(configuracao.pontoDosPesY * escalaVisual));
    const int offsetX = static_cast<int>(std::lround(configuracao.offsetPeX * escalaVisual));
    const int offsetY = static_cast<int>(std::lround(configuracao.offsetPeY * escalaVisual));
    const int ajusteX = static_cast<int>(std::lround(configuracao.ajusteTelaX * escalaVisual));
    const int ajusteY = static_cast<int>(std::lround(configuracao.ajusteTelaY * escalaVisual));
    const int telaPesX = posicaoDosPesNaTela.coordenadaHorizontal + offsetX;
    const int telaPesY = posicaoDosPesNaTela.coordenadaVertical + offsetY;

    return Compartilhado::Geometria::Retangulo{
        telaPesX - pontoPesX + ajusteX,
        telaPesY - pontoPesY + ajusteY,
        largura,
        altura
    };
}

} // namespace MiniFazenda::Infraestrutura::Assets::ConfigVisualDoPersonagem
