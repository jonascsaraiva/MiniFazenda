#pragma once

namespace Constantes {

constexpr int LARGURA_DA_JANELA = 1280;
constexpr int ALTURA_DA_JANELA = 720;
constexpr int TAXA_DE_QUADROS_DESEJADA = 60;
constexpr int MILISSEGUNDOS_POR_QUADRO = 1000 / TAXA_DE_QUADROS_DESEJADA;

constexpr int QUANTIDADE_DE_COLUNAS_DA_GRADE = 8;
constexpr int QUANTIDADE_DE_LINHAS_DA_GRADE = 8;
constexpr int LARGURA_DO_CANTEIRO = 128;
constexpr int ALTURA_DO_CANTEIRO = 64;

constexpr int TEMPO_PARA_CRESCER = 6;
constexpr int TEMPO_PARA_MADURAR = 14;
constexpr int TEMPO_PARA_MORRER = 34;

constexpr int TAMANHO_DO_BOTAO_DA_INTERFACE = 52;
constexpr int ESPACAMENTO_DOS_BOTOES = 12;

constexpr const char* TITULO_DA_JANELA = "MiniFazenda2 v1.0.0";

} // namespace Constantes

