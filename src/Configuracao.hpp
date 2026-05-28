#pragma once

#include "Tipos.hpp"

#include <algorithm>
#include <cctype>
#include <exception>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>

namespace Configuracao {

inline std::string removerEspacosDasBordas(std::string texto) {
    auto naoEspaco = [](unsigned char caractere) {
        return !std::isspace(caractere);
    };

    texto.erase(texto.begin(), std::find_if(texto.begin(), texto.end(), naoEspaco));
    texto.erase(std::find_if(texto.rbegin(), texto.rend(), naoEspaco).base(), texto.end());
    return texto;
}

inline void aplicarValorSeExistir(
    const std::unordered_map<std::string, int>& valores,
    const std::string& chave,
    int& destino
) {
    const auto iterador = valores.find(chave);
    if (iterador != valores.end()) {
        destino = iterador->second;
    }
}

inline bool carregarConfiguracoesDoLayout(
    const std::filesystem::path& caminhoDoArquivo,
    ConfiguracoesDoLayout& configuracoes
) {
    std::ifstream arquivo(caminhoDoArquivo);
    if (!arquivo.is_open()) {
        return false;
    }

    std::unordered_map<std::string, int> valores;
    std::string linha;

    while (std::getline(arquivo, linha)) {
        const std::size_t comentario = linha.find_first_of("#;");
        if (comentario != std::string::npos) {
            linha = linha.substr(0, comentario);
        }

        linha = removerEspacosDasBordas(linha);
        if (linha.empty() || (linha.front() == '[' && linha.back() == ']')) {
            continue;
        }

        const std::size_t separador = linha.find('=');
        if (separador == std::string::npos) {
            continue;
        }

        const std::string chave = removerEspacosDasBordas(linha.substr(0, separador));
        const std::string valorEmTexto = removerEspacosDasBordas(linha.substr(separador + 1));
        if (chave.empty() || valorEmTexto.empty()) {
            continue;
        }

        try {
            valores[chave] = std::stoi(valorEmTexto);
        } catch (const std::exception&) {
        }
    }

    aplicarValorSeExistir(valores, "deslocamentoGradeHorizontal", configuracoes.deslocamentoGradeHorizontal);
    aplicarValorSeExistir(valores, "deslocamentoGradeVertical", configuracoes.deslocamentoGradeVertical);
    aplicarValorSeExistir(valores, "posicaoCasaHorizontal", configuracoes.posicaoCasaHorizontal);
    aplicarValorSeExistir(valores, "posicaoCasaVertical", configuracoes.posicaoCasaVertical);
    aplicarValorSeExistir(valores, "tamanhoLarguraCasa", configuracoes.tamanhoLarguraCasa);
    aplicarValorSeExistir(valores, "tamanhoAlturaCasa", configuracoes.tamanhoAlturaCasa);
    aplicarValorSeExistir(valores, "posicaoCasinhaHorizontal", configuracoes.posicaoCasinhaHorizontal);
    aplicarValorSeExistir(valores, "posicaoCasinhaVertical", configuracoes.posicaoCasinhaVertical);
    aplicarValorSeExistir(valores, "tamanhoLarguraCasinha", configuracoes.tamanhoLarguraCasinha);
    aplicarValorSeExistir(valores, "tamanhoAlturaCasinha", configuracoes.tamanhoAlturaCasinha);

    return true;
}

} // namespace Configuracao
