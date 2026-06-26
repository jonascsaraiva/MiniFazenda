#pragma once

namespace MiniFazenda::Dominio::Ferramentas {

enum class AcaoDaFerramenta {
    Nenhuma,
    CriarTerra,
    ArarTerra,
    RemoverTerra,
    Plantar,
    AcelerarCrescimento,
    Colher
};

struct ResultadoDaFerramenta {
    AcaoDaFerramenta acao = AcaoDaFerramenta::Nenhuma;

    bool houveMudanca() const {
        return acao != AcaoDaFerramenta::Nenhuma;
    }
};

} // namespace MiniFazenda::Dominio::Ferramentas
