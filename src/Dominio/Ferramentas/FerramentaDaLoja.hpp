#pragma once

#include "Dominio/Ferramentas/Ferramenta.hpp"

namespace MiniFazenda::Dominio::Ferramentas {

class FerramentaDaLoja final : public Ferramenta {
public:
    TipoDeFerramenta tipo() const override {
        return TipoDeFerramenta::Loja;
    }

    ResultadoDaFerramenta aplicar(
        ContextoDaFerramenta& contexto,
        Compartilhado::Geometria::PosicaoDeCanteiroNoMapa posicao
    ) const override {
        (void)contexto;
        (void)posicao;
        return ResultadoDaFerramenta{};
    }
};

} // namespace MiniFazenda::Dominio::Ferramentas
