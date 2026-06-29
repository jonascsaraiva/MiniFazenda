#pragma once

#include "Aplicacao/Estado/EstadoDoJogo.hpp"

#include <cstddef>

namespace MiniFazenda::Aplicacao::Servicos {

inline void avancarCrescimentoDosCanteiros(
    Dominio::Grade::GradeGlobalDeCanteiros& grade,
    int tamanhoAtualDoGrid
) {
    std::size_t indice = 0;

    while (indice < grade.posicoesDeCanteirosEmCrescimento().size()) {
        const Compartilhado::Geometria::PosicaoNaGrade posicao =
            grade.posicoesDeCanteirosEmCrescimento()[indice];
        Dominio::Grade::TileDeTerra* tile = grade.obterTile(posicao);

        const bool tileInvalido =
            tile == nullptr ||
            !tile->existeNoMapa() ||
            !tile->canteiro().precisaAvancarCrescimento();

        if (tileInvalido) {
            grade.removerCanteiroDaListaDeCrescimento(posicao);
            continue;
        }

        if (!Dominio::Grade::GradeGlobalDeCanteiros::posicaoEstaDentroDaGradeAtual(posicao, tamanhoAtualDoGrid)) {
            ++indice;
            continue;
        }

        Dominio::Canteiros::Canteiro* canteiro = grade.obterCanteiro(posicao);
        if (canteiro == nullptr || !canteiro->avancarUmSegundo()) {
            grade.removerCanteiroDaListaDeCrescimento(posicao);
            continue;
        }

        if (!canteiro->precisaAvancarCrescimento()) {
            grade.removerCanteiroDaListaDeCrescimento(posicao);
            continue;
        }

        ++indice;
    }
}

inline void avancarTempoDoJogo(Estado::EstadoDoJogo& jogo, float deltaTime) {
    jogo.personagem().avancarAnimacaoIdle(deltaTime);
    jogo.adicionarAoAcumuladorDeSegundos(deltaTime);

    while (jogo.acumuladorDeSegundos() >= 1.0f) {
        avancarCrescimentoDosCanteiros(jogo.grade(), jogo.tamanhoAtualDoGrid());
        jogo.consumirSegundoDoAcumulador();
    }
}

} // namespace MiniFazenda::Aplicacao::Servicos
