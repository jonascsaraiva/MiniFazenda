#pragma once

#include "Compartilhado/Constantes.hpp"
#include "Compartilhado/Geometria/Posicoes.hpp"
#include "Dominio/Animacao/AnimacaoIdle.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <vector>

namespace MiniFazenda::Dominio::Personagem {

enum class EstadoDoPersonagem {
    Parado,
    Andando
};

enum class DirecaoIsometrica {
    BaixoDireita,
    BaixoEsquerda,
    CimaDireita,
    CimaEsquerda
};

enum class AnimacaoVisualDoPersonagem {
    Idle,
    WalkBaixoDireita,
    WalkBaixoEsquerda,
    WalkCimaDireita,
    WalkCimaEsquerda
};

class Personagem {
public:
    Compartilhado::Geometria::PosicaoNaGrade posicaoNaGrade() const {
        return Compartilhado::Geometria::PosicaoNaGrade{
            static_cast<int>(std::lround(posicaoDosPesNaGrade_.indiceColuna)),
            static_cast<int>(std::lround(posicaoDosPesNaGrade_.indiceLinha))
        };
    }

    Compartilhado::Geometria::PosicaoNaGradeDecimal posicaoDosPesNaGrade() const {
        return posicaoDosPesNaGrade_;
    }

    EstadoDoPersonagem estadoAtual() const {
        return estadoAtual_;
    }

    DirecaoIsometrica direcaoAtual() const {
        return direcaoAtual_;
    }

    AnimacaoVisualDoPersonagem animacaoVisualAtual() const {
        if (estadoAtual_ == EstadoDoPersonagem::Parado) {
            return AnimacaoVisualDoPersonagem::Idle;
        }

        if (direcaoAtual_ == DirecaoIsometrica::BaixoDireita) {
            return AnimacaoVisualDoPersonagem::WalkBaixoDireita;
        }

        if (direcaoAtual_ == DirecaoIsometrica::BaixoEsquerda) {
            return AnimacaoVisualDoPersonagem::WalkBaixoEsquerda;
        }

        if (direcaoAtual_ == DirecaoIsometrica::CimaDireita) {
            return AnimacaoVisualDoPersonagem::WalkCimaDireita;
        }

        return AnimacaoVisualDoPersonagem::WalkCimaEsquerda;
    }

    bool estaAndando() const {
        return estadoAtual_ == EstadoDoPersonagem::Andando;
    }

    void avancarAnimacaoVisual(float deltaTime) {
        if (estadoAtual_ != EstadoDoPersonagem::Parado) {
            return;
        }

        animacaoIdle_.avancar(deltaTime);
    }

    int indiceFrameDaAnimacaoVisualAtual() const {
        if (estadoAtual_ != EstadoDoPersonagem::Parado) {
            return 0;
        }

        return animacaoIdle_.indiceFrameAtual();
    }

    void caminharAte(Compartilhado::Geometria::PosicaoNaGrade destino) {
        const Compartilhado::Geometria::PosicaoNaGradeDecimal destinoDosPes{
            static_cast<float>(destino.indiceColuna),
            static_cast<float>(destino.indiceLinha)
        };

        caminho_.clear();
        waypointAtual_ = 0;

        const float diferencaColuna = destinoDosPes.indiceColuna - posicaoDosPesNaGrade_.indiceColuna;
        const float diferencaLinha = destinoDosPes.indiceLinha - posicaoDosPesNaGrade_.indiceLinha;

        if (std::abs(diferencaColuna) <= Compartilhado::Constantes::TOLERANCIA_CHEGADA_PERSONAGEM_EM_CELULAS &&
            std::abs(diferencaLinha) <= Compartilhado::Constantes::TOLERANCIA_CHEGADA_PERSONAGEM_EM_CELULAS) {
            posicaoDosPesNaGrade_ = destinoDosPes;
            parar();
            return;
        }

        if (std::abs(diferencaColuna) > std::abs(diferencaLinha)) {
            adicionarWaypointSeNecessario(
                Compartilhado::Geometria::PosicaoNaGradeDecimal{
                    destinoDosPes.indiceColuna,
                    posicaoDosPesNaGrade_.indiceLinha
                }
            );
            adicionarWaypointSeNecessario(destinoDosPes);
        } else {
            adicionarWaypointSeNecessario(
                Compartilhado::Geometria::PosicaoNaGradeDecimal{
                    posicaoDosPesNaGrade_.indiceColuna,
                    destinoDosPes.indiceLinha
                }
            );
            adicionarWaypointSeNecessario(destinoDosPes);
        }

        if (caminho_.empty()) {
            parar();
            return;
        }

        estadoAtual_ = EstadoDoPersonagem::Andando;
        atualizarDirecaoPara(caminho_[waypointAtual_]);
    }

