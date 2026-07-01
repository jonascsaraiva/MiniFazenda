# Tamanhos

Este documento consolida as medidas que governam o posicionamento visual atual do jogo. Os valores abaixo foram extraidos do codigo-fonte e, quando sao derivados, a propria linha da tabela indica a constante ou funcao usada como origem do calculo. O unico valor reservado que ainda nao existe no codigo e o padrao futuro de frame `250 x 250 px`, marcado explicitamente na secao do personagem como requisito de arte ainda pendente de migracao.

## Janela E Background

| Medida | Valor atual | Origem |
| --- | ---: | --- |
| Largura da janela | `1280 px` | `src/Compartilhado/Constantes.hpp::LARGURA_DA_JANELA` |
| Altura da janela | `720 px` | `src/Compartilhado/Constantes.hpp::ALTURA_DA_JANELA` |
| Retangulo usado para desenhar o background | `x=0, y=0, w=1280, h=720` | `src/Apresentacao/Renderizacao/Mundo/DesenhoDoMundo.hpp::desenharFundo`, usando `LARGURA_DA_JANELA` e `ALTURA_DA_JANELA` |
| Centro visual do background | `x=576, y=296` | `src/Compartilhado/Constantes.hpp::CENTRO_VISUAL_BACKGROUND_X` e `CENTRO_VISUAL_BACKGROUND_Y`; os mesmos valores aparecem em `assets/config.ini::centroVisualBackgroundX/Y` |
| Origem inicial de layout antes de centralizar a grade | `x=576, y=296` | `src/Apresentacao/ConfiguracoesDoLayout.hpp::origemGradeHorizontal/origemGradeVertical`, inicializadas com `CENTRO_VISUAL_BACKGROUND_X/Y` |
| Arquivo de background configurado | `background.png` | `src/Apresentacao/ConfiguracoesDoLayout.hpp::arquivoBackgroundPrincipal` e `assets/config.ini::arquivoBackgroundPrincipal` |
| Primeiro fallback de background | `background/fundo_gramado.png` | `src/Infraestrutura/Assets/LocalizadorDeAssets.hpp::candidatosParaBackground` |

Observacao: o codigo sempre desenha o background no retangulo da janela (`1280 x 720 px`), independentemente do tamanho nativo do arquivo de imagem (`src/Apresentacao/Renderizacao/Mundo/DesenhoDoMundo.hpp::desenharFundo`).

## Canteiro Isometrico No Zoom Inicial

| Medida | Valor atual | Origem |
| --- | ---: | --- |
| Largura do canteiro | `128 px` | `src/Compartilhado/Constantes.hpp::LARGURA_DO_CANTEIRO` |
| Altura do canteiro | `64 px` | `src/Compartilhado/Constantes.hpp::ALTURA_DO_CANTEIRO` |
| Proporcao do losango | `2:1` | `128 / 64`, derivado de `LARGURA_DO_CANTEIRO` e `ALTURA_DO_CANTEIRO` |
| Meia largura usada na isometria | `64 px` | `LARGURA_DO_CANTEIRO / 2`, usado em `src/Apresentacao/Isometria/Isometrico.hpp::converterGradeParaTela` |
| Meia altura usada na isometria | `32 px` | `ALTURA_DO_CANTEIRO / 2`, usado em `src/Apresentacao/Isometria/Isometrico.hpp::converterGradeParaTela` |
| Inclinacao da aresta do losango | `32 / 64 = 1 / 2` | `ALTURA_DO_CANTEIRO / 2` dividido por `LARGURA_DO_CANTEIRO / 2` |
| Angulo da aresta com a horizontal | `arctan(32 / 64)` | Derivado de `LARGURA_DO_CANTEIRO` e `ALTURA_DO_CANTEIRO`; a aresta avanca `64 px` na horizontal e `32 px` na vertical |
| Ponto superior do losango dentro do retangulo do tile | `(64, 0)` | `src/Apresentacao/Renderizacao/Primitivas/PrimitivasSDL.hpp::desenharLosango`, com `centroX = x + w / 2` e `topoY = y` |
| Ponto direito do losango dentro do retangulo do tile | `(128, 32)` | `src/Apresentacao/Renderizacao/Primitivas/PrimitivasSDL.hpp::desenharLosango`, com `x + w` e `y + h / 2` |
| Ponto inferior do losango dentro do retangulo do tile | `(64, 64)` | `src/Apresentacao/Renderizacao/Primitivas/PrimitivasSDL.hpp::desenharLosango`, com `x + w / 2` e `y + h` |
| Ponto esquerdo do losango dentro do retangulo do tile | `(0, 32)` | `src/Apresentacao/Renderizacao/Primitivas/PrimitivasSDL.hpp::desenharLosango`, com `x` e `y + h / 2` |
| Ponto usado pelo codigo como base de planta e pes do personagem | `(64, 32)` | `src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp::calcularDestinoDoSpriteDaPlanta` e `src/Apresentacao/Renderizacao/Mundo/RenderizadorDoPersonagem.hpp::calcularPontoDosPesDoPersonagemNaTela` |
| Distancia vertical entre a base usada pelo codigo e a ponta inferior | `32 px` | `(64 - 32)`, derivado de `ALTURA_DO_CANTEIRO` e das funcoes `desenharLosango` e `calcularPontoDosPesDoPersonagemNaTela` |

