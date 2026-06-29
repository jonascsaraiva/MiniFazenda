# Personagem e movimento

## Onde vive a logica

A logica de dominio do personagem vive em `src/Dominio/Personagem/Personagem.hpp`.
Ela controla estado, direcao, posicao dos pes na grade e caminho em segmentos. O
avanco de tempo do jogo chama essa logica por `src/Aplicacao/Servicos/ServicoDeTempo.hpp`.

A renderizacao vive em `src/Apresentacao/Renderizacao/Mundo/RenderizadorDoPersonagem.hpp`.
Ela apenas le o estado do personagem e desenha o sprite.

Os ajustes visuais manuais do sprite ficam em
`src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp`. Nao coloque numeros
magicos de recorte, escala ou offset no renderizador.

## Posicao logica e posicao visual

A posicao logica do personagem representa os pes/base no chao isometrico. Essa
posicao fica em coordenadas de grade e pode ter casas decimais durante o
movimento.

A posicao visual e o retangulo onde o sprite e desenhado. O sprite deve ser
desenhado acima dos pes usando offsets configuraveis. O conceito usado e:

```text
dest.x = telaPesX - pontoDosPesX + ajusteTelaX
dest.y = telaPesY - pontoDosPesY + ajusteTelaY
dest.w = destinoLargura
dest.h = destinoAltura
```

`offsetPeX` e `offsetPeY` permitem ajustar a referencia dos pes antes desse
calculo. `pontoDosPesX` e `pontoDosPesY` indicam onde os pes ficam dentro do
retangulo final do sprite. Os valores de destino e offsets sao a base no zoom
1.0; o renderizador escala esses valores junto com a camera.

## Movimento

O movimento atual e isometrico em segmentos, nao uma linha reta livre em pixels.
Quando o jogador clica em uma area valida do chao, o clique vira uma posicao de
grade/chao. O personagem cria um caminho simples em "L" isometrico ate esse
destino.

As quatro direcoes previstas sao:

- BaixoDireita
- BaixoEsquerda
- CimaDireita
- CimaEsquerda

Cliques em UI, botoes, loja ou painel de configuracoes nao devem mover o
personagem. O clique so inicia movimento quando sobra para a area jogavel e a
posicao convertida esta dentro da grade valida.

Ainda nao ha pathfinding com obstaculos. O caminho atual e propositalmente
simples: primeiro um segmento em uma direcao da grade e depois o outro segmento,
formando um "L" isometrico.

## Como ajustar o sprite

Edite `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp`.

- `frameOrigemX`: desloca o inicio horizontal do primeiro frame na spritesheet.
- `frameOrigemY`: desloca o recorte para cima ou baixo.
- `frameLargura`: largura de cada frame recortado.
- `frameAltura`: altura de cada frame recortado.
- `frameEspacamentoX`: espaco horizontal entre frames.
- `destinoLargura`: largura final desenhada na tela.
- `destinoAltura`: altura final desenhada na tela.
- `pontoDosPesX`: posicao horizontal dos pes dentro do destino final.
- `pontoDosPesY`: posicao vertical dos pes dentro do destino final.
- `ajusteTelaX`: ajuste horizontal final sem alterar a logica dos pes.
- `ajusteTelaY`: ajuste vertical final sem alterar a logica dos pes.

O idle atual usa spritesheet horizontal. A origem do frame e calculada assim:

```text
src.x = frameOrigemX + indiceFrame * (frameLargura + frameEspacamentoX)
src.y = frameOrigemY
src.w = frameLargura
src.h = frameAltura
```

As animacoes futuras ja tem entradas previstas para:

- Idle
- WalkBaixoDireita
- WalkBaixoEsquerda
- WalkCimaDireita
- WalkCimaEsquerda

No momento as caminhadas usam o idle como fallback. Para adicionar sprites
novas, altere a configuracao da animacao correspondente com caminho da textura,
quantidade de frames, origem, largura, altura, espacamento, duracao por frame e
offset visual proprio.

## Cuidados contra regressao

Nao coloque numeros magicos de recorte, escala ou offset no renderizador.
Centralize os ajustes visuais do personagem na configuracao visual.

Nao quebre a ancora pelos pes. A posicao logica deve continuar sendo a base do
personagem no chao, e o sprite deve ser deslocado a partir dela.

Nao troque o movimento isometrico em segmentos por movimento direto pixel-a-pixel
sem mudar explicitamente o contrato da mecanica.
