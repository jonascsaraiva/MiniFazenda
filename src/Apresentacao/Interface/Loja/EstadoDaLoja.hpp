#pragma once

#include "Apresentacao/Interface/Loja/TiposDaLoja.hpp"

#include <optional>

namespace MiniFazenda::Apresentacao::Interface::Loja {

class EstadoDaLoja {
public:
    bool aberta() const {
        return aberta_;
    }

    void abrir() {
        aberta_ = true;
    }

    void fechar() {
        aberta_ = false;
        identificadorDaSementeSobHover_.reset();
    }

    AbaPrincipalDaLoja abaPrincipalSelecionada() const {
        return abaPrincipalSelecionada_;
    }

    void selecionarAbaPrincipal(AbaPrincipalDaLoja aba) {
        abaPrincipalSelecionada_ = aba;
        identificadorDaSementeSobHover_.reset();
    }

    FiltroDeSementesDaLoja filtroDeSementesSelecionado() const {
        return filtroDeSementesSelecionado_;
    }

    void selecionarFiltroDeSementes(FiltroDeSementesDaLoja filtro) {
        filtroDeSementesSelecionado_ = filtro;
        identificadorDaSementeSobHover_.reset();
    }

    std::optional<int> identificadorDaSementeSobHover() const {
        return identificadorDaSementeSobHover_;
    }

    void definirSementeSobHover(std::optional<int> identificadorDaSemente) {
        identificadorDaSementeSobHover_ = identificadorDaSemente;
    }

    std::optional<int> identificadorDaSementeSelecionadaVisualmente() const {
        return identificadorDaSementeSelecionadaVisualmente_;
    }

    void definirSementeSelecionadaVisualmente(int identificadorDaSemente) {
        identificadorDaSementeSelecionadaVisualmente_ = identificadorDaSemente;
    }

private:
    bool aberta_ = false;
    AbaPrincipalDaLoja abaPrincipalSelecionada_ = AbaPrincipalDaLoja::Sementes;
    FiltroDeSementesDaLoja filtroDeSementesSelecionado_ = FiltroDeSementesDaLoja::Todos;
    std::optional<int> identificadorDaSementeSobHover_;
    std::optional<int> identificadorDaSementeSelecionadaVisualmente_;
};

} // namespace MiniFazenda::Apresentacao::Interface::Loja
