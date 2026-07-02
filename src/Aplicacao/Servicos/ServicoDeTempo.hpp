#pragma once

#include "Aplicacao/Estado/EstadoDoJogo.hpp"

#include <cstddef>

namespace MiniFazenda::Aplicacao::Servicos {

inline void avancarCrescimentoDosCanteiros(
    Dominio::Mapa::MapaDaFazenda& mapa,
    int tamanhoAtualDoGrid
) {
    std::size_t indice = 0;

    while (indice < mapa.identificadoresDeCanteirosEmCrescimento().size()) {
        const Dominio::Ocupacao::IdentificadorDeEntidadeDeMapa identificador =
            mapa.identificadoresDeCanteirosEmCrescimento()[indice];
        Dominio::Mapa::EntidadeDoMapa* entidade = mapa.obterEntidade(identificador);
        Dominio::Canteiros::Canteiro* canteiro =
            entidade != nullptr ? mapa.obterCanteiroAgricola(identificador) : nullptr;

        const bool canteiroInvalido =
            entidade == nullptr ||
            !entidade->ehCanteiroAgricola() ||
            canteiro == nullptr ||
            !canteiro->precisaAvancarCrescimento();

        if (canteiroInvalido) {
            mapa.removerCanteiroDaListaDeCrescimento(identificador);
            continue;
        }

        if (!Dominio::Mapa::MapaDaFazenda::areaDeOcupacaoEstaDentroDaAreaJogavel(
                entidade->areaDeOcupacao(),
                tamanhoAtualDoGrid
            )) {
            ++indice;
            continue;
        }

        if (canteiro == nullptr || !canteiro->avancarUmSegundo()) {
            mapa.removerCanteiroDaListaDeCrescimento(identificador);
            continue;
        }

        if (!canteiro->precisaAvancarCrescimento()) {
            mapa.removerCanteiroDaListaDeCrescimento(identificador);
            continue;
        }

        ++indice;
    }
}

inline void avancarTempoDoJogo(Estado::EstadoDoJogo& jogo, float deltaTime) {
    jogo.personagem().avancarMovimento(deltaTime);
    jogo.adicionarAoAcumuladorDeSegundos(deltaTime);

    while (jogo.acumuladorDeSegundos() >= 1.0f) {
        avancarCrescimentoDosCanteiros(jogo.mapa(), jogo.tamanhoAtualDoGrid());
        jogo.consumirSegundoDoAcumulador();
    }
}

} // namespace MiniFazenda::Aplicacao::Servicos
