#pragma once

#include "Apresentacao/Camera/CameraDoJogo.hpp"
#include "Apresentacao/ConfiguracoesDoLayout.hpp"
#include "Compartilhado/ConstantesDoJogo.hpp"

#include <algorithm>
#include <cctype>
#include <exception>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>

namespace MiniFazenda::Infraestrutura::Configuracao {

inline std::string removerEspacosDasBordas(std::string texto) {
    auto naoEspaco = [](unsigned char caractere) {
        return !std::isspace(caractere);
    };

    texto.erase(texto.begin(), std::find_if(texto.begin(), texto.end(), naoEspaco));
    texto.erase(std::find_if(texto.rbegin(), texto.rend(), naoEspaco).base(), texto.end());
    return texto;
}

inline void aplicarValorInteiroSeExistir(
    const std::unordered_map<std::string, std::string>& valores,
    const std::string& chave,
    int& destino
) {
    const auto iterador = valores.find(chave);
    if (iterador == valores.end()) {
        return;
    }

    try {
        destino = std::stoi(iterador->second);
    } catch (const std::exception&) {
    }
}

inline void aplicarValorTextoSeExistir(
    const std::unordered_map<std::string, std::string>& valores,
    const std::string& chave,
    std::string& destino
) {
    const auto iterador = valores.find(chave);
    if (iterador != valores.end() && !iterador->second.empty()) {
        destino = iterador->second;
    }
}

inline bool carregarConfiguracoesDoLayout(
    const std::filesystem::path& caminhoDoArquivo,
    Apresentacao::ConfiguracoesDoLayout& configuracoes
) {
    std::ifstream arquivo(caminhoDoArquivo);
    if (!arquivo.is_open()) {
        return false;
    }

    std::unordered_map<std::string, std::string> valores;
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

        valores[chave] = valorEmTexto;
    }

    aplicarValorInteiroSeExistir(valores, "centroVisualBackgroundX", configuracoes.centroVisualBackgroundX);
    aplicarValorInteiroSeExistir(valores, "centroVisualBackgroundY", configuracoes.centroVisualBackgroundY);
    aplicarValorInteiroSeExistir(valores, "deslocamentoGradeHorizontal", configuracoes.origemGradeHorizontal);
    aplicarValorInteiroSeExistir(valores, "deslocamentoGradeVertical", configuracoes.origemGradeVertical);
    Apresentacao::Camera::aplicarOrigemCentradaDaGrade(configuracoes, Compartilhado::Constantes::TAMANHO_INICIAL_GRID);
    aplicarValorInteiroSeExistir(valores, "origemGradeHorizontal", configuracoes.origemGradeHorizontal);
    aplicarValorInteiroSeExistir(valores, "origemGradeVertical", configuracoes.origemGradeVertical);
    aplicarValorTextoSeExistir(valores, "arquivoBackgroundPrincipal", configuracoes.arquivoBackgroundPrincipal);

    return true;
}

} // namespace MiniFazenda::Infraestrutura::Configuracao
