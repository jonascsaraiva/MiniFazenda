#pragma once

#include "Aplicacao/Estado/EstadoDoJogo.hpp"
#include "Compartilhado/Geometria/Posicoes.hpp"
#include "Dominio/Ferramentas/RegistroDeFerramentas.hpp"

namespace MiniFazenda::Aplicacao::Servicos {

inline Dominio::Ferramentas::ResultadoDaFerramenta aplicarFerramentaNoJogo(
    Estado::EstadoDoJogo& jogo,
    Compartilhado::Geometria::PosicaoNaGrade posicao
) {
    static const Dominio::Ferramentas::RegistroDeFerramentas ferramentas;

    Dominio::Ferramentas::ContextoDaFerramenta contexto{
        jogo.grade(),
        jogo.jogador(),
        jogo.tamanhoAtualDoGrid()
    };

    return ferramentas.obter(jogo.ferramentaSelecionada()).aplicar(contexto, posicao);
}

} // namespace MiniFazenda::Aplicacao::Servicos
