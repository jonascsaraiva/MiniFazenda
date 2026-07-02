#pragma once

#include "Compartilhado/Constantes.hpp"
#include "Compartilhado/Geometria/Posicoes.hpp"
#include "Dominio/Canteiros/Canteiro.hpp"
#include "Dominio/Ocupacao/GridDeOcupacao.hpp"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <optional>
#include <vector>

namespace MiniFazenda::Dominio::Mapa {

using Compartilhado::Geometria::AreaNaGradeDeOcupacao;
using Compartilhado::Geometria::PosicaoDeCanteiroNoMapa;
using Compartilhado::Geometria::PosicaoNaGradeDeOcupacao;
using Ocupacao::IdentificadorDeEntidadeDeMapa;

constexpr std::size_t INDICE_INVALIDO_DA_LISTA_DO_MAPA = std::numeric_limits<std::size_t>::max();

enum class TipoDeEntidadeDoMapa {
    Canteiro,
    Arvore,
    Casa,
    Cerca,
    Animal,
    Decoracao,
    ObjetoInterativo
};

class EntidadeDoMapa {
public:
    IdentificadorDeEntidadeDeMapa identificador() const {
        return identificador_;
    }

    TipoDeEntidadeDoMapa tipo() const {
        return tipo_;
    }

    AreaNaGradeDeOcupacao areaDeOcupacao() const {
        return areaDeOcupacao_;
    }

    std::optional<PosicaoDeCanteiroNoMapa> posicaoDoCanteiroNoMapa() const {
        return posicaoDoCanteiroNoMapa_;
    }

    const Canteiros::Canteiro* canteiroAgricola() const {
        return canteiroAgricola_.has_value() ? &(*canteiroAgricola_) : nullptr;
    }

    bool ehCanteiroAgricola() const {
        return tipo_ == TipoDeEntidadeDoMapa::Canteiro && canteiroAgricola_.has_value();
    }

    bool ocupa(PosicaoNaGradeDeOcupacao posicao) const {
        return Ocupacao::areaDeOcupacaoContem(areaDeOcupacao_, posicao);
    }

    int profundidadeDaBase() const {
        return Ocupacao::calcularProfundidadeDaBase(areaDeOcupacao_);
    }

private:
    friend class MapaDaFazenda;

    static EntidadeDoMapa criarCanteiro(
        IdentificadorDeEntidadeDeMapa identificador,
        PosicaoDeCanteiroNoMapa posicao
    ) {
        EntidadeDoMapa entidade;
        entidade.identificador_ = identificador;
        entidade.tipo_ = TipoDeEntidadeDoMapa::Canteiro;
        entidade.areaDeOcupacao_ = Ocupacao::calcularAreaDeOcupacaoDoCanteiro(posicao);
        entidade.posicaoDoCanteiroNoMapa_ = posicao;
        entidade.canteiroAgricola_ = Canteiros::Canteiro{};
        return entidade;
    }

    Canteiros::Canteiro* canteiroAgricolaMutavel() {
        return canteiroAgricola_.has_value() ? &(*canteiroAgricola_) : nullptr;
    }

    IdentificadorDeEntidadeDeMapa identificador_ = Ocupacao::IDENTIFICADOR_INVALIDO_DE_ENTIDADE_DE_MAPA;
    TipoDeEntidadeDoMapa tipo_ = TipoDeEntidadeDoMapa::Decoracao;
    AreaNaGradeDeOcupacao areaDeOcupacao_;
    std::optional<PosicaoDeCanteiroNoMapa> posicaoDoCanteiroNoMapa_;
    std::optional<Canteiros::Canteiro> canteiroAgricola_;
    std::size_t indiceNaListaDeCrescimento_ = INDICE_INVALIDO_DA_LISTA_DO_MAPA;
};

class MapaDaFazenda {
public:
    static bool posicaoEstaDentroDoMapaGlobal(PosicaoDeCanteiroNoMapa posicao) {
        return posicao.indiceColuna >= 0 &&
               posicao.indiceColuna < Compartilhado::Constantes::QUANTIDADE_DE_COLUNAS_DA_GRADE_GLOBAL &&
               posicao.indiceLinha >= 0 &&
               posicao.indiceLinha < Compartilhado::Constantes::QUANTIDADE_DE_LINHAS_DA_GRADE_GLOBAL;
    }

    static int normalizarTamanhoDaAreaJogavel(int tamanhoAtualDoGrid) {
        const int tamanhoLimitado = std::clamp(
            tamanhoAtualDoGrid,
            Compartilhado::Constantes::TAMANHO_INICIAL_GRID,
            Compartilhado::Constantes::TAMANHO_MAXIMO_GRID
        );

        if ((tamanhoLimitado - Compartilhado::Constantes::TAMANHO_INICIAL_GRID) %
                Compartilhado::Constantes::INCREMENTO_TAMANHO_GRID ==
            0) {
            return tamanhoLimitado;
        }

        const int deslocamento = tamanhoLimitado - Compartilhado::Constantes::TAMANHO_INICIAL_GRID;
        return Compartilhado::Constantes::TAMANHO_INICIAL_GRID +
               (deslocamento / Compartilhado::Constantes::INCREMENTO_TAMANHO_GRID) *
                   Compartilhado::Constantes::INCREMENTO_TAMANHO_GRID;
    }

