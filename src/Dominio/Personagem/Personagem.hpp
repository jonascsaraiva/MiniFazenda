#pragma once

#include "Compartilhado/ConstantesDoJogo.hpp"
#include "Compartilhado/Geometria/Posicoes.hpp"
#include "Dominio/Ocupacao/GridDeOcupacao.hpp"

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

class Personagem {
public:
    Compartilhado::Geometria::PosicaoNaGradeDeOcupacao posicaoNaGradeDeOcupacao() const {
        return Compartilhado::Geometria::PosicaoNaGradeDeOcupacao{
            static_cast<int>(std::lround(posicaoDosPesNaGradeDeOcupacao_.indiceColuna)),
            static_cast<int>(std::lround(posicaoDosPesNaGradeDeOcupacao_.indiceLinha))
        };
    }

    Compartilhado::Geometria::PosicaoDecimalNaGradeDeOcupacao posicaoDosPesNaGradeDeOcupacao() const {
        return posicaoDosPesNaGradeDeOcupacao_;
    }

    EstadoDoPersonagem estadoAtual() const {
        return estadoAtual_;
    }

    DirecaoIsometrica direcaoAtual() const {
        return direcaoAtual_;
    }

    bool estaAndando() const {
        return estadoAtual_ == EstadoDoPersonagem::Andando;
    }