A conversao de grade para tela usa o topo-esquerda do retangulo de destino do tile. Em zoom inicial, o deslocamento horizontal entre colunas/linhas e de `64 px`, e o deslocamento vertical e de `32 px` (`src/Apresentacao/Isometria/Isometrico.hpp::converterGradeParaTela`, derivado de `LARGURA_DO_CANTEIRO / 2` e `ALTURA_DO_CANTEIRO / 2`).

| Conversao | Formula documentada | Origem |
| --- | --- | --- |
| Grade local para tela X | `x = (coluna - linha) * 64 + deslocamentoHorizontal` | `src/Apresentacao/Isometria/Isometrico.hpp::converterGradeParaTela` |
| Grade local para tela Y | `y = (coluna + linha) * 32 + deslocamentoVertical` | `src/Apresentacao/Isometria/Isometrico.hpp::converterGradeParaTela` |
| Grade global para local | `colunaLocal = colunaGlobal - 127`, `linhaLocal = linhaGlobal - 127` | `src/Apresentacao/Isometria/Isometrico.hpp::converterGradeGlobalParaTela`, usando `COLUNA_DE_ORIGEM_VISUAL_DA_GRADE_GLOBAL` e `LINHA_DE_ORIGEM_VISUAL_DA_GRADE_GLOBAL` |
| Tela para grade | subtrai `64 px` do X ajustado antes de inverter a isometria | `src/Apresentacao/Isometria/Isometrico.hpp::converterTelaParaGrade`, usando `larguraDoCanteiro / 2` |

## Zoom E Dimensoes Escaladas

O tamanho renderizado do canteiro e calculado por `round(tamanhoBase * zoomAtual)`, limitado entre `ZOOM_MINIMO` e `ZOOM_MAXIMO` (`src/Apresentacao/Camera/CameraDoJogo.hpp::calcularDimensoesDoCanteiroRenderizado`).

| Medida | Valor atual | Origem |
| --- | ---: | --- |
| Zoom minimo | `0.5x` | `src/Compartilhado/Constantes.hpp::ZOOM_MINIMO` |
| Zoom inicial | `1.0x` | `src/Compartilhado/Constantes.hpp::ZOOM_INICIAL` |
| Zoom maximo | `2.0x` | `src/Compartilhado/Constantes.hpp::ZOOM_MAXIMO` |
| Passo do zoom | `0.1x` | `src/Compartilhado/Constantes.hpp::PASSO_DO_ZOOM` |
| Canteiro no zoom minimo | `64 x 32 px` | `round(128 * 0.5)` e `round(64 * 0.5)`, derivados de `LARGURA_DO_CANTEIRO`, `ALTURA_DO_CANTEIRO` e `ZOOM_MINIMO` |
| Canteiro no zoom inicial | `128 x 64 px` | `round(128 * 1.0)` e `round(64 * 1.0)`, derivados de `LARGURA_DO_CANTEIRO`, `ALTURA_DO_CANTEIRO` e `ZOOM_INICIAL` |
| Canteiro no zoom maximo | `256 x 128 px` | `round(128 * 2.0)` e `round(64 * 2.0)`, derivados de `LARGURA_DO_CANTEIRO`, `ALTURA_DO_CANTEIRO` e `ZOOM_MAXIMO` |
| Meia largura no zoom minimo | `32 px` | `64 / 2`, usado depois por `converterGradeParaTela` |
| Meia altura no zoom minimo | `16 px` | `32 / 2`, usado depois por `converterGradeParaTela` |
| Meia largura no zoom maximo | `128 px` | `256 / 2`, usado depois por `converterGradeParaTela` |
| Meia altura no zoom maximo | `64 px` | `128 / 2`, usado depois por `converterGradeParaTela` |

