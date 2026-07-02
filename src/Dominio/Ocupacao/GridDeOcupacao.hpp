#pragma once

#include "Compartilhado/ConstantesDoJogo.hpp"
#include "Compartilhado/Geometria/Posicoes.hpp"

#include <algorithm>
#include <cstddef>
#include <optional>
#include <vector>

namespace MiniFazenda::Dominio::Ocupacao {

using Compartilhado::Geometria::AreaNaGradeDeOcupacao;
using Compartilhado::Geometria::PosicaoDeCanteiroNoMapa;
using Compartilhado::Geometria::PosicaoNaGradeDeOcupacao;

using IdentificadorDeEntidadeDeMapa = int;

constexpr IdentificadorDeEntidadeDeMapa IDENTIFICADOR_INVALIDO_DE_ENTIDADE_DE_MAPA = 0;

inline bool areaDeOcupacaoEhValida(AreaNaGradeDeOcupacao area) {
    return area.largura > 0 && area.altura > 0;
}

inline bool areaDeOcupacaoContem(
    AreaNaGradeDeOcupacao area,
    PosicaoNaGradeDeOcupacao posicao
) {
    return areaDeOcupacaoEhValida(area) &&
           posicao.indiceColuna >= area.indiceColuna &&
           posicao.indiceColuna < area.indiceColuna + area.largura &&
           posicao.indiceLinha >= area.indiceLinha &&
           posicao.indiceLinha < area.indiceLinha + area.altura;
}

inline int calcularProfundidadeDaBase(AreaNaGradeDeOcupacao area) {
    if (!areaDeOcupacaoEhValida(area)) {
        return -1;
    }

    return area.indiceColuna + area.largura - 1 + area.indiceLinha + area.altura - 1;
}

inline PosicaoNaGradeDeOcupacao converterCanteiroParaOcupacao(PosicaoDeCanteiroNoMapa posicaoDoCanteiro) {
    return PosicaoNaGradeDeOcupacao{
        posicaoDoCanteiro.indiceColuna * Compartilhado::Constantes::UNIDADES_DE_OCUPACAO_POR_CANTEIRO,
        posicaoDoCanteiro.indiceLinha * Compartilhado::Constantes::UNIDADES_DE_OCUPACAO_POR_CANTEIRO
    };
}

inline std::optional<PosicaoDeCanteiroNoMapa> converterOcupacaoAlinhadaParaCanteiro(
    PosicaoNaGradeDeOcupacao origem
) {
    if (origem.indiceColuna % Compartilhado::Constantes::UNIDADES_DE_OCUPACAO_POR_CANTEIRO != 0 ||
        origem.indiceLinha % Compartilhado::Constantes::UNIDADES_DE_OCUPACAO_POR_CANTEIRO != 0) {
        return std::nullopt;
    }

    return PosicaoDeCanteiroNoMapa{
        origem.indiceColuna / Compartilhado::Constantes::UNIDADES_DE_OCUPACAO_POR_CANTEIRO,
        origem.indiceLinha / Compartilhado::Constantes::UNIDADES_DE_OCUPACAO_POR_CANTEIRO
    };
}

inline AreaNaGradeDeOcupacao calcularAreaDeOcupacao(
    PosicaoNaGradeDeOcupacao origem,
    int largura,
    int altura
) {
    return AreaNaGradeDeOcupacao{
        origem.indiceColuna,
        origem.indiceLinha,
        largura,
        altura
    };
}

inline AreaNaGradeDeOcupacao calcularAreaDeOcupacaoDoCanteiro(PosicaoNaGradeDeOcupacao origem) {
    return calcularAreaDeOcupacao(
        origem,
        Compartilhado::Constantes::UNIDADES_DE_OCUPACAO_POR_CANTEIRO,
        Compartilhado::Constantes::UNIDADES_DE_OCUPACAO_POR_CANTEIRO
    );
}

inline AreaNaGradeDeOcupacao calcularAreaDeOcupacaoDoCanteiro(PosicaoDeCanteiroNoMapa posicaoDoCanteiro) {
    return calcularAreaDeOcupacaoDoCanteiro(converterCanteiroParaOcupacao(posicaoDoCanteiro));
}

struct RegistroDeOcupacao {
    IdentificadorDeEntidadeDeMapa identificador = IDENTIFICADOR_INVALIDO_DE_ENTIDADE_DE_MAPA;
    AreaNaGradeDeOcupacao area;

    bool ocupa(PosicaoNaGradeDeOcupacao posicao) const {
        return areaDeOcupacaoContem(area, posicao);
    }

    int profundidadeDaBase() const {
        return calcularProfundidadeDaBase(area);
    }
};

class GridDeOcupacao {
public:
    GridDeOcupacao()
        : GridDeOcupacao(
              Compartilhado::Constantes::QUANTIDADE_DE_COLUNAS_DA_GRADE_DE_OCUPACAO_GLOBAL,
              Compartilhado::Constantes::QUANTIDADE_DE_LINHAS_DA_GRADE_DE_OCUPACAO_GLOBAL
          ) {
    }

    GridDeOcupacao(int quantidadeDeColunas, int quantidadeDeLinhas)
        : quantidadeDeColunas_(std::max(0, quantidadeDeColunas)),
          quantidadeDeLinhas_(std::max(0, quantidadeDeLinhas)),
          ocupacao_(
              static_cast<std::size_t>(quantidadeDeColunas_) * static_cast<std::size_t>(quantidadeDeLinhas_),
              IDENTIFICADOR_INVALIDO_DE_ENTIDADE_DE_MAPA
          ) {
    }

    int quantidadeDeColunas() const {
        return quantidadeDeColunas_;
    }

    int quantidadeDeLinhas() const {
        return quantidadeDeLinhas_;
    }

