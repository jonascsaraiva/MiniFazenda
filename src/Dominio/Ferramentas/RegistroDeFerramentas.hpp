#pragma once

#include "Dominio/Ferramentas/CursorDeColheita.hpp"
#include "Dominio/Ferramentas/Enxada.hpp"
#include "Dominio/Ferramentas/FerramentaDeSemente.hpp"
#include "Dominio/Ferramentas/PresenteAcelerador.hpp"
#include "Dominio/Ferramentas/RemovedorDeTerra.hpp"

#include <array>
#include <memory>

namespace MiniFazenda::Dominio::Ferramentas {

class RegistroDeFerramentas {
public:
    RegistroDeFerramentas() {
        registrar(std::make_unique<CursorDeColheita>());
        registrar(std::make_unique<Enxada>());
        registrar(std::make_unique<RemovedorDeTerra>());
        registrar(std::make_unique<FerramentaDeSemente>());
        registrar(std::make_unique<PresenteAcelerador>());
    }

    const Ferramenta& obter(TipoDeFerramenta tipo) const {
        const std::size_t indice = indiceDaFerramenta(tipo);
        return *ferramentas_[indice];
    }

private:
    void registrar(std::unique_ptr<Ferramenta> ferramenta) {
        const std::size_t indice = indiceDaFerramenta(ferramenta->tipo());
        ferramentas_[indice] = std::move(ferramenta);
    }

    std::array<std::unique_ptr<Ferramenta>, QUANTIDADE_DE_FERRAMENTAS> ferramentas_{};
};

} // namespace MiniFazenda::Dominio::Ferramentas