### Observacoes De Zoom

| Caso | Valor derivado | Observacao |
| --- | ---: | --- |
| Larguras impares nos passos de zoom | `77, 115, 141, 179, 205, 243 px` | Derivados de `round(128 * zoom)`, para `zoom = 0.6, 0.9, 1.1, 1.4, 1.6, 1.9`; `desenharLosango` usa `w / 2` inteiro, entao o lado direito fica `1 px` mais longo nesses casos |
| Alturas impares nos passos de zoom | `45, 51, 77, 83, 109, 115 px` | Derivados de `round(64 * zoom)`, para `zoom = 0.7, 0.8, 1.2, 1.3, 1.7, 1.8`; `desenharLosango` usa `h / 2` inteiro, entao a metade inferior pode ficar `1 px` mais longa nesses casos |
| Hitbox debug em dimensoes impares | remove `1 px` quando `w` ou `h` e impar | `src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp::calcularHitboxDoCanteiro`, com `(destino.w / 2) * 2` e `(destino.h / 2) * 2`; `DEBUG_HITBOX_TILES` esta `false` em `src/Compartilhado/Constantes.hpp::DEBUG_HITBOX_TILES` |

## Grade Jogavel, Grade Global E Centralizacao

| Medida | Valor atual | Origem |
| --- | ---: | --- |
| Grade global alocada | `256 x 256 tiles` | `src/Compartilhado/Constantes.hpp::QUANTIDADE_DE_COLUNAS_DA_GRADE_GLOBAL_ALOCADA` e `QUANTIDADE_DE_LINHAS_DA_GRADE_GLOBAL_ALOCADA` |
| Total de tiles alocados | `65536 tiles` | `256 * 256`, derivado de `src/Compartilhado/Constantes.hpp::TOTAL_DE_TILES_DA_GRADE_GLOBAL` |
| Centro da grade global | `(128, 128)` | `src/Compartilhado/Constantes.hpp::COLUNA_CENTRAL_DA_GRADE_GLOBAL` e `LINHA_CENTRAL_DA_GRADE_GLOBAL` |
| Tamanho inicial da grade jogavel | `12 x 12 tiles` | `src/Compartilhado/Constantes.hpp::TAMANHO_INICIAL_GRID` |
| Tamanho maximo da grade jogavel | `24 x 24 tiles` | `src/Compartilhado/Constantes.hpp::TAMANHO_MAXIMO_GRID` |
| Incremento de tamanho da grade jogavel | `2 tiles` | `src/Compartilhado/Constantes.hpp::INCREMENTO_TAMANHO_GRID` |
| Tamanhos validos pela normalizacao atual | `12, 14, 16, 18, 20, 22, 24` | Derivados de `TAMANHO_INICIAL_GRID`, `TAMANHO_MAXIMO_GRID`, `INCREMENTO_TAMANHO_GRID` e `src/Dominio/Grade/GradeGlobalDeCanteiros.hpp::normalizarTamanhoDaGradeAtual` |
| Inicio global da grade jogavel inicial | `(122, 122)` | `src/Dominio/Grade/GradeGlobalDeCanteiros.hpp::calcularColunaInicialDaGradeAtual` e `calcularLinhaInicialDaGradeAtual`, com `TAMANHO_INICIAL_GRID` |
| Fim global incluso da grade jogavel inicial | `(133, 133)` | `(122 + 12 - 1)`, derivado de `posicaoEstaDentroDaGradeAtual` |
| Inicio global da grade jogavel maxima | `(116, 116)` | `src/Dominio/Grade/GradeGlobalDeCanteiros.hpp::calcularColunaInicialDaGradeAtual` e `calcularLinhaInicialDaGradeAtual`, com `TAMANHO_MAXIMO_GRID` |
| Fim global incluso da grade jogavel maxima | `(139, 139)` | `(116 + 24 - 1)`, derivado de `posicaoEstaDentroDaGradeAtual` |
| Nucleo inicial ativado | `2 x 2 tiles` | `src/Compartilhado/Constantes.hpp::QUANTIDADE_DE_COLUNAS_DO_NUCLEO_INICIAL` e `QUANTIDADE_DE_LINHAS_DO_NUCLEO_INICIAL`; ativado em `src/Aplicacao/Servicos/InicializadorDaFazenda.hpp::criarGradeGlobalComNucleoInicial` |
| Inicio global do nucleo inicial | `(127, 127)` | `src/Compartilhado/Constantes.hpp::COLUNA_INICIAL_DO_NUCLEO_INICIAL` e `LINHA_INICIAL_DO_NUCLEO_INICIAL` |
| Origem visual global da grade | `(127, 127)` | `src/Compartilhado/Constantes.hpp::COLUNA_DE_ORIGEM_VISUAL_DA_GRADE_GLOBAL` e `LINHA_DE_ORIGEM_VISUAL_DA_GRADE_GLOBAL` |
| Tiles iniciais realmente ativados | `(127,127)`, `(128,127)`, `(127,128)`, `(128,128)` | `src/Aplicacao/Servicos/InicializadorDaFazenda.hpp::criarGradeGlobalComNucleoInicial` |

