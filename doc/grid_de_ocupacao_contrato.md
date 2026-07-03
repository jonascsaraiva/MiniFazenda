# Grid de ocupacao do mundo

Este documento registra o contrato tecnico do mapa isometrico do MiniFazenda2. Ele complementa `doc/grid_de_ocupacao.md`, usado como referencia conceitual da migracao.

## Estado atual implementado

O agregado principal do mundo agora e `src/Dominio/Mapa/MapaDaFazenda.hpp`.

- `MapaDaFazenda` e a fonte da verdade para entidades presentes no mapa.
- `MapaDaFazenda` cria, remove, lista e consulta entidades do mapa.
- Canteiros sao entidades agricolas do mapa e carregam o componente de dominio `Canteiro`.
- Canteiros ocupam `2 x 2` unidades de ocupacao e podem iniciar em qualquer `PosicaoNaGradeDeOcupacao` livre, inclusive coordenadas impares.
- `GridDeOcupacao` e apenas o indice espacial interno do mapa.
- `GradeGlobalDeCanteiros`, `TileDeTerra` e `SincronizadorDoGridDeOcupacao` foram removidos.

A lavoura continua usando `Canteiro` para estados, arar, plantar, crescimento, maturacao, morte, colheita, restos e limpeza. Essas regras nao foram movidas para o mapa nem para o grid.

## Separacao de responsabilidades

### Mapa da fazenda

`MapaDaFazenda` controla dominio de mapa:

- identificador das entidades;
- tipo de entidade;
- area logica ocupada;
- criacao e remocao;
- consulta por posicao de canteiro ou unidade de ocupacao;
- lista de entidades;
- lista de canteiros que precisam avancar crescimento;
- acesso controlado ao componente agricola `Canteiro`.

Ele nao conhece SDL, textura, renderer, sprite, caminho de asset, camera, zoom ou offset visual.

### Grid de ocupacao

`GridDeOcupacao` responde somente perguntas espaciais:

- se uma area esta dentro do grid;
- se uma area esta livre;
- qual identificador ocupa uma celula;
- qual registro ocupa uma regiao;
- qual profundidade logica a base de uma area possui.

Ele armazena registros de ocupacao por identificador e area. A entidade real vive no `MapaDaFazenda`.

### Canteiro agricola

`Canteiro` permanece dono das transicoes agricolas:

```text
TerraVazia -> arar() -> TerraArada
TerraArada -> plantar(planta) -> SementePlantada -> PlantaCrescendo -> PlantaJovem -> PlantaMadura
PlantaMadura -> colher() -> Restos
PlantaMorta -> limparPlantaMorta() -> Restos
Restos -> limparRestos() -> TerraVazia
```

Codigo externo chama intencoes no canteiro; nao altera estado interno diretamente.

### Renderizacao

A apresentacao le entidades do `MapaDaFazenda` e decide como desenhar:

- textura;
- escala;
- ancora;
- offset;
- zoom;
- camera;
- retangulo de destino;
- destaque visual;
- ordem de desenho existente.

Sprites podem ser maiores que o footprint logico. A ocupacao nao cresce automaticamente por causa do tamanho visual do sprite.

## Unidade logica

O canteiro visual atual mede `128 x 64 px` no zoom base e segue proporcao isometrica `2:1`.

Contrato:

```text
1 canteiro visual = 2 x 2 unidades de ocupacao
1 unidade de ocupacao ~= 64 x 32 px no zoom base
```

Constantes:

- `UNIDADES_DE_OCUPACAO_POR_CANTEIRO = 2`
- `LARGURA_DA_UNIDADE_DE_OCUPACAO = 64`
- `ALTURA_DA_UNIDADE_DE_OCUPACAO = 32`

Conversao central:

```text
PosicaoDeCanteiroNoMapa(coluna, linha)
  -> PosicaoNaGradeDeOcupacao(coluna * 2, linha * 2)
  -> AreaNaGradeDeOcupacao(coluna * 2, linha * 2, 2, 2)
```

Essa conversao fica em `Dominio/Ocupacao/GridDeOcupacao.hpp`, mas e apenas um helper de compatibilidade para posicoes alinhadas ao legado. O contrato principal para criacao e posicionamento livre e:

```text
PosicaoNaGradeDeOcupacao(coluna, linha)
  -> AreaNaGradeDeOcupacao(coluna, linha, largura, altura)
```

Para canteiros, `largura = 2` e `altura = 2`. Nao ha obrigacao arquitetural de a origem do canteiro ser par.

