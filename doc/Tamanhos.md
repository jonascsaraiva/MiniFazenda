# Medidas e posicionamento visual do jogo

Este documento consolida as medidas que governam o posicionamento visual atual do jogo. Os valores abaixo foram extraídos do código-fonte e, quando são derivados, o próprio item indica a constante ou função usada como origem do cálculo. A seção do personagem já reflete a migração para o padrão de frame `250 x 250 px`.

## Como ler este documento

A documentação foi reorganizada em blocos curtos. Cada item mantém a medida, o valor atual e a origem técnica do cálculo ou da constante. Caminhos, constantes e funções continuam destacados em formato de código para evitar ambiguidade.

## Sumário

- [1. Janela e background](#1-janela-e-background)
- [2. Canteiro isométrico no zoom inicial](#2-canteiro-isométrico-no-zoom-inicial)
- [3. Zoom e dimensões escaladas](#3-zoom-e-dimensões-escaladas)
- [4. Grade jogável, grade global e centralização](#4-grade-jogável-grade-global-e-centralização)
- [5. Plantas dentro do tile](#5-plantas-dentro-do-tile)
- [6. Personagem jogador](#6-personagem-jogador)
- [7. Interface e HUD](#7-interface-e-hud)
- [8. Observações sobre inconsistências](#8-observações-sobre-inconsistências)
- [9. Notas para arte](#9-notas-para-arte)

## 1. Janela e background

- **Largura da janela** — `1280 px`
  - Origem: `src/Compartilhado/ConstantesDaJanela.hpp::LARGURA_DA_JANELA`
- **Altura da janela** — `720 px`
  - Origem: `src/Compartilhado/ConstantesDaJanela.hpp::ALTURA_DA_JANELA`
- **Retângulo usado para desenhar o background** — `x=0, y=0, w=1280, h=720`
  - Origem: `src/Apresentacao/Renderizacao/Mundo/DesenhoDoMundo.hpp::desenharFundo`, usando `LARGURA_DA_JANELA` e `ALTURA_DA_JANELA`
- **Centro visual do background** — `x=576, y=296`
  - Origem: `src/Compartilhado/ConstantesDaIsometria.hpp::CENTRO_VISUAL_BACKGROUND_X` e `CENTRO_VISUAL_BACKGROUND_Y`; os mesmos valores aparecem em `assets/config.ini::centroVisualBackgroundX/Y`
- **Origem inicial de layout antes de centralizar a grade** — `x=576, y=296`
  - Origem: `src/Apresentacao/ConfiguracoesDoLayout.hpp::origemGradeHorizontal/origemGradeVertical`, inicializadas com `CENTRO_VISUAL_BACKGROUND_X/Y`
- **Arquivo de background configurado** — `background.png`
  - Origem: `src/Apresentacao/ConfiguracoesDoLayout.hpp::arquivoBackgroundPrincipal` e `assets/config.ini::arquivoBackgroundPrincipal`
- **Primeiro fallback de background** — `background/fundo_gramado.png`
  - Origem: `src/Infraestrutura/Assets/LocalizadorDeAssets.hpp::candidatosParaBackground`

Observação: o código sempre desenha o background no retângulo da janela (`1280 x 720 px`), independentemente do tamanho nativo do arquivo de imagem (`src/Apresentacao/Renderizacao/Mundo/DesenhoDoMundo.hpp::desenharFundo`).

## 2. Canteiro isométrico no zoom inicial

- **Largura do canteiro** — `128 px`
  - Origem: `src/Compartilhado/ConstantesDaIsometria.hpp::LARGURA_DO_CANTEIRO`
- **Altura do canteiro** — `64 px`
  - Origem: `src/Compartilhado/ConstantesDaIsometria.hpp::ALTURA_DO_CANTEIRO`
- **Proporção do losango** — `2:1`
  - Origem: `128 / 64`, derivado de `LARGURA_DO_CANTEIRO` e `ALTURA_DO_CANTEIRO`
- **Meia largura usada na isometria** — `64 px`
  - Origem: `LARGURA_DO_CANTEIRO / 2`, usado em `src/Apresentacao/Isometria/Isometrico.hpp::converterGradeParaTela`
- **Meia altura usada na isometria** — `32 px`
  - Origem: `ALTURA_DO_CANTEIRO / 2`, usado em `src/Apresentacao/Isometria/Isometrico.hpp::converterGradeParaTela`
- **Inclinação da aresta do losango** — `32 / 64 = 1 / 2`
  - Origem: `ALTURA_DO_CANTEIRO / 2` dividido por `LARGURA_DO_CANTEIRO / 2`
- **Ângulo da aresta com a horizontal** — `arctan(32 / 64)`
  - Origem: Derivado de `LARGURA_DO_CANTEIRO` e `ALTURA_DO_CANTEIRO`; a aresta avança `64 px` na horizontal e `32 px` na vertical
- **Ponto superior do losango dentro do retângulo do tile** — `(64, 0)`
  - Origem: `src/Apresentacao/Renderizacao/Primitivas/PrimitivasSDL.hpp::desenharLosango`, com `centroX = x + w / 2` e `topoY = y`
- **Ponto direito do losango dentro do retângulo do tile** — `(128, 32)`
  - Origem: `src/Apresentacao/Renderizacao/Primitivas/PrimitivasSDL.hpp::desenharLosango`, com `x + w` e `y + h / 2`
- **Ponto inferior do losango dentro do retângulo do tile** — `(64, 64)`
  - Origem: `src/Apresentacao/Renderizacao/Primitivas/PrimitivasSDL.hpp::desenharLosango`, com `x + w / 2` e `y + h`
- **Ponto esquerdo do losango dentro do retângulo do tile** — `(0, 32)`
  - Origem: `src/Apresentacao/Renderizacao/Primitivas/PrimitivasSDL.hpp::desenharLosango`, com `x` e `y + h / 2`
- **Ponto usado pelo código como base de planta e ponto legado equivalente dos pés do personagem** — `(64, 32)`
  - Origem: `src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp::calcularDestinoDoSpriteDaPlanta` e equivalencia mantida por `src/Apresentacao/Renderizacao/Mundo/RenderizadorDoPersonagem.hpp::calcularPontoDosPesDoPersonagemNaTela`
- **Distancia vertical entre a base usada pelo código e a ponta inferior** — `32 px`
  - Origem: `(64 - 32)`, derivado de `ALTURA_DO_CANTEIRO` e das funcoes `desenharLosango` e `calcularPontoDosPesDoPersonagemNaTela`

A conversão de grade para tela usa o topo-esquerda do retângulo de destino do tile. Em zoom inicial, o deslocamento horizontal entre colunas/linhas é de `64 px`, e o deslocamento vertical é de `32 px` (`src/Apresentacao/Isometria/Isometrico.hpp::converterGradeParaTela`, derivado de `LARGURA_DO_CANTEIRO / 2` e `ALTURA_DO_CANTEIRO / 2`).

- **Grade local para tela X** — `x = (coluna - linha) * 64 + deslocamentoHorizontal`
  - Origem: `src/Apresentacao/Isometria/Isometrico.hpp::converterGradeParaTela`
- **Grade local para tela Y** — `y = (coluna + linha) * 32 + deslocamentoVertical`
  - Origem: `src/Apresentacao/Isometria/Isometrico.hpp::converterGradeParaTela`
- **Grade global para local** — `colunaLocal = colunaGlobal - 127`, `linhaLocal = linhaGlobal - 127`
  - Origem: `src/Apresentacao/Isometria/Isometrico.hpp::converterGradeGlobalParaTela`, usando `COLUNA_DE_ORIGEM_VISUAL_DA_GRADE_GLOBAL` e `LINHA_DE_ORIGEM_VISUAL_DA_GRADE_GLOBAL`
- **Tela para grade decimal** — remove origem visual e camera, subtrai meia largura renderizada do X ajustado e inverte a isometria antes do `floor`
  - Origem: `src/Apresentacao/Isometria/Isometrico.hpp::converterTelaParaGradeDecimal` e `converterTelaParaGrade`
- **Tela para ocupacao global** — usa as dimensoes renderizadas arredondadas da unidade de ocupacao, remove origem visual e camera, inverte a isometria e soma a origem global de ocupacao
  - Origem: `src/Apresentacao/Isometria/Isometrico.hpp::converterTelaParaOcupacaoGlobal`, com dimensoes de `Camera::calcularDimensoesDaUnidadeDeOcupacaoRenderizada`

### Unidade de ocupacao

- **Largura da unidade de ocupacao no zoom base** - `64 px`
  - Origem: `src/Compartilhado/ConstantesDoJogo.hpp::LARGURA_DA_UNIDADE_DE_OCUPACAO`; um canteiro ocupa `2 x 2` unidades
- **Altura da unidade de ocupacao no zoom base** - `32 px`
  - Origem: `src/Compartilhado/ConstantesDoJogo.hpp::ALTURA_DA_UNIDADE_DE_OCUPACAO`; um canteiro ocupa `2 x 2` unidades
- **Origem logica do canteiro** - `PosicaoNaGradeDeOcupacao`
  - Origem: `src/Dominio/Ocupacao/GridDeOcupacao.hpp::calcularAreaDeOcupacaoDoCanteiro(PosicaoNaGradeDeOcupacao)`; a origem pode ser par ou impar

## 3. Zoom e dimensões escaladas

O tamanho renderizado do canteiro é calculado por `round(tamanhoBase * zoomAtual)`, limitado entre `ZOOM_MINIMO` e `ZOOM_MAXIMO` (`src/Apresentacao/Camera/CameraDoJogo.hpp::calcularDimensoesDoCanteiroRenderizado`).

- **Zoom mínimo** — `0.5x`
  - Origem: `src/Compartilhado/ConstantesDaCamera.hpp::ZOOM_MINIMO`
- **Zoom inicial** — `1.0x`
  - Origem: `src/Compartilhado/ConstantesDaCamera.hpp::ZOOM_INICIAL`
- **Zoom máximo** — `2.0x`
  - Origem: `src/Compartilhado/ConstantesDaCamera.hpp::ZOOM_MAXIMO`
- **Passo do zoom** — `0.1x`
  - Origem: `src/Compartilhado/ConstantesDaCamera.hpp::PASSO_DO_ZOOM`
- **Canteiro no zoom mínimo** — `64 x 32 px`
  - Origem: `round(128 * 0.5)` e `round(64 * 0.5)`, derivados de `LARGURA_DO_CANTEIRO`, `ALTURA_DO_CANTEIRO` e `ZOOM_MINIMO`
- **Canteiro no zoom inicial** — `128 x 64 px`
  - Origem: `round(128 * 1.0)` e `round(64 * 1.0)`, derivados de `LARGURA_DO_CANTEIRO`, `ALTURA_DO_CANTEIRO` e `ZOOM_INICIAL`
- **Canteiro no zoom máximo** — `256 x 128 px`
  - Origem: `round(128 * 2.0)` e `round(64 * 2.0)`, derivados de `LARGURA_DO_CANTEIRO`, `ALTURA_DO_CANTEIRO` e `ZOOM_MAXIMO`
- **Meia largura no zoom mínimo** — `32 px`
  - Origem: `64 / 2`, usado depois por `converterGradeParaTela`
- **Meia altura no zoom mínimo** — `16 px`
  - Origem: `32 / 2`, usado depois por `converterGradeParaTela`
- **Meia largura no zoom máximo** — `128 px`
  - Origem: `256 / 2`, usado depois por `converterGradeParaTela`
- **Meia altura no zoom máximo** — `64 px`
  - Origem: `128 / 2`, usado depois por `converterGradeParaTela`

### Observações de zoom

- **Larguras ímpares nos passos de zoom** — `77, 115, 141, 179, 205, 243 px`
  - Observação: Derivados de `round(128 * zoom)`, para `zoom = 0.6, 0.9, 1.1, 1.4, 1.6, 1.9`; `desenharLosango` usa `w / 2` inteiro, então o lado direito fica `1 px` mais longo nesses casos
- **Alturas ímpares nos passos de zoom** — `45, 51, 77, 83, 109, 115 px`
  - Observação: Derivados de `round(64 * zoom)`, para `zoom = 0.7, 0.8, 1.2, 1.3, 1.7, 1.8`; `desenharLosango` usa `h / 2` inteiro, então a metade inferior pode ficar `1 px` mais longa nesses casos
- **Hitbox debug em dimensões ímpares** — remove `1 px` quando `w` ou `h` é ímpar
  - Observação: `src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp::calcularHitboxDoCanteiro`, com `(destino.w / 2) * 2` e `(destino.h / 2) * 2`; `DEBUG_HITBOX_TILES` está `false` em `src/Compartilhado/ConstantesDaIsometria.hpp::DEBUG_HITBOX_TILES`
- **Clique em zoom quebrado** - a conversao inversa usa as mesmas dimensoes renderizadas arredondadas usadas no desenho, e as metades inteiras calculadas por `Isometrico::calcularMetadeDaDimensaoIsometrica`
  - Observação: a ordem logica e mouse na janela -> camera/pan -> origem visual -> dimensoes renderizadas do zoom -> inversao isometrica -> `PosicaoNaGradeDeOcupacao`
- **Zoom no ponto do cursor** - `Camera::aplicarZoomNoPonto` preserva a `PosicaoDecimalNaGradeDeOcupacao` local sob o mouse ao recomputar o offset com as novas dimensoes renderizadas
  - Observação: isso evita deriva quando `round(tamanhoBase * zoom)` e divisao inteira produzem fatores efetivos diferentes de `zoomNovo / zoomAntigo`

## 4. Grade jogável, grade global e centralização

- **Grade global alocada** — `256 x 256 tiles`
  - Origem: `src/Compartilhado/ConstantesDoJogo.hpp::QUANTIDADE_DE_COLUNAS_DA_GRADE_GLOBAL_ALOCADA` e `QUANTIDADE_DE_LINHAS_DA_GRADE_GLOBAL_ALOCADA`
- **Total de tiles alocados** — `65536 tiles`
  - Origem: `256 * 256`, derivado de `src/Compartilhado/ConstantesDoJogo.hpp::TOTAL_DE_TILES_DA_GRADE_GLOBAL`
- **Centro da grade global** — `(128, 128)`
  - Origem: `src/Compartilhado/ConstantesDoJogo.hpp::COLUNA_CENTRAL_DA_GRADE_GLOBAL` e `LINHA_CENTRAL_DA_GRADE_GLOBAL`
- **Tamanho inicial da grade jogável** — `12 x 12 tiles`
  - Origem: `src/Compartilhado/ConstantesDoJogo.hpp::TAMANHO_INICIAL_GRID`
- **Tamanho inicial em unidades de ocupacao** - `24 x 24 unidades`
  - Origem: `TAMANHO_INICIAL_GRID * UNIDADES_DE_OCUPACAO_POR_CANTEIRO`; validado por `src/Dominio/Mapa/MapaDaFazenda.hpp::calcularTamanhoDaAreaJogavelEmOcupacao`
- **Tamanho máximo da grade jogável** — `24 x 24 tiles`
  - Origem: `src/Compartilhado/ConstantesDoJogo.hpp::TAMANHO_MAXIMO_GRID`
- **Tamanho maximo em unidades de ocupacao** - `48 x 48 unidades`
  - Origem: `TAMANHO_MAXIMO_GRID * UNIDADES_DE_OCUPACAO_POR_CANTEIRO`
- **Incremento de tamanho da grade jogável** — `2 tiles`
  - Origem: `src/Compartilhado/ConstantesDoJogo.hpp::INCREMENTO_TAMANHO_GRID`
- **Tamanhos validos pela normalizacao atual** — `12, 14, 16, 18, 20, 22, 24`
  - Origem: Derivados de `TAMANHO_INICIAL_GRID`, `TAMANHO_MAXIMO_GRID`, `INCREMENTO_TAMANHO_GRID` e `src/Dominio/Mapa/MapaDaFazenda.hpp::normalizarTamanhoDaAreaJogavel`
- **Inicio global da grade jogável inicial** — `(122, 122)`
  - Origem: `src/Dominio/Mapa/MapaDaFazenda.hpp::calcularColunaInicialDaAreaJogavel` e `calcularLinhaInicialDaAreaJogavel`, com `TAMANHO_INICIAL_GRID`
- **Fim global incluso da grade jogável inicial** — `(133, 133)`
  - Origem: `(122 + 12 - 1)`, derivado de `posicaoEstaDentroDaAreaJogavel`
- **Inicio global da grade jogável maxima** — `(116, 116)`
  - Origem: `src/Dominio/Mapa/MapaDaFazenda.hpp::calcularColunaInicialDaAreaJogavel` e `calcularLinhaInicialDaAreaJogavel`, com `TAMANHO_MAXIMO_GRID`
- **Fim global incluso da grade jogável maxima** — `(139, 139)`
  - Origem: `(116 + 24 - 1)`, derivado de `posicaoEstaDentroDaAreaJogavel`
- **Nucleo inicial criado** — `2 x 2 canteiros`
  - Origem: `src/Compartilhado/ConstantesDoJogo.hpp::QUANTIDADE_DE_COLUNAS_DO_NUCLEO_INICIAL` e `QUANTIDADE_DE_LINHAS_DO_NUCLEO_INICIAL`; criado em `src/Aplicacao/Servicos/InicializadorDaFazenda.hpp::criarMapaDaFazendaComNucleoInicial`
- **Inicio global do núcleo inicial** — `(127, 127)`
  - Origem: `src/Compartilhado/ConstantesDoJogo.hpp::COLUNA_INICIAL_DO_NUCLEO_INICIAL` e `LINHA_INICIAL_DO_NUCLEO_INICIAL`
- **Origem visual global da grade** — `(127, 127)`
  - Origem: `src/Compartilhado/ConstantesDaIsometria.hpp::COLUNA_DE_ORIGEM_VISUAL_DA_GRADE_GLOBAL` e `LINHA_DE_ORIGEM_VISUAL_DA_GRADE_GLOBAL`
- **Canteiros iniciais realmente criados** — `(127,127)`, `(128,127)`, `(127,128)`, `(128,128)`
  - Origem: `src/Aplicacao/Servicos/InicializadorDaFazenda.hpp::criarMapaDaFazendaComNucleoInicial`

A grade global de coordenadas (`256 x 256 posicoes de canteiro`) e a area jogavel atual (`12 x 12` ate `24 x 24 canteiros`) nao sao a mesma coisa. A grade global e o espaco total de coordenadas; a area jogavel e uma janela quadrada centralizada dentro desse espaco, validada por `MapaDaFazenda::posicaoEstaDentroDaAreaJogavel`.

### Centralização no background

`Camera::aplicarOrigemCentradaDaGrade` recalcula `origemGradeHorizontal/origemGradeVertical` com `Camera::calcularDeslocamentoCentradoPara`. Com as constantes atuais e zoom base, a origem renderizada fica igual para os tamanhos pares usados hoje.

- **Origem da grade para `12 x 12` no zoom base** — `(512, 232)`
  - Origem: `src/Apresentacao/Camera/CameraDoJogo.hpp::calcularDeslocamentoCentradoPara`, com `TAMANHO_INICIAL_GRID`, `CENTRO_VISUAL_BACKGROUND_X/Y`, `LARGURA_DO_CANTEIRO`, `ALTURA_DO_CANTEIRO`
- **Primeiro tile da grade `12 x 12`** — `(512, -88)`
  - Origem: `src/Apresentacao/Camera/CameraDoJogo.hpp::calcularDeslocamentoCentradoPara`, com inicio global `(122,122)`
- **Retângulo da grade `12 x 12` no zoom base** — `x=-192, y=-88, w=1536, h=768`
  - Origem: `src/Apresentacao/Camera/CameraDoJogo.hpp::calcularRetanguloDaGradeRenderizada`, com `12 * 128` e `12 * 64`
- **Centro do retângulo `12 x 12`** — `(576, 296)`
  - Origem: Derivado do retângulo `x=-192, y=-88, w=1536, h=768`, alinhado a `CENTRO_VISUAL_BACKGROUND_X/Y`
- **Origem da grade para `24 x 24` no zoom base** — `(512, 232)`
  - Origem: `src/Apresentacao/Camera/CameraDoJogo.hpp::calcularDeslocamentoCentradoPara`, com `TAMANHO_MAXIMO_GRID`
- **Primeiro tile da grade `24 x 24`** — `(512, -472)`
  - Origem: `src/Apresentacao/Camera/CameraDoJogo.hpp::calcularDeslocamentoCentradoPara`, com inicio global `(116,116)`
- **Retângulo da grade `24 x 24` no zoom base** — `x=-960, y=-472, w=3072, h=1536`
  - Origem: `src/Apresentacao/Camera/CameraDoJogo.hpp::calcularRetanguloDaGradeRenderizada`, com `24 * 128` e `24 * 64`
- **Centro do retângulo `24 x 24`** — `(576, 296)`
  - Origem: Derivado do retângulo `x=-960, y=-472, w=3072, h=1536`, alinhado a `CENTRO_VISUAL_BACKGROUND_X/Y`
- **Margem horizontal minima visivel no pan** — `320 px`
  - Origem: `1280 * 0.25`, derivado de `LARGURA_DA_JANELA` em `src/Compartilhado/ConstantesDaJanela.hpp` e `FRACAO_MINIMA_VISIVEL_AO_PAN` em `src/Compartilhado/ConstantesDaCamera.hpp`
- **Margem vertical minima visivel no pan** — `180 px`
  - Origem: `720 * 0.25`, derivado de `ALTURA_DA_JANELA` em `src/Compartilhado/ConstantesDaJanela.hpp` e `FRACAO_MINIMA_VISIVEL_AO_PAN` em `src/Compartilhado/ConstantesDaCamera.hpp`

## 5. Plantas dentro do tile

### Sprites de planta com textura

- **Ponto de plantio no tile no zoom base** — `(64, 32)` relativo ao topo-esquerda do tile
  - Origem: `src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp::calcularDestinoDoSpriteDaPlanta`, com `destinoDoCanteiro.w / 2` e `destinoDoCanteiro.h / 2`
- **Escala da textura da planta** — `destinoDoCanteiro.w / sprite.largura`
  - Origem: `src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp::calcularDestinoDoSpriteDaPlanta`
- **Largura final da planta** — `round(sprite.largura * escala)`
  - Origem: `src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp::calcularDestinoDoSpriteDaPlanta`
- **Altura final da planta** — `round(sprite.altura * escala)`
  - Origem: `src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp::calcularDestinoDoSpriteDaPlanta`
- **Âncora X escalada** — `round(sprite.ancoraDaBase.x * escala)`
  - Origem: `src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp::calcularDestinoDoSpriteDaPlanta`
- **Âncora Y escalada** — `round(sprite.ancoraDaBase.y * escala)`
  - Origem: `src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp::calcularDestinoDoSpriteDaPlanta`
- **Destino final da textura** — `x = pontoDePlantioX - ancoraX`, `y = pontoDePlantioY - ancoraY`
  - Origem: `src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp::calcularDestinoDoSpriteDaPlanta`
- **Âncora calculada quando há pixels opacos** — `x = (minimoX + maximoX) / 2`, `y = maximoY`
  - Origem: `src/Infraestrutura/Assets/RecursosDaFazenda.hpp::calcularAncoraDaBaseDoSpriteDaPlanta`
- **Âncora calculada quando não há pixels opacos** — `x = sprite.largura / 2`, `y = sprite.altura`
  - Origem: `src/Infraestrutura/Assets/RecursosDaFazenda.hpp::calcularAncoraDaBaseDoSpriteDaPlanta`
- **Fallback quando o sprite não possui âncora valida** — usa o proprio retângulo do tile, `128 x 64 px` no zoom base
  - Origem: `src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp::calcularDestinoDoSpriteDaPlanta`

Conclusao para arte: a textura da planta deve ter a base visual no ponto inferior mais baixo dos pixels opacos, pois o código calcula a âncora em `y = maximoY` e alinha esse ponto ao centro do tile (`64, 32` no zoom base).

### Fallback vetorial da planta

Estes valores so aparecem quando a textura da planta não existe ou não foi carregada. Eles revelam a expectativa atual do motor para a base visual da planta.

- **Semente plantada** — `x=61, y=29, w=6, h=6`
  - Origem: `src/Apresentacao/Renderizacao/Mundo/DesenhoDoMundo.hpp::desenharFallbackDaPlanta`, com `w / 2 - 3`, `h / 2 - 3`, `6`, `6`
- **Planta crescendo, jovem ou madura, haste vertical** — `x=58, y=14, w=12, h=20`
  - Origem: `src/Apresentacao/Renderizacao/Mundo/DesenhoDoMundo.hpp::desenharFallbackDaPlanta`, com `w / 2 - 6`, `h / 2 - 18`, `12`, `20`
- **Planta crescendo, jovem ou madura, volume horizontal** — `x=46, y=23, w=36, h=8`
  - Origem: `src/Apresentacao/Renderizacao/Mundo/DesenhoDoMundo.hpp::desenharFallbackDaPlanta`, com `w / 2 - 18`, `h / 2 - 9`, `36`, `8`
- **Planta morta, haste vertical** — `x=61, y=16, w=6, h=18`
  - Origem: `src/Apresentacao/Renderizacao/Mundo/DesenhoDoMundo.hpp::desenharFallbackDaPlanta`, com `w / 2 - 3`, `h / 2 - 16`, `6`, `18`
- **Planta morta, volume horizontal** — `x=51, y=24, w=26, h=5`
  - Origem: `src/Apresentacao/Renderizacao/Mundo/DesenhoDoMundo.hpp::desenharFallbackDaPlanta`, com `w / 2 - 13`, `h / 2 - 8`, `26`, `5`

Observação: o ponto de plantio usado para sprites com textura e `(64, 32)`, mas o fallback vetorial pode ultrapassar esse ponto em `2 px` ou `3 px`, dependendo do estado visual (`src/Apresentacao/Renderizacao/Mundo/DesenhoDoMundo.hpp::desenharFallbackDaPlanta`). Isso não foi corrigido nesta tarefa.

## 6. Personagem jogador

O código de renderizacao do personagem foi localizado em `src/Apresentacao/Renderizacao/Mundo/RenderizadorDoPersonagem.hpp`. A configuração visual fica em `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp`.

### Sprite e animação atual

- **Textura usada atualmente** — `sprites/personagem/Boneco_piscando_olhos.png`
  - Origem: `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::caminhoTextura`
- **Quantidade de animacoes configuradas** — `5`
  - Origem: `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::QUANTIDADE_DE_ANIMACOES`
- **Animacoes de caminhada** — usam o mesmo idle como fallback de configuração
  - Origem: `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::configuracaoWalkBaixoDireita`, `configuracaoWalkBaixoEsquerda`, `configuracaoWalkCimaDireita`, `configuracaoWalkCimaEsquerda`
- **Frames do idle** — `5`
  - Origem: `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::quantidadeFramesIdle`
- **Origem X do primeiro frame** — `0 px`
  - Origem: `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::frameOrigemX`
- **Origem Y do primeiro frame** — `0 px`
  - Origem: `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::frameOrigemY`
- **Largura do frame atual** — `250 px`
  - Origem: `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::frameLargura`
- **Altura do frame atual** — `250 px`
  - Origem: `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::frameAltura`
- **Espacamento horizontal entre frames** — `0 px`
  - Origem: `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::frameEspacamentoX`
- **Espacamento vertical entre frames** — `0 px`
  - Origem: `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::frameEspacamentoY`
- **Duracao de referência por frame** — `0.065 s`
  - Origem: `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::duracaoPorFrame`
- **Largura minima exigida da spritesheet atual** — `1250 px`
  - Origem: `0 + (5 - 1) * (250 + 0) + 250`, derivado de `validarDimensoesDaTexturaDoPersonagem`
- **Altura minima exigida da spritesheet atual** — `250 px`
  - Origem: `0 + 250`, derivado de `validarDimensoesDaTexturaDoPersonagem`

A animação idle atual usa uma spritesheet horizontal de cinco frames. A escolha do frame fica em `src/Apresentacao/Animacao/AnimacaoIdleDoPersonagem.hpp`, e o estado visual do personagem é avançado por `src/Apresentacao/Animacao/AnimadorDoPersonagem.hpp`; o domínio informa apenas se o personagem está parado, andando e qual é a direção lógica atual.

### Destino e âncora dos pés

- **Largura de destino no zoom base** — `63 px`
  - Origem: `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::destinoLargura`
- **Altura de destino no zoom base** — `96 px`
  - Origem: `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::destinoAltura`
- **Ponto dos pés dentro do destino no zoom base** — `(32, 96)`
  - Origem: `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::pontoDosPesX` e `pontoDosPesY`
- **Offset dos pés** — `(0, 0)`
  - Origem: `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::offsetPeX` e `offsetPeY`
- **Ajuste final de tela** — `(0, 0)`
  - Origem: `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::ajusteTelaX` e `ajusteTelaY`
- **Destino no zoom mínimo** — `32 x 48 px`
  - Origem: `round(63 * 0.5)` e `round(96 * 0.5)`, derivados de `destinoLargura`, `destinoAltura` e `ZOOM_MINIMO`
- **Ponto dos pés no destino no zoom mínimo** — `(16, 48)`
  - Origem: `round(32 * 0.5)` e `round(96 * 0.5)`, derivados de `pontoDosPesX/Y` e `ZOOM_MINIMO`
- **Destino no zoom máximo** — `126 x 192 px`
  - Origem: `round(63 * 2.0)` e `round(96 * 2.0)`, derivados de `destinoLargura`, `destinoAltura` e `ZOOM_MAXIMO`
- **Ponto dos pés no destino no zoom máximo** — `(64, 192)`
  - Origem: `round(32 * 2.0)` e `round(96 * 2.0)`, derivados de `pontoDosPesX/Y` e `ZOOM_MAXIMO`
- **Ponto visual dos pés do personagem dentro da unidade de ocupacao no zoom base** — `(64, 16)` relativo ao topo-esquerda da unidade `64 x 32`
  - Origem: `src/Apresentacao/Isometria/Isometrico.hpp::converterCentroDaUnidadeDeOcupacaoGlobalParaTela` mais a centralizacao horizontal em `src/Apresentacao/Renderizacao/Mundo/RenderizadorDoPersonagem.hpp::centralizarPontoDosPesDoPersonagemNaUnidadeDeOcupacao`
- **Posicao inicial dos pés na grade de ocupacao** — `(257, 256)`
  - Origem: `src/Dominio/Personagem/Personagem.hpp::calcularPosicaoInicialDosPes`, derivada de `converterCanteiroParaOcupacao(PosicaoDeCanteiroNoMapa{128, 128})` mais `(1, 0)` para preservar o centro do canteiro legado
- **Posicao inicial visual dos pés na tela, sem pan e no zoom base** — `(608, 328)`
  - Origem: Derivado de `posicaoDosPesNaGradeDeOcupacao_`, origem `(512, 232)`, `LARGURA_DA_UNIDADE_DE_OCUPACAO` e `ALTURA_DA_UNIDADE_DE_OCUPACAO`
- **Retângulo inicial do personagem na tela, sem pan e no zoom base** — `x=576, y=232, w=63, h=96`
  - Origem: `(608 - 32, 328 - 96, 63, 96)`, derivado de `calcularRetanguloDeDestino`

### Movimento e ocupacao

O personagem usa `PosicaoNaGradeDeOcupacao` para a posicao inteira dos pes e `PosicaoDecimalNaGradeDeOcupacao` durante o movimento. Ele anda na malha menor `1 x 1` de ocupacao, mas nao e ocupante fixo do `GridDeOcupacao`.

A velocidade logica atual e `VELOCIDADE_PERSONAGEM_EM_UNIDADES_DE_OCUPACAO_POR_SEGUNDO = 10.0f`, derivada de `VELOCIDADE_PERSONAGEM_EM_CELULAS_POR_SEGUNDO = 5.0f` multiplicada por `UNIDADES_DE_OCUPACAO_POR_CANTEIRO = 2`. Isso preserva a velocidade visual aproximada da grade antiga.

Cliques no mundo para movimento usam a mesma conversao tela -> `PosicaoNaGradeDeOcupacao` usada pela malha de ocupacao. Ferramentas agricolas, preview e criacao de canteiros continuam usando a ocupacao real e nao dependem do personagem.

### Padrão de arte atual

- **Frame do personagem** — `250 x 250 px`
  - Origem: `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::frameLargura` e `frameAltura`
- **Convenção da âncora dos pés** — centro inferior do destino arredondado para inteiro, `((destinoLargura + 1) / 2, destinoAltura)`
  - Origem: `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::pontoDosPesX` e `pontoDosPesY`

## 7. Interface e HUD

Estas medidas não alteram o posicionamento do mundo, mas fazem parte da composicao visual desenhada por cima dele.

O hit-test da interface usa retangulos semiabertos como padrao: `x <= pontoX < x + largura` e `y <= pontoY < y + altura`. O limite inicial pertence a area clicavel; os limites direito e inferior nao pertencem, evitando que retangulos adjacentes compartilhem o mesmo pixel clicavel. Origem: `src/Apresentacao/Interface/AreaDeInteracao.hpp::pontoEstaNoRetanguloSemiaberto`.

### Barra de ferramentas

- **Quantidade de botões inferiores** — `5`
  - Origem: `src/Apresentacao/Interface/BarraDeFerramentas/BarraDeFerramentas.hpp::criarBotoesDaInterface`
- **Tamanho do botão** — `52 x 52 px`
  - Origem: `src/Compartilhado/ConstantesDaInterface.hpp::TAMANHO_DO_BOTAO_DA_INTERFACE`
- **Espacamento entre botões** — `12 px`
  - Origem: `src/Compartilhado/ConstantesDaInterface.hpp::ESPACAMENTO_DOS_BOTOES`
- **Largura total da barra** — `308 px`
  - Origem: `52 * 5 + 12 * (5 - 1)`, derivado de `criarBotoesDaInterface`
- **X do primeiro botão** — `486 px`
  - Origem: `1280 / 2 - 308 / 2`, derivado de `LARGURA_DA_JANELA` e `criarBotoesDaInterface`
- **Y dos botões** — `652 px`
  - Origem: `720 - 68`, derivado de `ALTURA_DA_JANELA` e `criarBotoesDaInterface`
- **Posicoes X dos botões** — `486, 550, 614, 678, 742 px`
  - Origem: `primeiroX + (52 + 12) * indice`, derivado de `criarBotoesDaInterface`
- **Margem de textura do ícone do botão** — `6 px`
  - Origem: `src/Apresentacao/Renderizacao/UI/BarraDeFerramentasRenderer.hpp::desenharBotaoFerramenta`
- **Área final do ícone com textura** — `40 x 40 px`
  - Origem: `52 - 6 * 2`, derivado de `desenharBotaoFerramenta`

### Loja modal

A Loja deixou de ser um painel pequeno da barra e passou a ser um modal proprio de apresentacao. O calculo fica centralizado em `src/Apresentacao/Interface/Loja/LayoutDaLoja.hpp`.

- **Painel principal na janela atual** — `x=200, y=80, w=880, h=560`
  - Origem: `LayoutDaLoja.hpp::calcularLayoutDaLoja`, com janela `1280 x 720`, largura preferida `880 px`, altura preferida `560 px` e centralizacao.
- **Asset de fundo do painel** — `assets/sprites/loja/loja_fundo.png`
  - Origem: `src/Infraestrutura/Assets/RecursosDaLoja.hpp::caminhoDoFundoDaLoja`
- **Area do titulo `Loja`** — `x=200, y=98, w=880, h=42`
  - Origem: `LayoutDaLoja.hpp::areaDoTitulo`
- **Area de dinheiro do jogador** — `x=232, y=108, w=220, h=28`
  - Origem: `LayoutDaLoja.hpp::areaDoDinheiro`
- **Botao fechar** — `x=1026, y=98, w=36, h=36`
  - Origem: `LayoutDaLoja.hpp::botaoFechar`
- **Abas principais** — `w=142, h=42`, `y=168`, `x=341, 493, 645, 797`
  - Origem: `LayoutDaLoja.hpp::abasPrincipais`; abas: `Sementes`, `Animais`, `Construcoes`, `Decoracoes`.
- **Filtros de sementes** — `w=116, h=34`, `y=226`, `x=396, 520, 644, 768`
  - Origem: `LayoutDaLoja.hpp::filtrosDeSementes`; filtros: `Todos`, `Frutas`, `Vegetais`, `Graos`.
- **Area de conteudo em `Sementes`** — `x=232, y=284, w=816, h=324`
  - Origem: `LayoutDaLoja.hpp::areaDeConteudo`
- **Card de semente** — `w=156, h=198`, primeiro card em `x=232, y=284`
  - Origem: `LayoutDaLoja.hpp::cartoesDeSementes`
- **Botao `Comprar` do primeiro card** — `x=250, y=430, w=120, h=34`
  - Origem: `LayoutDaLoja.hpp::botaoComprar`
- **Contrato de clique da Loja** — clicar fora do painel fecha e consome; clicar em semente fecha, seleciona a semente e consome; enquanto aberta, a Loja bloqueia mundo, pan e zoom.
  - Origem: `src/Apresentacao/Interface/Loja/ControladorDaLoja.hpp` e `src/Apresentacao/Cenas/CenaFazenda.hpp`

### HUD, configurações e cursor

- **Margem do texto da HUD** — `12 px`
  - Origem: `src/Apresentacao/Renderizacao/UI/HudRenderer.hpp::desenharStatusDoJogador`
- **Espacamento vertical do texto da HUD** — `22 px`
  - Origem: `src/Apresentacao/Renderizacao/UI/HudRenderer.hpp::desenharStatusDoJogador`
- **Botao de configurações** — `x=1232, y=12, w=36, h=36`
  - Origem: `src/Apresentacao/Renderizacao/UI/HudRenderer.hpp::desenharBotaoConfiguracoes`, com `1280 - 12 - 36`
- **Recuo do fallback vetorial do botão de configurações** — `10 px`
  - Origem: `src/Apresentacao/Renderizacao/UI/HudRenderer.hpp::desenharBotaoConfiguracoes`
- **Painel de configurações** — `x=400, y=200, w=480, h=320`
  - Origem: `src/Apresentacao/Renderizacao/UI/HudRenderer.hpp::desenharPainelConfiguracoes`, com janela `1280 x 720`
- **Botao de som no painel** — `x=560, y=350, w=160, h=40`
  - Origem: `src/Apresentacao/Renderizacao/UI/HudRenderer.hpp::desenharPainelConfiguracoes`, com painel `x=400, y=200, w=480, h=320`
- **Área do cursor customizado** — `38 x 38 px`
  - Origem: `src/Apresentacao/Renderizacao/Cursores/CursorCustomizado.hpp::desenharCursorCustomizado`

## 8. Observações sobre inconsistências

- **Base de planta/personagem não usa a ponta inferior do losango** — base usada `(64, 32)`, ponta inferior `(64, 64)`
  - Impacto: O sprite toca o chão no centro do losango no código atual, não na ponta inferior. Isso afeta bonecos fantasma e deve ser levado em conta antes de reposicionar arte
- **Fallback vetorial de planta desce abaixo da base usada por sprites** — `2 px` ou `3 px`
  - Impacto: O fallback visual não coincide perfeitamente com a âncora das texturas de planta
- **Zooms com dimensões ímpares deixam metades truncadas** — diferença de `1 px`
  - Impacto: Pode gerar losangos ligeiramente assimétricos em alguns passos de zoom por causa de divisão inteira em `desenharLosango`
- **Contorno do losango usa `x + w` e `y + h`** — pontos `(128,32)` e `(64,64)` em um tile `128 x 64`
  - Impacto: O contorno é desenhado com coordenadas finais inclusivas por `SDL_RenderDrawLine`, diferente da interpretação usual de retângulo preenchido como faixa até `w - 1` e `h - 1`

## 9. Notas para arte

- O tile do chão deve ser pensado como um losango de proporção `2:1`, ou seja, `128 x 64 px` no zoom base (`src/Compartilhado/ConstantesDaIsometria.hpp::LARGURA_DO_CANTEIRO` e `ALTURA_DO_CANTEIRO`).
- Para montar um canvas de referência de um tile, use os pontos `(64,0)`, `(128,32)`, `(64,64)` e `(0,32)` dentro de um retângulo `128 x 64 px` (`src/Apresentacao/Renderizacao/Primitivas/PrimitivasSDL.hpp::desenharLosango`).
- No código atual, o ponto de contato de planta com o tile é o centro do losango, `(64,32)`, não a ponta inferior `(64,64)`. O personagem preserva esse ponto apenas como equivalencia inicial legada; depois anda pelos centros das unidades `1 x 1` de ocupacao (`src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp::calcularDestinoDoSpriteDaPlanta` e `src/Apresentacao/Renderizacao/Mundo/RenderizadorDoPersonagem.hpp::calcularPontoDosPesDoPersonagemNaTela`).
- Para sprites de planta, o motor procura a base no ponto mais baixo dos pixels opacos e centraliza essa base no ponto `(64,32)` do tile (`src/Infraestrutura/Assets/RecursosDaFazenda.hpp::calcularAncoraDaBaseDoSpriteDaPlanta`).
- O personagem atual e desenhado em um destino de `63 x 96 px` no zoom base, com os pés em `(32,96)` dentro desse destino (`src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::destinoLargura`, `destinoAltura`, `pontoDosPesX`, `pontoDosPesY`).
- O padrão atual de frame do personagem é `250 x 250 px`, com 5 frames alinhados horizontalmente e sem espaçamento (`src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::frameLargura`, `frameAltura`, `quantidadeFramesIdle`, `frameEspacamentoX`).