A grade global alocada (`256 x 256 tiles`) e a grade jogavel atual (`12 x 12` ate `24 x 24 tiles`) nao sao a mesma coisa. A grade global e o espaco total de armazenamento e coordenadas; a grade jogavel e uma janela quadrada centralizada dentro dessa grade, validada por `posicaoEstaDentroDaGradeAtual`.

### Centralizacao No Background

`Camera::aplicarOrigemCentradaDaGrade` recalcula `origemGradeHorizontal/origemGradeVertical` com `Camera::calcularDeslocamentoCentradoPara`. Com as constantes atuais e zoom base, a origem renderizada fica igual para os tamanhos pares usados hoje.

| Caso | Valor derivado | Origem |
| --- | ---: | --- |
| Origem da grade para `12 x 12` no zoom base | `(512, 232)` | `src/Apresentacao/Camera/CameraDoJogo.hpp::calcularDeslocamentoCentradoPara`, com `TAMANHO_INICIAL_GRID`, `CENTRO_VISUAL_BACKGROUND_X/Y`, `LARGURA_DO_CANTEIRO`, `ALTURA_DO_CANTEIRO` |
| Primeiro tile da grade `12 x 12` | `(512, -88)` | `src/Apresentacao/Camera/CameraDoJogo.hpp::calcularDeslocamentoCentradoPara`, com inicio global `(122,122)` |
| Retangulo da grade `12 x 12` no zoom base | `x=-192, y=-88, w=1536, h=768` | `src/Apresentacao/Camera/CameraDoJogo.hpp::calcularRetanguloDaGradeRenderizada`, com `12 * 128` e `12 * 64` |
| Centro do retangulo `12 x 12` | `(576, 296)` | Derivado do retangulo `x=-192, y=-88, w=1536, h=768`, alinhado a `CENTRO_VISUAL_BACKGROUND_X/Y` |
| Origem da grade para `24 x 24` no zoom base | `(512, 232)` | `src/Apresentacao/Camera/CameraDoJogo.hpp::calcularDeslocamentoCentradoPara`, com `TAMANHO_MAXIMO_GRID` |
| Primeiro tile da grade `24 x 24` | `(512, -472)` | `src/Apresentacao/Camera/CameraDoJogo.hpp::calcularDeslocamentoCentradoPara`, com inicio global `(116,116)` |
| Retangulo da grade `24 x 24` no zoom base | `x=-960, y=-472, w=3072, h=1536` | `src/Apresentacao/Camera/CameraDoJogo.hpp::calcularRetanguloDaGradeRenderizada`, com `24 * 128` e `24 * 64` |
| Centro do retangulo `24 x 24` | `(576, 296)` | Derivado do retangulo `x=-960, y=-472, w=3072, h=1536`, alinhado a `CENTRO_VISUAL_BACKGROUND_X/Y` |
| Margem horizontal minima visivel no pan | `320 px` | `1280 * 0.25`, derivado de `LARGURA_DA_JANELA` e `FRACAO_MINIMA_VISIVEL_AO_PAN` em `src/Compartilhado/Constantes.hpp` |
| Margem vertical minima visivel no pan | `180 px` | `720 * 0.25`, derivado de `ALTURA_DA_JANELA` e `FRACAO_MINIMA_VISIVEL_AO_PAN` em `src/Compartilhado/Constantes.hpp` |