    static int calcularColunaInicialDaAreaJogavel(int tamanhoAtualDoGrid) {
        const int tamanhoNormalizado = normalizarTamanhoDaAreaJogavel(tamanhoAtualDoGrid);
        return Compartilhado::Constantes::COLUNA_CENTRAL_DA_GRADE_GLOBAL - tamanhoNormalizado / 2;
    }

    static int calcularLinhaInicialDaAreaJogavel(int tamanhoAtualDoGrid) {
        const int tamanhoNormalizado = normalizarTamanhoDaAreaJogavel(tamanhoAtualDoGrid);
        return Compartilhado::Constantes::LINHA_CENTRAL_DA_GRADE_GLOBAL - tamanhoNormalizado / 2;
    }

    static bool posicaoEstaDentroDaAreaJogavel(
        PosicaoDeCanteiroNoMapa posicao,
        int tamanhoAtualDoGrid
    ) {
        const int tamanhoNormalizado = normalizarTamanhoDaAreaJogavel(tamanhoAtualDoGrid);
        const int colunaInicial = calcularColunaInicialDaAreaJogavel(tamanhoNormalizado);
        const int linhaInicial = calcularLinhaInicialDaAreaJogavel(tamanhoNormalizado);

        return posicao.indiceColuna >= colunaInicial &&
               posicao.indiceColuna < colunaInicial + tamanhoNormalizado &&
               posicao.indiceLinha >= linhaInicial &&
               posicao.indiceLinha < linhaInicial + tamanhoNormalizado;
    }

    const std::vector<EntidadeDoMapa>& entidades() const {
        return entidades_;
    }

    const std::vector<IdentificadorDeEntidadeDeMapa>& identificadoresDeCanteirosEmCrescimento() const {
        return identificadoresDeCanteirosEmCrescimento_;
    }

    const Ocupacao::GridDeOcupacao& indiceDeOcupacao() const {
        return indiceDeOcupacao_;
    }

    std::size_t quantidadeDeEntidades() const {
        return entidades_.size();
    }

    std::size_t quantidadeDeCanteiros() const {
        return static_cast<std::size_t>(std::count_if(
            entidades_.begin(),
            entidades_.end(),
            [](const EntidadeDoMapa& entidade) {
                return entidade.ehCanteiroAgricola();
            }
        ));
    }

    std::size_t quantidadeDeCanteirosEmCrescimento() const {
        return identificadoresDeCanteirosEmCrescimento_.size();
    }

    bool areaEstaLivre(AreaNaGradeDeOcupacao area) const {
        return indiceDeOcupacao_.areaEstaLivre(area);
    }

    std::optional<IdentificadorDeEntidadeDeMapa> criarCanteiro(PosicaoDeCanteiroNoMapa posicao) {
        if (!posicaoEstaDentroDoMapaGlobal(posicao)) {
            return std::nullopt;
        }

        const AreaNaGradeDeOcupacao area = Ocupacao::calcularAreaDeOcupacaoDoCanteiro(posicao);
        if (!indiceDeOcupacao_.areaEstaLivre(area)) {
            return std::nullopt;
        }

        const IdentificadorDeEntidadeDeMapa identificador = proximoIdentificador_++;
        EntidadeDoMapa entidade = EntidadeDoMapa::criarCanteiro(identificador, posicao);
        entidades_.push_back(entidade);

        if (!indiceDeOcupacao_.registrarOcupacao(identificador, area)) {
            entidades_.pop_back();
            return std::nullopt;
        }

        return identificador;
    }

    bool removerEntidade(IdentificadorDeEntidadeDeMapa identificador) {
        const auto iterador = std::find_if(
            entidades_.begin(),
            entidades_.end(),
            [identificador](const EntidadeDoMapa& entidade) {
                return entidade.identificador() == identificador;
            }
        );

        if (iterador == entidades_.end()) {
            return false;
        }

        removerCanteiroDaListaDeCrescimento(identificador);
        indiceDeOcupacao_.removerOcupacao(identificador);
        entidades_.erase(iterador);
        return true;
    }

    bool removerCanteiro(PosicaoDeCanteiroNoMapa posicao) {
        const EntidadeDoMapa* entidade = entidadeEmCanteiro(posicao);
        if (entidade == nullptr || !entidade->ehCanteiroAgricola()) {
            return false;
        }

        return removerEntidade(entidade->identificador());
    }

    const EntidadeDoMapa* obterEntidade(IdentificadorDeEntidadeDeMapa identificador) const {
        const auto iterador = std::find_if(
            entidades_.begin(),
            entidades_.end(),
            [identificador](const EntidadeDoMapa& entidade) {
                return entidade.identificador() == identificador;
            }
        );

        return iterador != entidades_.end() ? &(*iterador) : nullptr;
    }

    EntidadeDoMapa* obterEntidade(IdentificadorDeEntidadeDeMapa identificador) {
        return const_cast<EntidadeDoMapa*>(
            static_cast<const MapaDaFazenda&>(*this).obterEntidade(identificador)
        );
    }

