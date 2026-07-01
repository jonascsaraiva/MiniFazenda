#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <random>

namespace MiniFazenda::Dominio::Animacao {

enum class ExpressaoIdleDoPersonagem {
    PiscadaRapida,
    PiscadaDupla,
    Cansaco
};

class AnimacaoIdle {
public:
    AnimacaoIdle()
        : AnimacaoIdle(criarSementeAleatoria()) {
    }

    explicit AnimacaoIdle(unsigned int sementeAleatoria)
        : geradorAleatorio_(sementeAleatoria) {
        reiniciar();
    }

    void avancar(float deltaTime) {
        if (deltaTime <= 0.0f) {
            return;
        }

        float tempoRestante = deltaTime;
        int etapasProcessadas = 0;

        while (tempoRestante > 0.0f && etapasProcessadas < LIMITE_DE_ETAPAS_POR_AVANCO) {
            ++etapasProcessadas;

            if (estadoAtual_ == EstadoDaAnimacao::Aguardando) {
                if (tempoRestante < tempoRestanteDaEtapa_) {
                    tempoRestanteDaEtapa_ -= tempoRestante;
                    return;
                }

                tempoRestante -= tempoRestanteDaEtapa_;
                iniciarExpressaoSorteada();
                continue;
            }

            if (tempoRestante < tempoRestanteDaEtapa_) {
                tempoRestanteDaEtapa_ -= tempoRestante;
                return;
            }

            tempoRestante -= tempoRestanteDaEtapa_;
            avancarParaProximaEtapa();
        }
    }

    void reiniciar() {
        estadoAtual_ = EstadoDaAnimacao::Aguardando;
        quantidadeEtapas_ = 0;
        etapaAtual_ = 0;
        indiceFrameAtual_ = FRAME_OLHOS_ABERTOS_INICIAL;
        tempoRestanteDaEtapa_ = sortearIntervaloEntreExpressoes();
    }

    int indiceFrameAtual() const {
        return indiceFrameAtual_;
    }

    void iniciarPiscadaRapida(float duracaoPorFrame) {
        const float duracaoNormalizada = normalizarDuracaoRapida(duracaoPorFrame);

        limparEtapas();
        adicionarEtapa(FRAME_OLHOS_ABERTOS_INICIAL, duracaoNormalizada);
        adicionarEtapa(FRAME_OLHO_SEMI_FECHADO, duracaoNormalizada);
        adicionarEtapa(FRAME_OLHO_FECHANDO, duracaoNormalizada);
        adicionarEtapa(FRAME_OLHO_REABRINDO, duracaoNormalizada);
        adicionarEtapa(FRAME_OLHOS_ABERTOS_FINAL, duracaoNormalizada);
        iniciarSequenciaConfigurada();
    }

    void iniciarPiscadaDupla(float duracaoPorFrame) {
        const float duracaoNormalizada = normalizarDuracaoRapida(duracaoPorFrame);

        limparEtapas();
        adicionarEtapa(FRAME_OLHOS_ABERTOS_INICIAL, duracaoNormalizada);
        adicionarEtapa(FRAME_OLHO_SEMI_FECHADO, duracaoNormalizada);
        adicionarEtapa(FRAME_OLHO_FECHANDO, duracaoNormalizada);
        adicionarEtapa(FRAME_OLHO_REABRINDO, duracaoNormalizada);
        adicionarEtapa(FRAME_OLHOS_ABERTOS_INICIAL, duracaoNormalizada);
        adicionarEtapa(FRAME_OLHO_SEMI_FECHADO, duracaoNormalizada);
        adicionarEtapa(FRAME_OLHO_FECHANDO, duracaoNormalizada);
        adicionarEtapa(FRAME_OLHO_REABRINDO, duracaoNormalizada);
        adicionarEtapa(FRAME_OLHOS_ABERTOS_FINAL, duracaoNormalizada);
        iniciarSequenciaConfigurada();
    }

    void iniciarCansaco(float duracaoPorFrame, float duracaoOlhoSemiFechado) {
        const float duracaoNormalizada = normalizarDuracaoRapida(duracaoPorFrame);
        const float pausaNormalizada = std::clamp(
            duracaoOlhoSemiFechado,
            DURACAO_MINIMA_CANSACO,
            DURACAO_MAXIMA_CANSACO
        );

        limparEtapas();
        adicionarEtapa(FRAME_OLHOS_ABERTOS_INICIAL, duracaoNormalizada);
        adicionarEtapa(FRAME_OLHO_SEMI_FECHADO, pausaNormalizada);
        adicionarEtapa(FRAME_OLHOS_ABERTOS_FINAL, duracaoNormalizada);
        iniciarSequenciaConfigurada();
    }

    void iniciarPiscadaRapida() {
        iniciarPiscadaRapida(sortearDuracaoRapidaPorFrame());
    }

    void iniciarPiscadaDupla() {
        iniciarPiscadaDupla(sortearDuracaoRapidaPorFrame());
    }

    void iniciarCansaco() {
        iniciarCansaco(sortearDuracaoRapidaPorFrame(), sortearDuracaoCansaco());
    }

private:
    enum class EstadoDaAnimacao {
        Aguardando,
        ExecutandoSequencia
    };