## Plantas Dentro Do Tile

### Sprites De Planta Com Textura

| Medida | Valor atual | Origem |
| --- | ---: | --- |
| Ponto de plantio no tile no zoom base | `(64, 32)` relativo ao topo-esquerda do tile | `src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp::calcularDestinoDoSpriteDaPlanta`, com `destinoDoCanteiro.w / 2` e `destinoDoCanteiro.h / 2` |
| Escala da textura da planta | `destinoDoCanteiro.w / sprite.largura` | `src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp::calcularDestinoDoSpriteDaPlanta` |
| Largura final da planta | `round(sprite.largura * escala)` | `src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp::calcularDestinoDoSpriteDaPlanta` |
| Altura final da planta | `round(sprite.altura * escala)` | `src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp::calcularDestinoDoSpriteDaPlanta` |
| Ancora X escalada | `round(sprite.ancoraDaBase.x * escala)` | `src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp::calcularDestinoDoSpriteDaPlanta` |
| Ancora Y escalada | `round(sprite.ancoraDaBase.y * escala)` | `src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp::calcularDestinoDoSpriteDaPlanta` |
| Destino final da textura | `x = pontoDePlantioX - ancoraX`, `y = pontoDePlantioY - ancoraY` | `src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp::calcularDestinoDoSpriteDaPlanta` |
| Ancora calculada quando ha pixels opacos | `x = (minimoX + maximoX) / 2`, `y = maximoY` | `src/Infraestrutura/Assets/RecursosDaFazenda.hpp::calcularAncoraDaBaseDoSpriteDaPlanta` |
| Ancora calculada quando nao ha pixels opacos | `x = sprite.largura / 2`, `y = sprite.altura` | `src/Infraestrutura/Assets/RecursosDaFazenda.hpp::calcularAncoraDaBaseDoSpriteDaPlanta` |
| Fallback quando o sprite nao possui ancora valida | usa o proprio retangulo do tile, `128 x 64 px` no zoom base | `src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp::calcularDestinoDoSpriteDaPlanta` |

Conclusao para arte: a textura da planta deve ter a base visual no ponto inferior mais baixo dos pixels opacos, pois o codigo calcula a ancora em `y = maximoY` e alinha esse ponto ao centro do tile (`64, 32` no zoom base).

### Fallback Vetorial Da Planta

Estes valores so aparecem quando a textura da planta nao existe ou nao foi carregada. Eles revelam a expectativa atual do motor para a base visual da planta.

| Estado visual | Retangulo relativo ao tile no zoom base | Origem |
| --- | ---: | --- |
| Semente plantada | `x=61, y=29, w=6, h=6` | `src/Apresentacao/Renderizacao/Mundo/DesenhoDoMundo.hpp::desenharFallbackDaPlanta`, com `w / 2 - 3`, `h / 2 - 3`, `6`, `6` |
| Planta crescendo, jovem ou madura, haste vertical | `x=58, y=14, w=12, h=20` | `src/Apresentacao/Renderizacao/Mundo/DesenhoDoMundo.hpp::desenharFallbackDaPlanta`, com `w / 2 - 6`, `h / 2 - 18`, `12`, `20` |
| Planta crescendo, jovem ou madura, volume horizontal | `x=46, y=23, w=36, h=8` | `src/Apresentacao/Renderizacao/Mundo/DesenhoDoMundo.hpp::desenharFallbackDaPlanta`, com `w / 2 - 18`, `h / 2 - 9`, `36`, `8` |
| Planta morta, haste vertical | `x=61, y=16, w=6, h=18` | `src/Apresentacao/Renderizacao/Mundo/DesenhoDoMundo.hpp::desenharFallbackDaPlanta`, com `w / 2 - 3`, `h / 2 - 16`, `6`, `18` |
| Planta morta, volume horizontal | `x=51, y=24, w=26, h=5` | `src/Apresentacao/Renderizacao/Mundo/DesenhoDoMundo.hpp::desenharFallbackDaPlanta`, com `w / 2 - 13`, `h / 2 - 8`, `26`, `5` |

