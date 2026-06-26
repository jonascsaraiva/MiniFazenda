#pragma once

#include "Compartilhado/Geometria/Posicoes.hpp"
#include "Dominio/Ferramentas/ResultadoDaFerramenta.hpp"
#include "Dominio/Ferramentas/TipoDeFerramenta.hpp"
#include "Dominio/Grade/GradeGlobalDeCanteiros.hpp"
#include "Dominio/Jogador/Jogador.hpp"

namespace MiniFazenda::Dominio::Ferramentas {

struct ContextoDaFerramenta {
    Grade::GradeGlobalDeCanteiros& grade;
    Jogador::Jogador& jogador;
    int tamanhoAtualDoGrid = 0;
};

class Ferramenta {
public:
    virtual ~Ferramenta() = default;

    virtual TipoDeFerramenta tipo() const = 0;
    virtual ResultadoDaFerramenta aplicar(
        ContextoDaFerramenta& contexto,
        Compartilhado::Geometria::PosicaoNaGrade posicao
    ) const = 0;
};

inline bool posicaoPodeReceberAcaoDaFerramenta(
    const Grade::GradeGlobalDeCanteiros& grade,
    Compartilhado::Geometria::PosicaoNaGrade posicao,
    int tamanhoAtualDoGrid
) {
    (void)grade;
    return Grade::GradeGlobalDeCanteiros::posicaoEstaDentroDaGradeGlobal(posicao) &&
           Grade::GradeGlobalDeCanteiros::posicaoEstaDentroDaGradeAtual(posicao, tamanhoAtualDoGrid);
}

} // namespace MiniFazenda::Dominio::Ferramentas