    static constexpr int FRAME_OLHOS_ABERTOS_INICIAL = 0;
    static constexpr int FRAME_OLHO_SEMI_FECHADO = 1;
    static constexpr int FRAME_OLHO_FECHANDO = 2;
    static constexpr int FRAME_OLHO_REABRINDO = 3;
    static constexpr int FRAME_OLHOS_ABERTOS_FINAL = 4;
    static constexpr float INTERVALO_MINIMO_ENTRE_EXPRESSOES = 2.5f;
    static constexpr float INTERVALO_MAXIMO_ENTRE_EXPRESSOES = 6.0f;
    static constexpr float DURACAO_MINIMA_FRAME_RAPIDO = 0.04f;
    static constexpr float DURACAO_MAXIMA_FRAME_RAPIDO = 0.07f;
    static constexpr float DURACAO_MINIMA_CANSACO = 1.5f;
    static constexpr float DURACAO_MAXIMA_CANSACO = 3.0f;
    static constexpr int LIMITE_DE_ETAPAS_POR_AVANCO = 64;
    static constexpr std::size_t QUANTIDADE_MAXIMA_DE_ETAPAS = 9;

    struct EtapaDaAnimacao {
        int indiceFrame = FRAME_OLHOS_ABERTOS_INICIAL;
        float duracao = 0.0f;
    };

    static unsigned int criarSementeAleatoria() {
        return std::random_device{}();
    }

    float sortear(float minimo, float maximo) {
        std::uniform_real_distribution<float> distribuicao(minimo, maximo);
        return distribuicao(geradorAleatorio_);
    }

    float sortearIntervaloEntreExpressoes() {
        return sortear(INTERVALO_MINIMO_ENTRE_EXPRESSOES, INTERVALO_MAXIMO_ENTRE_EXPRESSOES);
    }

    float sortearDuracaoRapidaPorFrame() {
        return sortear(DURACAO_MINIMA_FRAME_RAPIDO, DURACAO_MAXIMA_FRAME_RAPIDO);
    }

    float sortearDuracaoCansaco() {
        return sortear(DURACAO_MINIMA_CANSACO, DURACAO_MAXIMA_CANSACO);
    }

    float normalizarDuracaoRapida(float duracaoPorFrame) const {
        return std::clamp(duracaoPorFrame, DURACAO_MINIMA_FRAME_RAPIDO, DURACAO_MAXIMA_FRAME_RAPIDO);
    }

    ExpressaoIdleDoPersonagem sortearExpressao() {
        std::uniform_int_distribution<int> distribuicao(1, 100);
        const int chance = distribuicao(geradorAleatorio_);

        if (chance <= 80) {
            return ExpressaoIdleDoPersonagem::PiscadaRapida;
        }

        if (chance <= 95) {
            return ExpressaoIdleDoPersonagem::PiscadaDupla;
        }

        return ExpressaoIdleDoPersonagem::Cansaco;
    }

    void iniciarExpressaoSorteada() {
        const ExpressaoIdleDoPersonagem expressao = sortearExpressao();

        if (expressao == ExpressaoIdleDoPersonagem::PiscadaRapida) {
            iniciarPiscadaRapida();
            return;
        }

        if (expressao == ExpressaoIdleDoPersonagem::PiscadaDupla) {
            iniciarPiscadaDupla();
            return;
        }

        iniciarCansaco();
    }

    void limparEtapas() {
        etapas_ = {};
        quantidadeEtapas_ = 0;
        etapaAtual_ = 0;
    }

    void adicionarEtapa(int indiceFrame, float duracao) {
        if (quantidadeEtapas_ >= etapas_.size()) {
            return;
        }

        etapas_[quantidadeEtapas_] = EtapaDaAnimacao{indiceFrame, duracao};
        ++quantidadeEtapas_;
    }

    void iniciarSequenciaConfigurada() {
        if (quantidadeEtapas_ == 0) {
            reiniciar();
            return;
        }

        estadoAtual_ = EstadoDaAnimacao::ExecutandoSequencia;
        etapaAtual_ = 0;
        indiceFrameAtual_ = etapas_[etapaAtual_].indiceFrame;
        tempoRestanteDaEtapa_ = etapas_[etapaAtual_].duracao;
    }

    void avancarParaProximaEtapa() {
        ++etapaAtual_;

        if (etapaAtual_ >= quantidadeEtapas_) {
            reiniciar();
            return;
        }

        indiceFrameAtual_ = etapas_[etapaAtual_].indiceFrame;
        tempoRestanteDaEtapa_ = etapas_[etapaAtual_].duracao;
    }

    EstadoDaAnimacao estadoAtual_ = EstadoDaAnimacao::Aguardando;
    std::array<EtapaDaAnimacao, QUANTIDADE_MAXIMA_DE_ETAPAS> etapas_{};
    std::size_t quantidadeEtapas_ = 0;
    std::size_t etapaAtual_ = 0;
    int indiceFrameAtual_ = FRAME_OLHOS_ABERTOS_INICIAL;
    float tempoRestanteDaEtapa_ = 0.0f;
    std::mt19937 geradorAleatorio_;
};

} // namespace MiniFazenda::Dominio::Animacao
