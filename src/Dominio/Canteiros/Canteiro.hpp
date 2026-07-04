#pragma once

#include "Dominio/Canteiros/EstadoDoCanteiro.hpp"
#include "Dominio/Plantas/Planta.hpp"

#include <algorithm>
#include <memory>
#include <optional>
#include <string>

namespace MiniFazenda::Dominio::Canteiros {

enum class EstadoLogicoDaPlantaNoCanteiro {
    Ausente,
    Crescendo,
    ProntaParaColheita,
    Morta
};

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

    bool possuiPlanta() const {
        return planta_ != nullptr;
    }

    std::optional<std::string> nomeDaPlantaAtual() const {
        if (planta_ == nullptr) {
            return std::nullopt;
        }

        return planta_->nome();
    }

    std::optional<int> percentualDeCrescimentoAteColheita() const {
        if (planta_ == nullptr) {
            return std::nullopt;
        }

        const int tempoParaMaturar = planta_->tempoParaMaturar();
        if (tempoParaMaturar <= 0) {
            return 100;
        }

        const int tempoLimitado = std::clamp(tempoDePlantioEmSegundos_, 0, tempoParaMaturar);
        return (tempoLimitado * 100) / tempoParaMaturar;
    }

    EstadoLogicoDaPlantaNoCanteiro estadoLogicoDaPlantaAtual() const {
        if (planta_ == nullptr) {
            return EstadoLogicoDaPlantaNoCanteiro::Ausente;
        }

        if (estadoVisualAtual_ == EstadoVisualDoCanteiro::PlantaMorta) {
            return EstadoLogicoDaPlantaNoCanteiro::Morta;
        }

        if (estadoVisualAtual_ == EstadoVisualDoCanteiro::PlantaMadura) {
            return EstadoLogicoDaPlantaNoCanteiro::ProntaParaColheita;
        }

        return EstadoLogicoDaPlantaNoCanteiro::Crescendo;
    }

    bool estaArado() const {
        return estadoVisualAtual_ == EstadoVisualDoCanteiro::TerraArada;
    }

    bool estaMadura() const {
        return estadoVisualAtual_ == EstadoVisualDoCanteiro::PlantaMadura;
    }

    bool estaComPlantaMorta() const {
        return estadoVisualAtual_ == EstadoVisualDoCanteiro::PlantaMorta;
    }

    bool estaComRestos() const {
        return estadoVisualAtual_ == EstadoVisualDoCanteiro::Restos;
    }

    bool precisaAvancarCrescimento() const {
        return estadoVisualAtual_ == EstadoVisualDoCanteiro::SementePlantada ||
               estadoVisualAtual_ == EstadoVisualDoCanteiro::PlantaCrescendo ||
               estadoVisualAtual_ == EstadoVisualDoCanteiro::PlantaJovem ||
               estadoVisualAtual_ == EstadoVisualDoCanteiro::PlantaMadura;
    }

    bool arar() {
        if (estadoVisualAtual_ != EstadoVisualDoCanteiro::TerraVazia) {
            return false;
        }

        estadoVisualAtual_ = EstadoVisualDoCanteiro::TerraArada;
        tempoDePlantioEmSegundos_ = 0;
        planta_.reset();
        return true;
    }

    bool limparPlantaMorta() {
        if (!estaComPlantaMorta()) {
            return false;
        }

        transformarEmRestos();
        return true;
    }

    bool limparRestos() {
        if (!estaComRestos()) {
            return false;
        }

        transformarEmTerraVazia();
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
        transformarEmRestos();
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

    void transformarEmRestos() {
        estadoVisualAtual_ = EstadoVisualDoCanteiro::Restos;
        tempoDePlantioEmSegundos_ = 0;
        planta_.reset();
    }

    EstadoVisualDoCanteiro estadoVisualAtual_ = EstadoVisualDoCanteiro::TerraVazia;
    int tempoDePlantioEmSegundos_ = 0;
    std::unique_ptr<Plantas::Planta> planta_;
};

} // namespace MiniFazenda::Dominio::Canteiros
