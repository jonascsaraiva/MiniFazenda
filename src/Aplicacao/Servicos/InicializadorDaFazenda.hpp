#pragma once

#include "Aplicacao/Estado/EstadoDoJogo.hpp"
#include "Compartilhado/ConstantesDoJogo.hpp"
#include "Compartilhado/Geometria/Posicoes.hpp"

namespace MiniFazenda::Aplicacao::Servicos {

inline Dominio::Mapa::MapaDaFazenda criarMapaDaFazendaComNucleoInicial() {
    Dominio::Mapa::MapaDaFazenda mapa;

    for (int linha = 0; linha < Compartilhado::Constantes::QUANTIDADE_DE_LINHAS_DO_NUCLEO_INICIAL; ++linha) {
        for (int coluna = 0; coluna < Compartilhado::Constantes::QUANTIDADE_DE_COLUNAS_DO_NUCLEO_INICIAL; ++coluna) {
            const Compartilhado::Geometria::PosicaoDeCanteiroNoMapa posicao{
                Compartilhado::Constantes::COLUNA_INICIAL_DO_NUCLEO_INICIAL + coluna,
                Compartilhado::Constantes::LINHA_INICIAL_DO_NUCLEO_INICIAL + linha
            };

            mapa.criarCanteiro(posicao, Compartilhado::Constantes::TAMANHO_INICIAL_GRID);
        }
    }

    return mapa;
}

inline Estado::EstadoDoJogo criarEstadoInicialDoJogo() {
    Estado::EstadoDoJogo jogo;
    jogo.mapa() = criarMapaDaFazendaComNucleoInicial();
    jogo.definirTamanhoAtualDoGrid(Compartilhado::Constantes::TAMANHO_INICIAL_GRID);
    return jogo;
}

} // namespace MiniFazenda::Aplicacao::Servicos
