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
        Compartilhado::Geometria::PosicaoNaGradeDeOcupacao posicao
    ) const = 0;
};

inline bool posicaoDeOcupacaoPodeReceberAcaoDaFerramenta(
    const Mapa::MapaDaFazenda& mapa,
    Compartilhado::Geometria::PosicaoNaGradeDeOcupacao posicao,
    int tamanhoAtualDoGrid
) {
    (void)mapa;
    return Mapa::MapaDaFazenda::posicaoEstaDentroDoMapaGlobal(posicao) &&
           Mapa::MapaDaFazenda::posicaoDeOcupacaoEstaDentroDaAreaJogavel(posicao, tamanhoAtualDoGrid);
}

} // namespace MiniFazenda::Dominio::Ferramentas
