# Personagem e Movimento

Este documento organiza a lógica de personagem, movimento e renderização do sprite. O objetivo é deixar claro onde cada responsabilidade fica no código, como a posição lógica se relaciona com a posição visual e quais cuidados devem ser mantidos para evitar regressões.

## Sumário

1. [Responsabilidades no código](#1-responsabilidades-no-código)
2. [Posição lógica e posição visual](#2-posição-lógica-e-posição-visual)
3. [Movimento do personagem](#3-movimento-do-personagem)
4. [Ajustes do sprite](#4-ajustes-do-sprite)
5. [Animações previstas](#5-animações-previstas)
6. [Cuidados contra regressão](#6-cuidados-contra-regressão)

---

## 1. Responsabilidades no código

A implementação do personagem está dividida entre domínio, serviço de tempo, renderização e configuração visual. Cada parte tem uma função específica e não deve assumir responsabilidades das outras.

### 1.1. Lógica de domínio

A lógica de domínio do personagem fica em:

`src/Dominio/Personagem/Personagem.hpp`

Essa camada controla:

- Estado do personagem.
- Direção atual.
- Posição dos pés na grade de ocupação.
- Caminho em segmentos.

Essa camada não controla animação, spritesheet, índice de frame, tempo visual, piscadas, textura ou qualquer escolha de desenho.

O avanço de tempo do jogo chama essa lógica por meio de:

`src/Aplicacao/Servicos/ServicoDeTempo.hpp`

Esse serviço avança movimento e crescimento. Ele não avança animação visual do personagem.

### 1.2. Animação visual de apresentação

A animação visual do personagem fica em:

`src/Apresentacao/Animacao/AnimadorDoPersonagem.hpp`

`src/Apresentacao/Animacao/AnimacaoIdleDoPersonagem.hpp`

Essa camada lê apenas `estadoAtual()` e `direcaoAtual()` do domínio. A partir disso, escolhe a animação visual, mantém índice de frame, tempo acumulado, sequência de idle e sorteio de piscadas.

### 1.3. Renderização

A renderização do personagem fica em:

`src/Apresentacao/Renderizacao/Mundo/RenderizadorDoPersonagem.hpp`

Essa camada deve ler o estado lógico do personagem, o estado visual produzido pela apresentação e desenhar o sprite correspondente.

O renderizador não deve concentrar regras de recorte, escala, offsets manuais ou valores fixos de posicionamento visual.

### 1.4. Configuração visual do sprite

Os ajustes visuais manuais do sprite ficam em:

`src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp`

Use esse arquivo para centralizar números de recorte, escala, destino, ponto dos pés e ajustes finais de tela.

> Regra prática: não coloque números mágicos de recorte, escala ou offset dentro do renderizador.

---

## 2. Posição lógica e posição visual

A posição do personagem possui dois conceitos diferentes: posição lógica e posição visual.

### 2.1. Posição lógica

A posição lógica representa os pés, ou a base do personagem, no chão isométrico.

Essa posição fica em `PosicaoNaGradeDeOcupacao` quando está parada ou no destino, e em `PosicaoDecimalNaGradeDeOcupacao` durante o movimento.

Isso significa que os pés andam na menor malha lógica `1 x 1` de ocupação. O personagem não é registrado como ocupante fixo do `GridDeOcupacao` nesta etapa.

Em outras palavras, a lógica não controla o retângulo inteiro do sprite. Ela controla o ponto de contato do personagem com o chão.

### 2.2. Posição visual

A posição visual é o retângulo onde o sprite é desenhado na tela.

O sprite deve ser desenhado acima dos pés, usando offsets configuráveis. O conceito usado é:

```text
dest.x = telaPesX - pontoDosPesX + ajusteTelaX
dest.y = telaPesY - pontoDosPesY + ajusteTelaY
dest.w = destinoLargura
dest.h = destinoAltura
```

### 2.3. Referência dos pés

Os campos abaixo controlam como os pés se relacionam com o retângulo final do sprite:

- `offsetPeX`: permite ajustar horizontalmente a referência dos pés antes do cálculo final.
- `offsetPeY`: permite ajustar verticalmente a referência dos pés antes do cálculo final.
- `pontoDosPesX`: indica a posição horizontal dos pés dentro do destino final do sprite.
- `pontoDosPesY`: indica a posição vertical dos pés dentro do destino final do sprite.
- `ajusteTelaX`: aplica um ajuste horizontal final sem alterar a lógica dos pés.
- `ajusteTelaY`: aplica um ajuste vertical final sem alterar a lógica dos pés.

Os valores de destino, ponto dos pés e offsets são definidos para o zoom `1.0`. O renderizador escala esses valores junto com a câmera.

---

## 3. Movimento do personagem

O movimento atual é isométrico em segmentos. Ele não é uma linha reta livre em pixels.

Quando o jogador clica em uma área válida do chão, o clique é convertido para `PosicaoNaGradeDeOcupacao`. A partir desse destino, o personagem cria um caminho simples em formato de “L” isométrico.

### 3.1. Direções previstas

As quatro direções previstas são:

- `BaixoDireita`
- `BaixoEsquerda`
- `CimaDireita`
- `CimaEsquerda`

### 3.2. Quando o clique deve mover o personagem

O clique só deve iniciar movimento quando todas as condições abaixo forem atendidas:

- O clique não aconteceu sobre a interface.
- O clique não aconteceu sobre botões.
- A Loja modal não está aberta.
- O clique não aconteceu sobre o painel de configurações.
- O clique sobrou para a área jogável.
- A posição convertida está dentro da área jogável de ocupação.

Cliques em UI, botões, Loja aberta ou painel de configurações não devem mover o personagem. Com a Loja aberta, clicar fora do painel fecha a Loja e consome o clique.

### 3.3. Caminho em “L” isométrico

Ainda não há pathfinding com obstáculos.

O caminho atual é propositalmente simples: primeiro o personagem percorre um segmento em uma direção da grade de ocupação e, depois, percorre o outro segmento. Esse comportamento forma um “L” isométrico até o destino.

A velocidade lógica atual é expressa em unidades de ocupação por segundo e é derivada da velocidade antiga em células grandes para preservar a sensação visual.

---

## 4. Ajustes do sprite

Todos os ajustes do sprite devem ser feitos em:

`src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp`

### 4.1. Campos de recorte da spritesheet

- `frameOrigemX`: desloca o início horizontal do primeiro frame na spritesheet.
- `frameOrigemY`: desloca o recorte para cima ou para baixo.
- `frameLargura`: define a largura de cada frame recortado.
- `frameAltura`: define a altura de cada frame recortado.
- `frameEspacamentoX`: define o espaço horizontal entre frames.

### 4.2. Campos de destino na tela

- `destinoLargura`: define a largura final desenhada na tela.
- `destinoAltura`: define a altura final desenhada na tela.

### 4.3. Campos de referência dos pés

- `pontoDosPesX`: define a posição horizontal dos pés dentro do destino final.
- `pontoDosPesY`: define a posição vertical dos pés dentro do destino final.

No padrão atual do personagem, o frame de origem possui `250 x 250 px`, mas esse tamanho não é o tamanho desenhado na tela. Os pés ficam no centro da borda inferior do canvas de origem; depois da escala para o destino, essa convenção equivale ao centro inferior do destino arredondado para inteiro, `pontoDosPesX = (destinoLargura + 1) / 2` e `pontoDosPesY = destinoAltura`.

### 4.4. Ajustes finais de tela

- `ajusteTelaX`: aplica ajuste horizontal final sem alterar a lógica dos pés.
- `ajusteTelaY`: aplica ajuste vertical final sem alterar a lógica dos pés.

### 4.5. Cálculo do recorte do frame

As animações do personagem usam spritesheets horizontais. A apresentação calcula o índice do frame atual e o renderizador usa esse índice junto da configuração visual para calcular a origem do recorte:

```text
src.x = frameOrigemX + indiceFrame * (frameLargura + frameEspacamentoX)
src.y = frameOrigemY
src.w = frameLargura
src.h = frameAltura
```

No idle, o índice do frame vem de `src/Apresentacao/Animacao/AnimacaoIdleDoPersonagem.hpp`, que controla intervalos aleatórios, sorteio de expressão e sequência da piscada.

---

## 5. Animações previstas

As animações futuras já possuem entradas previstas para:

- `Idle`
- `WalkBaixoDireita`
- `WalkBaixoEsquerda`
- `WalkCimaDireita`
- `WalkCimaEsquerda`

No momento, as animações de caminhada usam o idle como fallback de configuração visual.

Para adicionar sprites novas, altere a configuração da animação correspondente informando:

- Caminho da textura.
- Quantidade de frames.
- Origem do recorte.
- Largura do frame.
- Altura do frame.
- Espaçamento entre frames.
- Duração por frame.
- Offset visual próprio.
- Modo de reprodução (`FrameFixo`, `LoopContinuo` ou `IdleComPiscadas`).

A escolha de qual animação visual corresponde a `Parado` ou `Andando + Direção` fica na apresentação. Novas spritesheets de caminhada não exigem alteração em `src/Dominio/Personagem/Personagem.hpp`.

---

## 6. Cuidados contra regressão

Esta seção reúne os pontos que não devem ser alterados sem uma decisão explícita de mudança de contrato da mecânica.

### 6.1. Não colocar números mágicos no renderizador

Não coloque números mágicos de recorte, escala ou offset em:

`src/Apresentacao/Renderizacao/Mundo/RenderizadorDoPersonagem.hpp`

Centralize os ajustes visuais do personagem em:

`src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp`

### 6.2. Não quebrar a âncora pelos pés

A posição lógica deve continuar representando a base do personagem no chão.

O sprite deve ser deslocado a partir dessa base, e não o contrário.

Essa regra mantém coerente a relação entre:

- Posição dos pés.
- Movimento na grade de ocupação.
- Desenho do sprite.
- Escala aplicada pelo zoom da câmera.

### 6.3. Não trocar o modelo de movimento sem alterar o contrato

Não troque o movimento isométrico em segmentos por movimento direto pixel a pixel sem mudar explicitamente o contrato da mecânica.

O comportamento atual é:

- Clique no chão válido.
- Conversão para `PosicaoNaGradeDeOcupacao`.
- Criação de caminho simples em “L” isométrico.
- Movimento em segmentos até o destino.

Qualquer mudança para movimento livre, pathfinding com obstáculos ou interpolação direta em pixels deve ser tratada como alteração de mecânica, não como simples ajuste visual.
