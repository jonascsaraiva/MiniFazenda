#pragma once

#include "Aplicacao/Estado/EstadoDoJogo.hpp"

#include <cstddef>
#include <optional>

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
        const std::optional<Compartilhado::Geometria::PosicaoDeCanteiroNoMapa> posicaoDaEntidade =
            entidade != nullptr ? entidade->posicaoDoCanteiroNoMapa() : std::nullopt;
        Dominio::Canteiros::Canteiro* canteiro =
            posicaoDaEntidade.has_value() ? mapa.obterCanteiroAgricola(*posicaoDaEntidade) : nullptr;

        const bool canteiroInvalido =
            entidade == nullptr ||
            !entidade->ehCanteiroAgricola() ||
            !posicaoDaEntidade.has_value() ||
            canteiro == nullptr ||
            !canteiro->precisaAvancarCrescimento();

        if (canteiroInvalido) {
            mapa.removerCanteiroDaListaDeCrescimento(identificador);
            continue;
        }

        const Compartilhado::Geometria::PosicaoDeCanteiroNoMapa posicao =
            *posicaoDaEntidade;

        if (!Dominio::Mapa::MapaDaFazenda::posicaoEstaDentroDaAreaJogavel(posicao, tamanhoAtualDoGrid)) {
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