Observacao: o ponto de plantio usado para sprites com textura e `(64, 32)`, mas o fallback vetorial pode ultrapassar esse ponto em `2 px` ou `3 px`, dependendo do estado visual (`src/Apresentacao/Renderizacao/Mundo/DesenhoDoMundo.hpp::desenharFallbackDaPlanta`). Isso nao foi corrigido nesta tarefa.

## Personagem Jogador

O codigo de renderizacao do personagem foi localizado em `src/Apresentacao/Renderizacao/Mundo/RenderizadorDoPersonagem.hpp`. A configuracao visual fica em `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp`.

### Sprite E Animacao Atual

| Medida | Valor atual | Origem |
| --- | ---: | --- |
| Textura usada atualmente | `sprites/personagem/bonequinho.png` | `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::caminhoTextura` |
| Quantidade de animacoes configuradas | `5` | `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::QUANTIDADE_DE_ANIMACOES` |
| Animacoes de caminhada | usam o mesmo idle | `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::configuracaoWalkBaixoDireita`, `configuracaoWalkBaixoEsquerda`, `configuracaoWalkCimaDireita`, `configuracaoWalkCimaEsquerda` |
| Frames do idle | `8` | `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::quantidadeFramesIdle` |
| Origem X do primeiro frame | `0 px` | `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::frameOrigemX` |
| Origem Y do primeiro frame | `145 px` | `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::frameOrigemY` |
| Largura do frame atual | `271 px` | `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::frameLargura` |
| Altura do frame atual | `416 px` | `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::frameAltura` |
| Espacamento horizontal entre frames | `0 px` | `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::frameEspacamentoX` |
| Espacamento vertical entre frames | `0 px` | `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::frameEspacamentoY` |
| Duracao por frame | `0.12 s` | `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::duracaoPorFrame` |
| Tolerancia usada na troca de frame | `0.00001 s` | `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::calcularIndiceFrame` |
| Largura minima exigida da spritesheet atual | `2168 px` | `0 + (8 - 1) * (271 + 0) + 271`, derivado de `validarDimensoesDaTexturaDoPersonagem` |
| Altura minima exigida da spritesheet atual | `561 px` | `145 + 416`, derivado de `validarDimensoesDaTexturaDoPersonagem` |

A animacao idle atual e uma animacao por spritesheet. A classe `src/Dominio/Animacao/AnimacaoIdle.hpp::AnimacaoIdle` apenas acumula tempo, e o indice do frame vem de `ConfigVisualDoPersonagem::calcularIndiceFrame`. Nao ha deslocamento procedural de respiracao ou piscar no renderizador; esses efeitos, se existirem visualmente, precisam estar desenhados nos `8` frames do spritesheet atual.

### Destino E Ancora Dos Pes

| Medida | Valor atual | Origem |
| --- | ---: | --- |
| Largura de destino no zoom base | `63 px` | `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::destinoLargura` |
| Altura de destino no zoom base | `96 px` | `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::destinoAltura` |
| Ponto dos pes dentro do destino no zoom base | `(28, 95)` | `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::pontoDosPesX` e `pontoDosPesY` |
| Offset dos pes | `(0, 0)` | `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::offsetPeX` e `offsetPeY` |
| Ajuste final de tela | `(0, 0)` | `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::ajusteTelaX` e `ajusteTelaY` |
| Destino no zoom minimo | `32 x 48 px` | `round(63 * 0.5)` e `round(96 * 0.5)`, derivados de `destinoLargura`, `destinoAltura` e `ZOOM_MINIMO` |
| Ponto dos pes no destino no zoom minimo | `(14, 48)` | `round(28 * 0.5)` e `round(95 * 0.5)`, derivados de `pontoDosPesX/Y` e `ZOOM_MINIMO` |
| Destino no zoom maximo | `126 x 192 px` | `round(63 * 2.0)` e `round(96 * 2.0)`, derivados de `destinoLargura`, `destinoAltura` e `ZOOM_MAXIMO` |
| Ponto dos pes no destino no zoom maximo | `(56, 190)` | `round(28 * 2.0)` e `round(95 * 2.0)`, derivados de `pontoDosPesX/Y` e `ZOOM_MAXIMO` |
| Ponto dos pes quando parado exatamente em um tile no zoom base | `(64, 32)` relativo ao topo-esquerda do tile | `src/Apresentacao/Renderizacao/Mundo/RenderizadorDoPersonagem.hpp::calcularPontoDosPesDoPersonagemNaTela` |
| Retangulo do personagem relativo ao tile no zoom base | `x=36, y=-63, w=63, h=96` | `(64 - 28, 32 - 95, 63, 96)`, derivado de `calcularPontoDosPesDoPersonagemNaTela` e `calcularRetanguloDeDestino` |
| Posicao inicial dos pes na grade global | `(128, 128)` | `src/Dominio/Personagem/Personagem.hpp::posicaoDosPesNaGrade_`, usando `COLUNA_CENTRAL_DA_GRADE_GLOBAL` e `LINHA_CENTRAL_DA_GRADE_GLOBAL` |
| Posicao inicial dos pes na tela, sem pan e no zoom base | `(576, 328)` | Derivado de `posicaoDosPesNaGrade_`, origem `(512, 232)`, `LARGURA_DO_CANTEIRO` e `ALTURA_DO_CANTEIRO` |
| Retangulo inicial do personagem na tela, sem pan e no zoom base | `x=548, y=233, w=63, h=96` | `(576 - 28, 328 - 95, 63, 96)`, derivado de `calcularRetanguloDeDestino` |