    const std::vector<RegistroDeOcupacao>& ocupacoes() const {
        return ocupacoes_;
    }

    std::size_t quantidadeDeOcupacoes() const {
        return ocupacoes_.size();
    }

    bool posicaoEstaDentroDaGrade(PosicaoNaGradeDeOcupacao posicao) const {
        return posicao.indiceColuna >= 0 &&
               posicao.indiceColuna < quantidadeDeColunas_ &&
               posicao.indiceLinha >= 0 &&
               posicao.indiceLinha < quantidadeDeLinhas_;
    }

    bool areaEstaDentroDaGrade(AreaNaGradeDeOcupacao area) const {
        return areaDeOcupacaoEhValida(area) &&
               area.indiceColuna >= 0 &&
               area.indiceLinha >= 0 &&
               area.indiceColuna + area.largura <= quantidadeDeColunas_ &&
               area.indiceLinha + area.altura <= quantidadeDeLinhas_;
    }

    bool areaEstaLivre(AreaNaGradeDeOcupacao area) const {
        if (!areaEstaDentroDaGrade(area)) {
            return false;
        }

        for (int linha = area.indiceLinha; linha < area.indiceLinha + area.altura; ++linha) {
            for (int coluna = area.indiceColuna; coluna < area.indiceColuna + area.largura; ++coluna) {
                if (ocupacao_[calcularIndiceLinear(PosicaoNaGradeDeOcupacao{coluna, linha})] !=
                    IDENTIFICADOR_INVALIDO_DE_ENTIDADE_DE_MAPA) {
                    return false;
                }
            }
        }

        return true;
    }

    bool registrarOcupacao(
        IdentificadorDeEntidadeDeMapa identificador,
        AreaNaGradeDeOcupacao area
    ) {
        if (identificador == IDENTIFICADOR_INVALIDO_DE_ENTIDADE_DE_MAPA ||
            ocupacaoPorIdentificador(identificador) != nullptr ||
            !areaEstaLivre(area)) {
            return false;
        }

        ocupacoes_.push_back(RegistroDeOcupacao{identificador, area});
        ocuparArea(area, identificador);
        return true;
    }

    bool removerOcupacao(IdentificadorDeEntidadeDeMapa identificador) {
        const auto iterador = std::find_if(
            ocupacoes_.begin(),
            ocupacoes_.end(),
            [identificador](const RegistroDeOcupacao& ocupacao) {
                return ocupacao.identificador == identificador;
            }
        );

        if (iterador == ocupacoes_.end()) {
            return false;
        }

        liberarArea(iterador->area, identificador);
        ocupacoes_.erase(iterador);
        return true;
    }

    const RegistroDeOcupacao* ocupacaoPorIdentificador(IdentificadorDeEntidadeDeMapa identificador) const {
        const auto iterador = std::find_if(
            ocupacoes_.begin(),
            ocupacoes_.end(),
            [identificador](const RegistroDeOcupacao& ocupacao) {
                return ocupacao.identificador == identificador;
            }
        );

        return iterador != ocupacoes_.end() ? &(*iterador) : nullptr;
    }

    std::optional<IdentificadorDeEntidadeDeMapa> identificadorEm(PosicaoNaGradeDeOcupacao posicao) const {
        if (!posicaoEstaDentroDaGrade(posicao)) {
            return std::nullopt;
        }

        const IdentificadorDeEntidadeDeMapa identificador = ocupacao_[calcularIndiceLinear(posicao)];
        if (identificador == IDENTIFICADOR_INVALIDO_DE_ENTIDADE_DE_MAPA) {
            return std::nullopt;
        }

        return identificador;
    }

    const RegistroDeOcupacao* ocupacaoEm(PosicaoNaGradeDeOcupacao posicao) const {
        const std::optional<IdentificadorDeEntidadeDeMapa> identificador = identificadorEm(posicao);
        return identificador.has_value() ? ocupacaoPorIdentificador(*identificador) : nullptr;
    }

private:
    std::size_t calcularIndiceLinear(PosicaoNaGradeDeOcupacao posicao) const {
        return static_cast<std::size_t>(posicao.indiceLinha) * static_cast<std::size_t>(quantidadeDeColunas_) +
               static_cast<std::size_t>(posicao.indiceColuna);
    }

    void ocuparArea(AreaNaGradeDeOcupacao area, IdentificadorDeEntidadeDeMapa identificador) {
        for (int linha = area.indiceLinha; linha < area.indiceLinha + area.altura; ++linha) {
            for (int coluna = area.indiceColuna; coluna < area.indiceColuna + area.largura; ++coluna) {
                ocupacao_[calcularIndiceLinear(PosicaoNaGradeDeOcupacao{coluna, linha})] = identificador;
            }
        }
    }

    void liberarArea(AreaNaGradeDeOcupacao area, IdentificadorDeEntidadeDeMapa identificador) {
        for (int linha = area.indiceLinha; linha < area.indiceLinha + area.altura; ++linha) {
            for (int coluna = area.indiceColuna; coluna < area.indiceColuna + area.largura; ++coluna) {
                const PosicaoNaGradeDeOcupacao posicao{coluna, linha};
                IdentificadorDeEntidadeDeMapa& ocupante = ocupacao_[calcularIndiceLinear(posicao)];
                if (ocupante == identificador) {
                    ocupante = IDENTIFICADOR_INVALIDO_DE_ENTIDADE_DE_MAPA;
                }
            }
        }
    }

    int quantidadeDeColunas_ = 0;
    int quantidadeDeLinhas_ = 0;
    std::vector<IdentificadorDeEntidadeDeMapa> ocupacao_;
    std::vector<RegistroDeOcupacao> ocupacoes_;
};

} // namespace MiniFazenda::Dominio::Ocupacao