    const EntidadeDoMapa* entidadeEm(PosicaoNaGradeDeOcupacao posicao) const {
        const std::optional<IdentificadorDeEntidadeDeMapa> identificador =
            indiceDeOcupacao_.identificadorEm(posicao);
        return identificador.has_value() ? obterEntidade(*identificador) : nullptr;
    }

    const EntidadeDoMapa* entidadeEmCanteiro(PosicaoDeCanteiroNoMapa posicao) const {
        return entidadeEm(Ocupacao::converterCanteiroParaOcupacao(posicao));
    }

    EntidadeDoMapa* entidadeEmCanteiro(PosicaoDeCanteiroNoMapa posicao) {
        return const_cast<EntidadeDoMapa*>(
            static_cast<const MapaDaFazenda&>(*this).entidadeEmCanteiro(posicao)
        );
    }

    const Canteiros::Canteiro* obterCanteiroAgricola(PosicaoDeCanteiroNoMapa posicao) const {
        const EntidadeDoMapa* entidade = entidadeEmCanteiro(posicao);
        return entidade != nullptr ? entidade->canteiroAgricola() : nullptr;
    }

    Canteiros::Canteiro* obterCanteiroAgricola(PosicaoDeCanteiroNoMapa posicao) {
        EntidadeDoMapa* entidade = entidadeEmCanteiro(posicao);
        return entidade != nullptr ? entidade->canteiroAgricolaMutavel() : nullptr;
    }

    bool existeCanteiroEm(PosicaoDeCanteiroNoMapa posicao) const {
        return obterCanteiroAgricola(posicao) != nullptr;
    }

    void sincronizarCrescimentoDoCanteiro(PosicaoDeCanteiroNoMapa posicao) {
        EntidadeDoMapa* entidade = entidadeEmCanteiro(posicao);
        if (entidade == nullptr || !entidade->ehCanteiroAgricola()) {
            return;
        }

        const Canteiros::Canteiro* canteiro = entidade->canteiroAgricola();
        if (canteiro != nullptr && canteiro->precisaAvancarCrescimento()) {
            registrarCanteiroEmCrescimento(entidade->identificador());
            return;
        }

        removerCanteiroDaListaDeCrescimento(entidade->identificador());
    }

    void removerCanteiroDaListaDeCrescimento(IdentificadorDeEntidadeDeMapa identificador) {
        EntidadeDoMapa* entidade = obterEntidade(identificador);
        if (entidade == nullptr || entidade->indiceNaListaDeCrescimento_ == INDICE_INVALIDO_DA_LISTA_DO_MAPA) {
            return;
        }

        const std::size_t indiceRemovido = entidade->indiceNaListaDeCrescimento_;
        if (indiceRemovido >= identificadoresDeCanteirosEmCrescimento_.size()) {
            entidade->indiceNaListaDeCrescimento_ = INDICE_INVALIDO_DA_LISTA_DO_MAPA;
            return;
        }

        const std::size_t ultimoIndice = identificadoresDeCanteirosEmCrescimento_.size() - 1;

        if (indiceRemovido != ultimoIndice) {
            const IdentificadorDeEntidadeDeMapa identificadorMovido =
                identificadoresDeCanteirosEmCrescimento_[ultimoIndice];
            identificadoresDeCanteirosEmCrescimento_[indiceRemovido] = identificadorMovido;

            EntidadeDoMapa* entidadeMovida = obterEntidade(identificadorMovido);
            if (entidadeMovida != nullptr) {
                entidadeMovida->indiceNaListaDeCrescimento_ = indiceRemovido;
            }
        }

        identificadoresDeCanteirosEmCrescimento_.pop_back();
        entidade->indiceNaListaDeCrescimento_ = INDICE_INVALIDO_DA_LISTA_DO_MAPA;
    }

private:
    void registrarCanteiroEmCrescimento(IdentificadorDeEntidadeDeMapa identificador) {
        EntidadeDoMapa* entidade = obterEntidade(identificador);
        if (entidade == nullptr ||
            !entidade->ehCanteiroAgricola() ||
            entidade->indiceNaListaDeCrescimento_ != INDICE_INVALIDO_DA_LISTA_DO_MAPA) {
            return;
        }

        const Canteiros::Canteiro* canteiro = entidade->canteiroAgricola();
        if (canteiro == nullptr || !canteiro->precisaAvancarCrescimento()) {
            return;
        }

        entidade->indiceNaListaDeCrescimento_ = identificadoresDeCanteirosEmCrescimento_.size();
        identificadoresDeCanteirosEmCrescimento_.push_back(identificador);
    }

    Ocupacao::GridDeOcupacao indiceDeOcupacao_;
    std::vector<EntidadeDoMapa> entidades_;
    std::vector<IdentificadorDeEntidadeDeMapa> identificadoresDeCanteirosEmCrescimento_;
    IdentificadorDeEntidadeDeMapa proximoIdentificador_ = 1;
};

} // namespace MiniFazenda::Dominio::Mapa
