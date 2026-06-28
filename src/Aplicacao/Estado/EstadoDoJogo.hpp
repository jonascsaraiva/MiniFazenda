#pragma once

#include "Compartilhado/Constantes.hpp"
#include "Dominio/Ferramentas/TipoDeFerramenta.hpp"
#include "Dominio/Grade/GradeGlobalDeCanteiros.hpp"
#include "Dominio/Jogador/Jogador.hpp"

#include <optional>

namespace MiniFazenda::Aplicacao::Estado {

class EstadoDoJogo {
public:
    const Dominio::Grade::GradeGlobalDeCanteiros& grade() const {
        return grade_;
    }

    Dominio::Grade::GradeGlobalDeCanteiros& grade() {
        return grade_;
    }

    const Dominio::Jogador::Jogador& jogador() const {
        return jogador_;
    }

    Dominio::Jogador::Jogador& jogador() {
        return jogador_;
    }

    Dominio::Ferramentas::TipoDeFerramenta ferramentaSelecionada() const {
        return ferramentaSelecionada_;
    }

    void selecionarFerramenta(Dominio::Ferramentas::TipoDeFerramenta ferramenta) {
        ferramentaSelecionada_ = ferramenta;
    }

    std::optional<int> identificadorDaSementeSelecionada() const {
        return identificadorDaSementeSelecionada_;
    }

    void selecionarSemente(int identificadorDaSemente) {
        identificadorDaSementeSelecionada_ = identificadorDaSemente;
    }

    int tamanhoAtualDoGrid() const {
        return tamanhoAtualDoGrid_;
    }

    void definirTamanhoAtualDoGrid(int tamanhoAtualDoGrid) {
        tamanhoAtualDoGrid_ = Dominio::Grade::GradeGlobalDeCanteiros::normalizarTamanhoDaGradeAtual(tamanhoAtualDoGrid);
    }

    float acumuladorDeSegundos() const {
        return acumuladorDeSegundos_;
    }

    void adicionarAoAcumuladorDeSegundos(float deltaTime) {
        acumuladorDeSegundos_ += deltaTime;
    }

    void consumirSegundoDoAcumulador() {
        acumuladorDeSegundos_ -= 1.0f;
    }

private:
    Dominio::Grade::GradeGlobalDeCanteiros grade_;
    Dominio::Jogador::Jogador jogador_;
    Dominio::Ferramentas::TipoDeFerramenta ferramentaSelecionada_ = Dominio::Ferramentas::TipoDeFerramenta::Cursor;
    std::optional<int> identificadorDaSementeSelecionada_;
    int tamanhoAtualDoGrid_ = Compartilhado::Constantes::TAMANHO_INICIAL_GRID;
    float acumuladorDeSegundos_ = 0.0f;
};

} // namespace MiniFazenda::Aplicacao::Estado