## Tipos de posicao

`PosicaoDeCanteiroNoMapa` representa uma posicao agricola de canteiro.

`PosicaoNaGradeDeOcupacao` representa a menor unidade logica de ocupacao.

`PosicaoNaGrade` ainda aparece em isometria e compatibilidades de canteiro, mas nao e mais a posicao principal dos pes do personagem.

`PosicaoNaGradeDeOcupacao` e a menor unidade real de posicionamento do mundo. Clique de ferramenta, preview, validacao de area livre, criacao, renderizacao de canteiro e movimento do personagem devem usar essa unidade.

`PosicaoDecimalNaGradeDeOcupacao` representa a posicao dos pes do personagem durante o avanco entre dois destinos inteiros de ocupacao.

`PosicaoDeCanteiroNoMapa` pode continuar existindo para compatibilidade com canteiros alinhados, inicializacao e chamadas antigas. Ela nao pode ser usada como trava arquitetural que force novas entidades a nascerem em coordenadas pares de ocupacao.

## Clique e interacao

O clique no mundo separa movimento e ferramenta:

1. Para ferramentas, a apresentacao converte a tela para `PosicaoNaGradeDeOcupacao`.
2. A ferramenta consulta `MapaDaFazenda::entidadeEm()` pela celula real sob o mouse.
3. Se nao houver entidade e a enxada estiver selecionada, o mapa tenta criar um canteiro `2 x 2` com origem nessa celula.
4. Para movimento do personagem, a cena converte a tela para `PosicaoNaGradeDeOcupacao`, valida a area jogavel de ocupacao e cria o caminho simples em segmentos no dominio do personagem.

A ordem de conversao de tela para mundo e: coordenada do mouse na janela, compensacao de camera/pan, compensacao da origem visual da grade, uso das dimensoes renderizadas arredondadas pelo zoom, inversao isometrica e resolucao final para `PosicaoNaGradeDeOcupacao`. Preview, ferramenta, entidades e movimento do personagem devem usar essa mesma base matematica.

O zoom no ponto do cursor preserva a `PosicaoDecimalNaGradeDeOcupacao` local sob o mouse antes de recomputar o offset da camera. Isso evita que o preview ou o destino do clique escorreguem para outra unidade quando o zoom muda em valores com dimensoes impares ou arredondadas.

O personagem nao e registrado como ocupante fixo do `GridDeOcupacao`. A migracao atual muda escala logica, clique, caminho, velocidade e renderizacao dos pes; colisao dinamica e pathfinding continuam fora deste contrato.

## Renderizacao atual

`RenderizadorDaFazenda` consome `jogo.mapa().entidades()`, filtra entidades agricolas de tipo canteiro e calcula o destino visual pela `AreaNaGradeDeOcupacao` real da entidade.

O sprite do canteiro continua sendo o mesmo tile visual. Se a area comecar em origem impar, preview, area ocupada, clique interno, canteiro e planta se deslocam juntos na malha menor.

A lista unica de renderizaveis por profundidade ainda nao foi implementada. O personagem continua desenhado no fluxo existente e nao vira ocupante fixo do grid nesta etapa.

## Profundidade

Em isometria, a sobreposicao deve ser definida pela base do objeto, nao pelo topo do sprite.

Para uma area ocupada:

```text
profundidade = (coluna + largura - 1) + (linha + altura - 1)
```

Essa funcao permanece no dominio de ocupacao porque e derivada do footprint logico, sem detalhes visuais.

## Dependencias removidas

Removidos nesta migracao:

- `src/Dominio/Grade/GradeGlobalDeCanteiros.hpp`
- `src/Dominio/Grade/TileDeTerra.hpp`
- `src/Aplicacao/Servicos/SincronizadorDoGridDeOcupacao.hpp`

Fluxos migrados para `MapaDaFazenda`:

- estado do jogo;
- inicializacao do nucleo inicial;
- ferramentas;
- avanco de tempo;
- renderizacao dos canteiros;
- preview de criacao de terra;
- validacoes de area jogavel usadas por camera/cena.

## Proximas evolucoes recomendadas

1. Criar interacao por unidade de ocupacao para entidades nao agricolas.
2. Adicionar entidades estruturais/decorativas diretamente ao `MapaDaFazenda`.
3. Criar DTO de item renderizavel na apresentacao, sem SDL no dominio.
4. Ordenar canteiros, plantas, personagem e objetos por profundidade de base.
5. Definir regras para camadas de ocupacao caso objetos possam coexistir com chao agricola.