### Reserva Para O Novo Padrao De Arte

| Medida | Valor | Origem |
| --- | ---: | --- |
| Padrao futuro de frame | `250 x 250 px` | Requisito desta documentacao; ainda nao existe constante ou identificador no codigo atual |
| Estado da migracao | pendente | O codigo ainda usa `frameLargura=271`, `frameAltura=416`, `destinoLargura=63`, `destinoAltura=96` em `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp` |

## Interface E HUD

Estas medidas nao alteram o posicionamento do mundo, mas fazem parte da composicao visual desenhada por cima dele.

### Barra De Ferramentas

| Medida | Valor atual | Origem |
| --- | ---: | --- |
| Quantidade de botoes inferiores | `5` | `src/Apresentacao/Interface/BarraDeFerramentas/BarraDeFerramentas.hpp::criarBotoesDaInterface` |
| Tamanho do botao | `52 x 52 px` | `src/Compartilhado/Constantes.hpp::TAMANHO_DO_BOTAO_DA_INTERFACE` |
| Espacamento entre botoes | `12 px` | `src/Compartilhado/Constantes.hpp::ESPACAMENTO_DOS_BOTOES` |
| Largura total da barra | `308 px` | `52 * 5 + 12 * (5 - 1)`, derivado de `criarBotoesDaInterface` |
| X do primeiro botao | `486 px` | `1280 / 2 - 308 / 2`, derivado de `LARGURA_DA_JANELA` e `criarBotoesDaInterface` |
| Y dos botoes | `652 px` | `720 - 68`, derivado de `ALTURA_DA_JANELA` e `criarBotoesDaInterface` |
| Posicoes X dos botoes | `486, 550, 614, 678, 742 px` | `primeiroX + (52 + 12) * indice`, derivado de `criarBotoesDaInterface` |
| Margem de textura do icone do botao | `6 px` | `src/Apresentacao/Renderizacao/UI/BarraDeFerramentasRenderer.hpp::desenharBotaoFerramenta` |
| Area final do icone com textura | `40 x 40 px` | `52 - 6 * 2`, derivado de `desenharBotaoFerramenta` |

### Painel Da Loja

| Medida | Valor atual | Origem |
| --- | ---: | --- |
| Tamanho de cada opcao de semente | `48 x 48 px` | `src/Apresentacao/Interface/BarraDeFerramentas/BarraDeFerramentas.hpp::criarPainelDaLoja` |
| Preenchimento interno do painel | `8 px` | `src/Apresentacao/Interface/BarraDeFerramentas/BarraDeFerramentas.hpp::criarPainelDaLoja` |
| Espacamento entre opcoes | `8 px` | `src/Apresentacao/Interface/BarraDeFerramentas/BarraDeFerramentas.hpp::criarPainelDaLoja` |
| Margem de icone da opcao de semente | `5 px` | `src/Apresentacao/Renderizacao/UI/BarraDeFerramentasRenderer.hpp::desenharPainelDaLoja` |
| Area final do icone de semente | `38 x 38 px` | `48 - 5 * 2`, derivado de `desenharPainelDaLoja` |

### HUD, Configuracoes E Cursor

