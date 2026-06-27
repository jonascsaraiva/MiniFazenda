#pragma once

#include "Dominio/Canteiros/EstadoDoCanteiro.hpp"
#include "Dominio/Plantas/Planta.hpp"

#include <memory>
#include <optional>

namespace MiniFazenda::Dominio::Canteiros {

class Canteiro {
public:
    Canteiro() = default;

    Canteiro(const Canteiro& outro)
        : estadoVisualAtual_(outro.estadoVisualAtual_),
          tempoDePlantioEmSegundos_(outro.tempoDePlantioEmSegundos_),
          planta_(outro.planta_ != nullptr ? outro.planta_->clonar() : nullptr) {
    }

    Canteiro& operator=(const Canteiro& outro) {
        if (this == &outro) {
            return *this;
        }

        estadoVisualAtual_ = outro.estadoVisualAtual_;
        tempoDePlantioEmSegundos_ = outro.tempoDePlantioEmSegundos_;
        planta_ = outro.planta_ != nullptr ? outro.planta_->clonar() : nullptr;
        return *this;
    }

    Canteiro(Canteiro&&) noexcept = default;
    Canteiro& operator=(Canteiro&&) noexcept = default;

    EstadoVisualDoCanteiro estadoVisualAtual() const {
        return estadoVisualAtual_;
    }

    int tempoDePlantioEmSegundos() const {
        return tempoDePlantioEmSegundos_;
    }

    int identificadorDaSemente() const {
        return planta_ != nullptr ? planta_->identificadorDaSemente() : -1;
    }

    bool estaArado() const {
        return estadoVisualAtual_ == EstadoVisualDoCanteiro::TerraArada;
    }

    bool estaMadura() const {
        return estadoVisualAtual_ == EstadoVisualDoCanteiro::PlantaMadura;
    }

    bool precisaAvancarCrescimento() const {
        return estadoVisualAtual_ == EstadoVisualDoCanteiro::SementePlantada ||
               estadoVisualAtual_ == EstadoVisualDoCanteiro::PlantaCrescendo ||
               estadoVisualAtual_ == EstadoVisualDoCanteiro::PlantaJovem ||
               estadoVisualAtual_ == EstadoVisualDoCanteiro::PlantaMadura;
    }

    bool arar() {
        if (estadoVisualAtual_ != EstadoVisualDoCanteiro::TerraVazia &&
            estadoVisualAtual_ != EstadoVisualDoCanteiro::PlantaMorta) {
            return false;
        }

        estadoVisualAtual_ = EstadoVisualDoCanteiro::TerraArada;
        tempoDePlantioEmSegundos_ = 0;
        planta_.reset();
        return true;
    }

    bool plantar(std::unique_ptr<Plantas::Planta> planta) {
        if (!estaArado() || planta == nullptr) {
            return false;
        }

        planta_ = std::move(planta);
        tempoDePlantioEmSegundos_ = 0;
        estadoVisualAtual_ = EstadoVisualDoCanteiro::SementePlantada;
        return true;
    }

    bool acelerarParaMadura() {
        if (planta_ == nullptr ||
            (estadoVisualAtual_ != EstadoVisualDoCanteiro::SementePlantada &&
             estadoVisualAtual_ != EstadoVisualDoCanteiro::PlantaCrescendo &&
             estadoVisualAtual_ != EstadoVisualDoCanteiro::PlantaJovem)) {
            return false;
        }

        tempoDePlantioEmSegundos_ = planta_->tempoParaMaturar();
        estadoVisualAtual_ = EstadoVisualDoCanteiro::PlantaMadura;
        return true;
    }

    std::optional<Plantas::RecompensaDaColheita> colher() {
        if (!estaMadura() || planta_ == nullptr) {
            return std::nullopt;
        }

        const Plantas::RecompensaDaColheita recompensa = planta_->recompensaDaColheita();
        transformarEmTerraVazia();
        return recompensa;
    }

    bool avancarUmSegundo() {
        if (!precisaAvancarCrescimento() || planta_ == nullptr) {
            return false;
        }

        ++tempoDePlantioEmSegundos_;
        estadoVisualAtual_ = planta_->estadoVisualParaTempo(tempoDePlantioEmSegundos_);
        return true;
    }

private:
    void transformarEmTerraVazia() {
        estadoVisualAtual_ = EstadoVisualDoCanteiro::TerraVazia;
        tempoDePlantioEmSegundos_ = 0;
        planta_.reset();
    }

    EstadoVisualDoCanteiro estadoVisualAtual_ = EstadoVisualDoCanteiro::TerraVazia;
    int tempoDePlantioEmSegundos_ = 0;
    std::unique_ptr<Plantas::Planta> planta_;
};

} // namespace MiniFazenda::Dominio::Canteiros