    void caminharAte(Compartilhado::Geometria::PosicaoNaGradeDeOcupacao destino) {
        const Compartilhado::Geometria::PosicaoDecimalNaGradeDeOcupacao destinoDosPes{
            static_cast<float>(destino.indiceColuna),
            static_cast<float>(destino.indiceLinha)
        };

        caminho_.clear();
        waypointAtual_ = 0;

        const float diferencaColuna = destinoDosPes.indiceColuna - posicaoDosPesNaGradeDeOcupacao_.indiceColuna;
        const float diferencaLinha = destinoDosPes.indiceLinha - posicaoDosPesNaGradeDeOcupacao_.indiceLinha;

        if (std::abs(diferencaColuna) <=
                Compartilhado::Constantes::TOLERANCIA_CHEGADA_PERSONAGEM_EM_UNIDADES_DE_OCUPACAO &&
            std::abs(diferencaLinha) <=
                Compartilhado::Constantes::TOLERANCIA_CHEGADA_PERSONAGEM_EM_UNIDADES_DE_OCUPACAO) {
            posicaoDosPesNaGradeDeOcupacao_ = destinoDosPes;
            parar();
            return;
        }

        if (std::abs(diferencaColuna) > std::abs(diferencaLinha)) {
            adicionarWaypointSeNecessario(
                Compartilhado::Geometria::PosicaoDecimalNaGradeDeOcupacao{
                    destinoDosPes.indiceColuna,
                    posicaoDosPesNaGradeDeOcupacao_.indiceLinha
                }
            );
            adicionarWaypointSeNecessario(destinoDosPes);
        } else {
            adicionarWaypointSeNecessario(
                Compartilhado::Geometria::PosicaoDecimalNaGradeDeOcupacao{
                    posicaoDosPesNaGradeDeOcupacao_.indiceColuna,
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

        float distanciaRestante =
            Compartilhado::Constantes::VELOCIDADE_PERSONAGEM_EM_UNIDADES_DE_OCUPACAO_POR_SEGUNDO * deltaTime;

        while (distanciaRestante > 0.0f && estadoAtual_ == EstadoDoPersonagem::Andando) {
            const Compartilhado::Geometria::PosicaoDecimalNaGradeDeOcupacao destinoAtual = caminho_[waypointAtual_];
            const float diferencaColuna =
                destinoAtual.indiceColuna - posicaoDosPesNaGradeDeOcupacao_.indiceColuna;
            const float diferencaLinha =
                destinoAtual.indiceLinha - posicaoDosPesNaGradeDeOcupacao_.indiceLinha;
            const float distanciaAteWaypoint = std::sqrt(
                diferencaColuna * diferencaColuna + diferencaLinha * diferencaLinha
            );

            if (distanciaAteWaypoint <=
                    Compartilhado::Constantes::TOLERANCIA_CHEGADA_PERSONAGEM_EM_UNIDADES_DE_OCUPACAO ||
                distanciaAteWaypoint <= distanciaRestante) {
                posicaoDosPesNaGradeDeOcupacao_ = destinoAtual;
                distanciaRestante = std::max(0.0f, distanciaRestante - distanciaAteWaypoint);
                avancarParaProximoWaypoint();
                continue;
            }

            posicaoDosPesNaGradeDeOcupacao_.indiceColuna +=
                (diferencaColuna / distanciaAteWaypoint) * distanciaRestante;
            posicaoDosPesNaGradeDeOcupacao_.indiceLinha +=
                (diferencaLinha / distanciaAteWaypoint) * distanciaRestante;
            atualizarDirecaoPara(destinoAtual);
            distanciaRestante = 0.0f;
        }
    }

private:
    static Compartilhado::Geometria::PosicaoDecimalNaGradeDeOcupacao calcularPosicaoInicialDosPes() {
        const Compartilhado::Geometria::PosicaoNaGradeDeOcupacao origemDoCanteiroCentral =
            Ocupacao::converterCanteiroParaOcupacao(
                Compartilhado::Geometria::PosicaoDeCanteiroNoMapa{
                    Compartilhado::Constantes::COLUNA_CENTRAL_DA_GRADE_GLOBAL,
                    Compartilhado::Constantes::LINHA_CENTRAL_DA_GRADE_GLOBAL
                }
            );

        // Centro do canteiro legado quando a malha 2x2 passa a usar centros de unidades 1x1.
        return Compartilhado::Geometria::PosicaoDecimalNaGradeDeOcupacao{
            static_cast<float>(origemDoCanteiroCentral.indiceColuna + 1),
            static_cast<float>(origemDoCanteiroCentral.indiceLinha)
        };
    }

    void adicionarWaypointSeNecessario(Compartilhado::Geometria::PosicaoDecimalNaGradeDeOcupacao waypoint) {
        const Compartilhado::Geometria::PosicaoDecimalNaGradeDeOcupacao referencia =
            caminho_.empty() ? posicaoDosPesNaGradeDeOcupacao_ : caminho_.back();

        const float diferencaColuna = waypoint.indiceColuna - referencia.indiceColuna;
        const float diferencaLinha = waypoint.indiceLinha - referencia.indiceLinha;

        if (std::abs(diferencaColuna) <=
                Compartilhado::Constantes::TOLERANCIA_CHEGADA_PERSONAGEM_EM_UNIDADES_DE_OCUPACAO &&
            std::abs(diferencaLinha) <=
                Compartilhado::Constantes::TOLERANCIA_CHEGADA_PERSONAGEM_EM_UNIDADES_DE_OCUPACAO) {
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
    }

    void atualizarDirecaoPara(Compartilhado::Geometria::PosicaoDecimalNaGradeDeOcupacao destinoAtual) {
        const float diferencaColuna =
            destinoAtual.indiceColuna - posicaoDosPesNaGradeDeOcupacao_.indiceColuna;
        const float diferencaLinha =
            destinoAtual.indiceLinha - posicaoDosPesNaGradeDeOcupacao_.indiceLinha;

        if (std::abs(diferencaColuna) > std::abs(diferencaLinha)) {
            direcaoAtual_ = diferencaColuna >= 0.0f
                ? DirecaoIsometrica::BaixoDireita
                : DirecaoIsometrica::CimaEsquerda;
            return;
        }

        if (std::abs(diferencaLinha) >
            Compartilhado::Constantes::TOLERANCIA_CHEGADA_PERSONAGEM_EM_UNIDADES_DE_OCUPACAO) {
            direcaoAtual_ = diferencaLinha >= 0.0f
                ? DirecaoIsometrica::BaixoEsquerda
                : DirecaoIsometrica::CimaDireita;
        }
    }

    Compartilhado::Geometria::PosicaoDecimalNaGradeDeOcupacao posicaoDosPesNaGradeDeOcupacao_ =
        calcularPosicaoInicialDosPes();
    EstadoDoPersonagem estadoAtual_ = EstadoDoPersonagem::Parado;
    DirecaoIsometrica direcaoAtual_ = DirecaoIsometrica::BaixoDireita;
    std::vector<Compartilhado::Geometria::PosicaoDecimalNaGradeDeOcupacao> caminho_;
    std::size_t waypointAtual_ = 0;
};

} // namespace MiniFazenda::Dominio::Personagem
