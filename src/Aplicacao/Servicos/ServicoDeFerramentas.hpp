#pragma once

#include "Aplicacao/Estado/EstadoDoJogo.hpp"
#include "Compartilhado/Geometria/Posicoes.hpp"
#include "Dominio/Ferramentas/RegistroDeFerramentas.hpp"

namespace MiniFazenda::Aplicacao::Servicos {

inline Dominio::Ferramentas::ResultadoDaFerramenta aplicarFerramentaNoJogo(
    Estado::EstadoDoJogo& jogo,
    Compartilhado::Geometria::PosicaoDeCanteiroNoMapa posicao
) {
    static const Dominio::Ferramentas::RegistroDeFerramentas ferramentas;

    Dominio::Ferramentas::ContextoDaFerramenta contexto{
        jogo.mapa(),
        jogo.jogador(),
        jogo.identificadorDaSementeSelecionada(),
        jogo.tamanhoAtualDoGrid()
    };

    return ferramentas.obter(jogo.ferramentaSelecionada()).aplicar(contexto, posicao);
}

} // namespace MiniFazenda::Aplicacao::Servicos
