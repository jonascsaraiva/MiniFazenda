#pragma once

#include "Dominio/Canteiros/EstadoDoCanteiro.hpp"

#include <memory>
#include <string>

namespace MiniFazenda::Dominio::Plantas {

struct RecompensaDaColheita {
    int moedas = 0;
    int experiencia = 0;
};

class Planta {
public:
    virtual ~Planta() = default;

    virtual std::unique_ptr<Planta> clonar() const = 0;
    virtual int identificadorDaSemente() const = 0;
    virtual std::string nome() const = 0;
    virtual std::string pastaDeSprites() const = 0;
    virtual int custoEmMoedas() const = 0;
    virtual int tempoParaCrescer() const = 0;
    virtual int tempoParaFicarJovem() const {
        return tempoParaMaturar();
    }
    virtual int tempoParaMaturar() const = 0;
    virtual int tempoParaMorrer() const = 0;
    virtual RecompensaDaColheita recompensaDaColheita() const = 0;

    virtual Canteiros::EstadoVisualDoCanteiro estadoVisualParaTempo(int tempoDePlantioEmSegundos) const {
        if (tempoDePlantioEmSegundos >= tempoParaMorrer()) {
            return Canteiros::EstadoVisualDoCanteiro::PlantaMorta;
        }

        if (tempoDePlantioEmSegundos >= tempoParaMaturar()) {
            return Canteiros::EstadoVisualDoCanteiro::PlantaMadura;
        }

        if (tempoDePlantioEmSegundos >= tempoParaFicarJovem()) {
            return Canteiros::EstadoVisualDoCanteiro::PlantaJovem;
        }

        if (tempoDePlantioEmSegundos >= tempoParaCrescer()) {
            return Canteiros::EstadoVisualDoCanteiro::PlantaCrescendo;
        }

        return Canteiros::EstadoVisualDoCanteiro::SementePlantada;
    }
};

} // namespace MiniFazenda::Dominio::Plantas
