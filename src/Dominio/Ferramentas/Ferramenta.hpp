#pragma once

#include "Compartilhado/Geometria/Posicoes.hpp"
#include "Dominio/Ferramentas/ResultadoDaFerramenta.hpp"
#include "Dominio/Ferramentas/TipoDeFerramenta.hpp"
#include "Dominio/Jogador/Jogador.hpp"
#include "Dominio/Mapa/MapaDaFazenda.hpp"

#include <optional>

namespace MiniFazenda::Dominio::Ferramentas {

struct ContextoDaFerramenta {
    Mapa::MapaDaFazenda& mapa;
    Jogador::Jogador& jogador;
    std::optional<int> identificadorDaSementeSelecionada;
    int tamanhoAtualDoGrid = 0;
};

class Ferramenta {
public:
    virtual ~Ferramenta() = default;

    virtual TipoDeFerramenta tipo() const = 0;
    virtual ResultadoDaFerramenta aplicar(
        ContextoDaFerramenta& contexto,
        Compartilhado::Geometria::PosicaoDeCanteiroNoMapa posicao
    ) const = 0;
};

inline bool posicaoPodeReceberAcaoDaFerramenta(
    const Mapa::MapaDaFazenda& mapa,
    Compartilhado::Geometria::PosicaoDeCanteiroNoMapa posicao,
    int tamanhoAtualDoGrid
) {
    (void)mapa;
    return Mapa::MapaDaFazenda::posicaoEstaDentroDoMapaGlobal(posicao) &&
           Mapa::MapaDaFazenda::posicaoEstaDentroDaAreaJogavel(posicao, tamanhoAtualDoGrid);
}

} // namespace MiniFazenda::Dominio::Ferramentas