    void avancarMovimento(float deltaTime) {
        if (deltaTime <= 0.0f || estadoAtual_ != EstadoDoPersonagem::Andando || caminho_.empty()) {
            return;
        }

        float distanciaRestante = Compartilhado::Constantes::VELOCIDADE_PERSONAGEM_EM_CELULAS_POR_SEGUNDO * deltaTime;

        while (distanciaRestante > 0.0f && estadoAtual_ == EstadoDoPersonagem::Andando) {
            const Compartilhado::Geometria::PosicaoNaGradeDecimal destinoAtual = caminho_[waypointAtual_];
            const float diferencaColuna = destinoAtual.indiceColuna - posicaoDosPesNaGrade_.indiceColuna;
            const float diferencaLinha = destinoAtual.indiceLinha - posicaoDosPesNaGrade_.indiceLinha;
            const float distanciaAteWaypoint = std::sqrt(
                diferencaColuna * diferencaColuna + diferencaLinha * diferencaLinha
            );

            if (distanciaAteWaypoint <= Compartilhado::Constantes::TOLERANCIA_CHEGADA_PERSONAGEM_EM_CELULAS ||
                distanciaAteWaypoint <= distanciaRestante) {
                posicaoDosPesNaGrade_ = destinoAtual;
                distanciaRestante = std::max(0.0f, distanciaRestante - distanciaAteWaypoint);
                avancarParaProximoWaypoint();
                continue;
            }

            posicaoDosPesNaGrade_.indiceColuna += (diferencaColuna / distanciaAteWaypoint) * distanciaRestante;
            posicaoDosPesNaGrade_.indiceLinha += (diferencaLinha / distanciaAteWaypoint) * distanciaRestante;
            atualizarDirecaoPara(destinoAtual);
            distanciaRestante = 0.0f;
        }
    }

private:
    void adicionarWaypointSeNecessario(Compartilhado::Geometria::PosicaoNaGradeDecimal waypoint) {
        const Compartilhado::Geometria::PosicaoNaGradeDecimal referencia =
            caminho_.empty() ? posicaoDosPesNaGrade_ : caminho_.back();

        const float diferencaColuna = waypoint.indiceColuna - referencia.indiceColuna;
        const float diferencaLinha = waypoint.indiceLinha - referencia.indiceLinha;

        if (std::abs(diferencaColuna) <= Compartilhado::Constantes::TOLERANCIA_CHEGADA_PERSONAGEM_EM_CELULAS &&
            std::abs(diferencaLinha) <= Compartilhado::Constantes::TOLERANCIA_CHEGADA_PERSONAGEM_EM_CELULAS) {
            return;
        }

        caminho_.push_back(waypoint);
    }

    void avancarParaProximoWaypoint() {
        ++waypointAtual_;

        if (waypointAtual_ >= caminho_.size()) {
            parar();
            return;
        }

        atualizarDirecaoPara(caminho_[waypointAtual_]);
    }

    void parar() {
        estadoAtual_ = EstadoDoPersonagem::Parado;
        caminho_.clear();
        waypointAtual_ = 0;
        animacaoIdle_.reiniciar();
    }

    void atualizarDirecaoPara(Compartilhado::Geometria::PosicaoNaGradeDecimal destinoAtual) {
        const float diferencaColuna = destinoAtual.indiceColuna - posicaoDosPesNaGrade_.indiceColuna;
        const float diferencaLinha = destinoAtual.indiceLinha - posicaoDosPesNaGrade_.indiceLinha;

        if (std::abs(diferencaColuna) > std::abs(diferencaLinha)) {
            direcaoAtual_ = diferencaColuna >= 0.0f
                ? DirecaoIsometrica::BaixoDireita
                : DirecaoIsometrica::CimaEsquerda;
            return;
        }

        if (std::abs(diferencaLinha) > Compartilhado::Constantes::TOLERANCIA_CHEGADA_PERSONAGEM_EM_CELULAS) {
            direcaoAtual_ = diferencaLinha >= 0.0f
                ? DirecaoIsometrica::BaixoEsquerda
                : DirecaoIsometrica::CimaDireita;
        }
    }

    Compartilhado::Geometria::PosicaoNaGradeDecimal posicaoDosPesNaGrade_{
        static_cast<float>(Compartilhado::Constantes::COLUNA_CENTRAL_DA_GRADE_GLOBAL),
        static_cast<float>(Compartilhado::Constantes::LINHA_CENTRAL_DA_GRADE_GLOBAL)
    };
    EstadoDoPersonagem estadoAtual_ = EstadoDoPersonagem::Parado;
    DirecaoIsometrica direcaoAtual_ = DirecaoIsometrica::BaixoDireita;
    std::vector<Compartilhado::Geometria::PosicaoNaGradeDecimal> caminho_;
    std::size_t waypointAtual_ = 0;
    Animacao::AnimacaoIdle animacaoIdle_;
};

} // namespace MiniFazenda::Dominio::Personagem
