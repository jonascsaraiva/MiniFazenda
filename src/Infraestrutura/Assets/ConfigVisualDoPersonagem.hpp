#pragma once

#include "Compartilhado/Geometria/Posicoes.hpp"
#include "Dominio/Personagem/Personagem.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cmath>

namespace MiniFazenda::Infraestrutura::Assets::ConfigVisualDoPersonagem {

// Ajustes manuais do sprite do personagem. Mantenha recorte, escala e ancora
// visual aqui para o renderizador nao depender de numeros magicos.
// Para adicionar novas animacoes, crie uma nova funcao de configuracao e adicione
constexpr const char* caminhoTextura = "sprites/personagem/bonequinho.png";

constexpr int quantidadeFramesIdle = 8;
constexpr int frameOrigemX = 0;
constexpr int frameOrigemY = 145;
constexpr int frameLargura = 271;
constexpr int frameAltura = 416;
constexpr int frameEspacamentoX = 0;
constexpr int frameEspacamentoY = 0;
constexpr float duracaoPorFrame = 0.12f;

constexpr int destinoLargura = 63;
constexpr int destinoAltura = 96;
constexpr int offsetPeX = 0;
constexpr int offsetPeY = 0;
constexpr int ajusteTelaX = 0;
constexpr int ajusteTelaY = 0;
constexpr int pontoDosPesX = 28;
constexpr int pontoDosPesY = 95;

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
        pontoDosPesY
    };
}

// Por enquanto as caminhadas usam o idle como fallback. Para adicionar
// spritesheets futuras, crie constantes equivalentes acima e retorne outra
// ConfiguracaoDaAnimacao na funcao correspondente.
constexpr ConfiguracaoDaAnimacao configuracaoWalkBaixoDireita() {
    return configuracaoIdle();
}

constexpr ConfiguracaoDaAnimacao configuracaoWalkBaixoEsquerda() {
    return configuracaoIdle();
}

constexpr ConfiguracaoDaAnimacao configuracaoWalkCimaDireita() {
    return configuracaoIdle();
}

constexpr ConfiguracaoDaAnimacao configuracaoWalkCimaEsquerda() {
    return configuracaoIdle();
}

constexpr std::size_t QUANTIDADE_DE_ANIMACOES = 5;

inline constexpr std::array<ConfiguracaoDaAnimacao, QUANTIDADE_DE_ANIMACOES> CONFIGURACOES_POR_ANIMACAO = {{
    configuracaoIdle(),
    configuracaoWalkBaixoDireita(),
    configuracaoWalkBaixoEsquerda(),
    configuracaoWalkCimaDireita(),
    configuracaoWalkCimaEsquerda()
}};

inline std::size_t indiceDaAnimacao(Dominio::Personagem::AnimacaoVisualDoPersonagem animacao) {
    switch (animacao) {
        case Dominio::Personagem::AnimacaoVisualDoPersonagem::Idle:
            return 0;
        case Dominio::Personagem::AnimacaoVisualDoPersonagem::WalkBaixoDireita:
            return 1;
        case Dominio::Personagem::AnimacaoVisualDoPersonagem::WalkBaixoEsquerda:
            return 2;
        case Dominio::Personagem::AnimacaoVisualDoPersonagem::WalkCimaDireita:
            return 3;
        case Dominio::Personagem::AnimacaoVisualDoPersonagem::WalkCimaEsquerda:
            return 4;
        default:
            return 0;
    }
}

inline const ConfiguracaoDaAnimacao& configuracaoParaAnimacao(
    Dominio::Personagem::AnimacaoVisualDoPersonagem animacao
) {
    return CONFIGURACOES_POR_ANIMACAO[indiceDaAnimacao(animacao)];
}

inline int calcularIndiceFrame(const ConfiguracaoDaAnimacao& configuracao, float tempoDaAnimacao) {
    if (configuracao.quantidadeFrames <= 0 || configuracao.duracaoPorFrame <= 0.0f || tempoDaAnimacao <= 0.0f) {
        return 0;
    }

    constexpr float toleranciaDeBordaDoFrame = 0.00001f;
    const int indiceSemLoop = static_cast<int>(
        std::floor((tempoDaAnimacao + toleranciaDeBordaDoFrame) / configuracao.duracaoPorFrame)
    );
    return indiceSemLoop % configuracao.quantidadeFrames;
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
