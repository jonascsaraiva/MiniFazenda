# Grid de ocupacao e mapa da fazenda

O MiniFazenda2 nao deve crescer como uma matriz de terra. O mundo deve ser um mapa isometrico composto por entidades com area logica de ocupacao. Canteiros continuam importantes, mas deixam de ser a celula fundamental universal do jogo.

## Decisao arquitetural

`MapaDaFazenda` e o agregado principal do mundo.

Ele representa entidades presentes na fazenda, sejam elas agricolas, estruturais, decorativas, interativas ou futuras entidades do jogo. O restante do sistema deve consultar o mapa para criar, remover, listar e localizar entidades.

`GridDeOcupacao` continua existindo, mas somente como indice espacial interno do mapa. Ele responde se uma area esta livre, qual identificador ocupa uma celula e qual area esta registrada para um ocupante. Ele nao guarda regra agricola, planta, sprite, textura, renderer, caminho de asset, camera, zoom ou gameplay.

## Canteiro como entidade agricola

Um canteiro e uma entidade do `MapaDaFazenda` com um componente agricola `Canteiro`.

`Canteiro` permanece dono de:

- arar;
- plantar;
- crescimento;
- maturacao;
- morte;
- colheita;
- geracao de restos;
- limpeza de planta morta;
- limpeza de restos.

Essas regras nao pertencem ao mapa nem ao grid de ocupacao.

## Unidade logica

O canteiro visual atual mede `128 x 64 px`.

O contrato de ocupacao e:

```text
1 canteiro visual = 2 x 2 unidades de ocupacao
1 unidade de ocupacao ~= 64 x 32 px
```

A conversao central e:

```text
PosicaoDeCanteiroNoMapa(coluna, linha)
  -> PosicaoNaGradeDeOcupacao(coluna * 2, linha * 2)
  -> AreaNaGradeDeOcupacao(coluna * 2, linha * 2, 2, 2)
```

Essa conversao e apenas compatibilidade para canteiros alinhados ao legado. A origem real de uma entidade no mundo e `PosicaoNaGradeDeOcupacao`, e um canteiro pode ser criado em qualquer origem livre dessa grade, inclusive coordenadas impares:

```text
PosicaoNaGradeDeOcupacao(coluna, linha)
  -> AreaNaGradeDeOcupacao(coluna, linha, 2, 2)
```

Isso permite que outros objetos futuros tenham footprints proprios, como `1 x 1`, `1 x 2`, `4 x 4` ou qualquer area definida pelo design.

## Separacao de responsabilidades

Dominio:

- controla mapa, entidades, posicoes, ocupacao e regras de gameplay;
- mantem `Canteiro` como dono das transicoes agricolas;
- nao conhece SDL nem recursos visuais.

Apresentacao:

- converte coordenadas de tela;
- calcula destino isometrico;
- aplica camera, zoom, destaque e ordem visual;
- desenha canteiros, plantas, personagem e interface.

Infraestrutura:

- carrega texturas, fontes, sons e configuracoes;
- mapeia assets para estados visuais;
- nao decide regra de dominio.

## Personagem

O personagem nao e ocupante fixo do `GridDeOcupacao` nesta etapa.

Sua posicao logica continua representando os pes, e o sprite e desenhado acima desse ponto. O mapa podera ajudar em colisao ou pathfinding no futuro, mas esta migracao nao mistura mapa estatico com navegacao dinamica do personagem.

## Clique

O clique de ferramentas agricolas usa a unidade real de ocupacao:

1. A apresentacao converte tela para `PosicaoNaGradeDeOcupacao`.
2. A ferramenta consulta a entidade em qualquer celula interna do footprint.
3. Se a enxada clicar em celula livre, ela monta uma area `2 x 2` com essa origem e pede ao `MapaDaFazenda` para criar o canteiro.
4. Preview, validacao de area livre, criacao e renderizacao usam a mesma `AreaNaGradeDeOcupacao`.

O movimento do personagem ainda usa `PosicaoNaGrade` como compatibilidade de pes na grade visual antiga. A infraestrutura de tela para ocupacao ja existe; uma proxima etapa pode migrar velocidade, caminho e renderizacao dos pes para a menor malha sem alterar o personagem para ocupante fixo do grid.

## Renderizacao

O renderizador da fazenda consome `MapaDaFazenda` como fonte dos canteiros e calcula o destino visual a partir da `AreaNaGradeDeOcupacao` real de cada entidade. O canteiro continua usando o mesmo sprite `128 x 64`, mas uma origem impar na ocupacao desloca o desenho pela malha menor em vez de aplicar offset visual falso.

A lista unica de renderizaveis por profundidade ainda e uma evolucao futura. Ela deve incluir chao, canteiros, plantas, arvores, casas, animais, decoracoes e personagem, usando a base logica do objeto como referencia de profundidade.

## Remocao do legado

A migracao estrutural removeu:

- `GradeGlobalDeCanteiros`;
- `TileDeTerra`;
- `SincronizadorDoGridDeOcupacao`.

Nao ha mais espelhamento de canteiros da grade antiga para o grid novo. O mapa cria a entidade real e registra sua ocupacao no grid interno no mesmo fluxo.
