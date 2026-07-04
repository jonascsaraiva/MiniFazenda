#pragma once

#include "Dominio/Plantas/Especies/PlantaMirtilo.hpp"
#include "Dominio/Plantas/Planta.hpp"

#include <array>
#include <memory>
#include <string>
#include <vector>

namespace MiniFazenda::Apresentacao::Interface::Loja {

enum class AbaPrincipalDaLoja {
    Sementes,
    Animais,
    Construcoes,
    Decoracoes
};

enum class FiltroDeSementesDaLoja {
    Todos,
    Frutas,
    Vegetais,
    Graos
};

inline constexpr std::array<AbaPrincipalDaLoja, 4> ABAS_PRINCIPAIS_DA_LOJA = {
    AbaPrincipalDaLoja::Sementes,
    AbaPrincipalDaLoja::Animais,
    AbaPrincipalDaLoja::Construcoes,
    AbaPrincipalDaLoja::Decoracoes
};

inline constexpr std::array<FiltroDeSementesDaLoja, 4> FILTROS_DE_SEMENTES_DA_LOJA = {
    FiltroDeSementesDaLoja::Todos,
    FiltroDeSementesDaLoja::Frutas,
    FiltroDeSementesDaLoja::Vegetais,
    FiltroDeSementesDaLoja::Graos
};

struct ItemDeSementeDaLoja {
    int identificadorDaSemente = -1;
    std::string nome;
    int precoEmMoedas = 0;
    FiltroDeSementesDaLoja filtro = FiltroDeSementesDaLoja::Todos;
};

inline FiltroDeSementesDaLoja filtroPadraoParaSemente(int identificadorDaSemente) {
    if (identificadorDaSemente == Dominio::Plantas::Especies::PlantaMirtilo::IDENTIFICADOR_DA_SEMENTE) {
        return FiltroDeSementesDaLoja::Frutas;
    }

    return FiltroDeSementesDaLoja::Todos;
}

inline bool itemDeSementePassaNoFiltro(
    const ItemDeSementeDaLoja& item,
    FiltroDeSementesDaLoja filtro
) {
    if (filtro == FiltroDeSementesDaLoja::Todos) {
        return true;
    }

    return item.filtro == filtro;
}

inline std::vector<ItemDeSementeDaLoja> criarItensDeSementesDaLoja(
    const std::vector<std::unique_ptr<Dominio::Plantas::Planta>>& especiesDisponiveis
) {
    std::vector<ItemDeSementeDaLoja> itens;

    for (const std::unique_ptr<Dominio::Plantas::Planta>& especie : especiesDisponiveis) {
        if (especie == nullptr) {
            continue;
        }

        const int identificadorDaSemente = especie->identificadorDaSemente();
        itens.push_back(ItemDeSementeDaLoja{
            identificadorDaSemente,
            especie->nome(),
            especie->custoEmMoedas(),
            filtroPadraoParaSemente(identificadorDaSemente)
        });
    }

    return itens;
}

} // namespace MiniFazenda::Apresentacao::Interface::Loja