| Medida | Valor atual | Origem |
| --- | ---: | --- |
| Margem do texto da HUD | `12 px` | `src/Apresentacao/Renderizacao/UI/HudRenderer.hpp::desenharStatusDoJogador` |
| Espacamento vertical do texto da HUD | `22 px` | `src/Apresentacao/Renderizacao/UI/HudRenderer.hpp::desenharStatusDoJogador` |
| Botao de configuracoes | `x=1232, y=12, w=36, h=36` | `src/Apresentacao/Renderizacao/UI/HudRenderer.hpp::desenharBotaoConfiguracoes`, com `1280 - 12 - 36` |
| Recuo do fallback vetorial do botao de configuracoes | `10 px` | `src/Apresentacao/Renderizacao/UI/HudRenderer.hpp::desenharBotaoConfiguracoes` |
| Painel de configuracoes | `x=400, y=200, w=480, h=320` | `src/Apresentacao/Renderizacao/UI/HudRenderer.hpp::desenharPainelConfiguracoes`, com janela `1280 x 720` |
| Botao de som no painel | `x=560, y=350, w=160, h=40` | `src/Apresentacao/Renderizacao/UI/HudRenderer.hpp::desenharPainelConfiguracoes`, com painel `x=400, y=200, w=480, h=320` |
| Area do cursor customizado | `38 x 38 px` | `src/Apresentacao/Renderizacao/Cursores/CursorCustomizado.hpp::desenharCursorCustomizado` |

## Observacoes Sobre Inconsistencias

| Ponto observado | Valor envolvido | Impacto |
| --- | ---: | --- |
| Base de planta/personagem nao usa a ponta inferior do losango | base usada `(64, 32)`, ponta inferior `(64, 64)` | O sprite toca o chao no centro do losango no codigo atual, nao na ponta inferior. Isso afeta bonecos fantasma e deve ser levado em conta antes de reposicionar arte |
| Fallback vetorial de planta desce abaixo da base usada por sprites | `2 px` ou `3 px` | O fallback visual nao coincide perfeitamente com a ancora das texturas de planta |
| Zooms com dimensoes impares deixam metades truncadas | diferenca de `1 px` | Pode gerar losangos ligeiramente assimetricos em alguns passos de zoom por causa de divisao inteira em `desenharLosango` |
| Contorno do losango usa `x + w` e `y + h` | pontos `(128,32)` e `(64,64)` em um tile `128 x 64` | O contorno e desenhado com coordenadas finais inclusivas por `SDL_RenderDrawLine`, diferente da interpretacao usual de retangulo preenchido como faixa ate `w - 1` e `h - 1` |

## Notas Para Arte

- O tile do chao deve ser pensado como um losango de proporcao `2:1`, ou seja, `128 x 64 px` no zoom base (`src/Compartilhado/Constantes.hpp::LARGURA_DO_CANTEIRO` e `ALTURA_DO_CANTEIRO`).
- Para montar um canvas de referencia de um tile, use os pontos `(64,0)`, `(128,32)`, `(64,64)` e `(0,32)` dentro de um retangulo `128 x 64 px` (`src/Apresentacao/Renderizacao/Primitivas/PrimitivasSDL.hpp::desenharLosango`).
- No codigo atual, o ponto de contato de planta e personagem com o tile e o centro do losango, `(64,32)`, nao a ponta inferior `(64,64)` (`src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp::calcularDestinoDoSpriteDaPlanta` e `src/Apresentacao/Renderizacao/Mundo/RenderizadorDoPersonagem.hpp::calcularPontoDosPesDoPersonagemNaTela`).
- Para sprites de planta, o motor procura a base no ponto mais baixo dos pixels opacos e centraliza essa base no ponto `(64,32)` do tile (`src/Infraestrutura/Assets/RecursosDaFazenda.hpp::calcularAncoraDaBaseDoSpriteDaPlanta`).
- O personagem atual e desenhado em um destino de `63 x 96 px` no zoom base, com os pes em `(28,95)` dentro desse destino (`src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::destinoLargura`, `destinoAltura`, `pontoDosPesX`, `pontoDosPesY`).
- O padrao futuro de frame `250 x 250 px` ainda nao existe no codigo; enquanto a migracao nao acontecer, os valores operacionais continuam sendo `271 x 416 px` por frame de origem e `63 x 96 px` no destino renderizado (`src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp::frameLargura`, `frameAltura`, `destinoLargura`, `destinoAltura`).
