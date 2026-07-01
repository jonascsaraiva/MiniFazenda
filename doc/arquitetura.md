# Arquitetura do MiniFazenda2

**Projeto:** MiniFazenda2  
**Tecnologia:** engine customizada em C++17 com SDL2  
**Status:** ativo, com arquitetura em camadas já aplicada de forma incremental

## Sumário

1. [Objetivo da arquitetura](#1-objetivo-da-arquitetura)
2. [Camadas principais](#2-camadas-principais)
3. [Regras obrigatórias](#3-regras-obrigatórias)
4. [Estrutura atual do projeto](#4-estrutura-atual-do-projeto)
5. [Fluxo de dependências](#5-fluxo-de-dependências)
6. [Padrão de ferramentas](#6-padrão-de-ferramentas)
7. [Padrão de plantas](#7-padrão-de-plantas)
8. [Estado do canteiro](#8-estado-do-canteiro)
9. [Grade encapsulada](#9-grade-encapsulada)
10. [Progressão do grid](#10-progressão-do-grid)
11. [Build](#11-build)
12. [Validações recomendadas](#12-validações-recomendadas)
13. [Pontos legados aceitos por enquanto](#13-pontos-legados-aceitos-por-enquanto)

---

## 1. Objetivo da arquitetura

O MiniFazenda2 separa regra de jogo, orquestração, renderização e infraestrutura para evitar:

- arquivos monolíticos;
- acoplamento com SDL2 no domínio;
- mutações globais sem validação.

A arquitetura atual já está dividida em camadas dentro da pasta `src/`, com responsabilidades bem definidas entre domínio, aplicação, apresentação, infraestrutura e elementos compartilhados.

---

## 2. Camadas principais

### `Compartilhado/`

Contém tipos puros e constantes pequenas usadas por várias camadas.

Esta camada deve permanecer simples e independente, servindo como base comum para estruturas como posições, constantes e tipos primitivos reutilizáveis.

### `Dominio/`

Contém as regras do jogo em C++ puro, sem dependência de SDL2.

É aqui que devem ficar as decisões de gameplay, como regras de canteiros, plantas, ferramentas, grade, jogador e demais comportamentos centrais do jogo.

O dominio nao deve conter representacao visual de entidades. No personagem, isso significa manter apenas estado logico como parado, andando, direcao, posicao dos pes e caminho em execucao; indice de frame, spritesheet, piscadas e temporizacao de animacao pertencem a apresentacao/infraestrutura.

### `Aplicacao/`

Contém o estado agregado e os casos de uso que orquestram o domínio.

Esta camada coordena as ações do jogo, como inicializar a fazenda, aplicar ferramentas e avançar o tempo, mas não deve assumir responsabilidades visuais nem lidar diretamente com SDL.

`Aplicacao/Estado/EstadoDoJogo.hpp` representa somente estado de gameplay: grade, jogador, personagem, ferramenta/seed selecionada, tamanho jogavel do grid e acumuladores usados pela logica da partida. Estados temporarios de interface ou preferencias da cena nao pertencem a esta camada.

### `Apresentacao/`

Contém câmera, isometria, interface e renderização com SDL.

Esta camada lê o estado do jogo e transforma esse estado em saída visual. Ela pode lidar com câmera, layout, cursores, HUD, barra de ferramentas, renderização da fazenda e desenho de elementos visuais.

A animacao visual do personagem tambem fica nesta camada: ela le o estado logico exposto pelo dominio e mantem o estado visual necessario para desenhar frames, piscadas e loops de spritesheet.

Estados temporarios da cena, como painel de configuracoes aberto, painel da loja aberto e audio mutado na cena, ficam em `Apresentacao/Interface/EstadoDaCenaFazenda.hpp`. A apresentacao consome esse estado para desenhar HUD, painel de configuracoes e demais elementos visuais sem contaminar o estado de gameplay.

### `Infraestrutura/`

Contém SDL, janela, renderer, assets, áudio, filesystem e leitura de `config.ini`.

Esta camada isola detalhes externos e técnicos, como inicialização de bibliotecas, localização de arquivos, carregamento de texturas, áudio e configurações.

Resumo das camadas:

```text
Compartilhado/     Tipos puros e constantes pequenas usadas por várias camadas.
Dominio/           Regras do jogo em C++ puro, sem SDL2.
Aplicacao/         Estado agregado e casos de uso que orquestram o domínio.
Apresentacao/      Câmera, isometria, interface e renderização com SDL.
Infraestrutura/    SDL, janela, renderer, assets, áudio, filesystem e config.ini.
```

---

## 3. Regras obrigatórias

Estas regras devem ser preservadas durante a evolução do projeto.

### 3.1. Domínio sem SDL

A pasta `Dominio/` não pode incluir:

- `SDL.h`;
- `SDL_image.h`;
- `SDL_ttf.h`;
- `SDL_mixer.h`;
- renderer;
- textura;
- janela;
- áudio;
- qualquer outro tipo visual ou específico de infraestrutura.

O domínio deve continuar sendo C++ puro.

### 3.2. Regra de jogo no domínio

A regra de jogo fica no domínio.

A apresentação apenas lê o estado e desenha. Ela não deve decidir regra de gameplay.

### 3.3. Grade protegida contra mutação externa

A grade não deve expor seus `std::vector` para mutação externa.

Quando listas internas precisarem ser acessadas fora da classe, o acesso deve ser somente por referência constante.

### 3.4. Ferramentas com polimorfismo

Ferramentas de jogo usam polimorfismo.

Regras de ferramenta não devem voltar para um modelo procedural baseado em `switch/case`.

### 3.5. Plantas com classe base

Plantas usam uma classe base chamada `Planta`.

Espécies novas devem herdar dessa base.

### 3.6. Canteiro como dono das transições

Canteiros centralizam suas próprias transições.

Código externo não deve alterar diretamente:

- estado visual;
- tempo;
- planta;
- fase interna do canteiro.

### 3.7. Namespaces seguindo a pasta

Os namespaces seguem a organização das pastas:

- `MiniFazenda::Dominio`
- `MiniFazenda::Aplicacao`
- `MiniFazenda::Apresentacao`
- `MiniFazenda::Infraestrutura`
- `MiniFazenda::Compartilhado`

### 3.8. Uso permitido de `switch/case`

`switch/case` é aceitável em bordas visuais ou mapeamentos de infraestrutura, como escolher ícone, cor ou som.

Ele não deve decidir regra de domínio.

### 3.9. Estado de gameplay separado do estado da cena

`EstadoDoJogo` deve conter apenas informacoes necessarias para regras e progresso da partida.

Flags de interface, preferencia visual/sonora da cena, paineis abertos e estados temporarios de HUD pertencem a estruturas da apresentacao, como `EstadoDaCenaFazenda`.

---

## 4. Estrutura atual do projeto

### `src/Principal.cpp`

Entry point SDL.

Responsável por criar a janela, carregar recursos, montar o estado inicial e executar o loop principal do jogo.

### `src/Compartilhado/`

#### `Constantes.hpp`

Contém constantes primitivas compartilhadas.

#### `Geometria/Posicoes.hpp`

Contém `PosicaoNaTela`, `PosicaoNaGrade` e comparadores puros.

### `src/Dominio/`

#### `Canteiros/EstadoDoCanteiro.hpp`

Define os estados possíveis do canteiro.

#### `Canteiros/Canteiro.hpp`

Centraliza o estado e as transições válidas do canteiro, como:

- arar;
- plantar;
- acelerar;
- colher;
- avançar crescimento.

### `src/Dominio/Plantas/`

#### `Planta.hpp`

Interface base polimórfica para espécies de planta.

#### `FabricaDePlantas.hpp`

Criação centralizada de plantas a partir da semente selecionada.

#### `Especies/PlantaMirtilo.hpp`

Espécie inicial, preservando tempos e recompensa legados.

### `src/Dominio/Ferramentas/`

Contém o sistema de ferramentas baseado em polimorfismo.

Arquivos principais:

- `Ferramenta.hpp`
- `TipoDeFerramenta.hpp`
- `ResultadoDaFerramenta.hpp`
- `RegistroDeFerramentas.hpp`
- `Enxada.hpp`
- `FerramentaDeSemente.hpp`
- `CursorDeColheita.hpp`
- `FerramentaDaLoja.hpp`
- `RemovedorDeTerra.hpp`

Cada ferramenta implementa sua própria regra de aplicação.

### `src/Dominio/Grade/`

#### `TileDeTerra.hpp`

Representa o tile de terra usado pela grade.

#### `GradeGlobalDeCanteiros.hpp`

Responsável pelo armazenamento da grade global, índices internos e validações de posição.

### `src/Dominio/Jogador/Jogador.hpp`

Controla:

- moedas;
- experiência;
- nível;
- aplicação de recompensa.

### `src/Dominio/Personagem/Personagem.hpp`

Controla apenas estado logico de gameplay:

- posicao dos pes na grade;
- estado parado/andando;
- direcao isometrica atual;
- caminho em segmentos.

Nao expoe animacao visual, spritesheet, indice de frame, piscada ou tempo de desenho.

### `src/Aplicacao/`

#### `Estado/EstadoDoJogo.hpp`

Agrega:

- grade;
- jogador;
- personagem;
- ferramenta selecionada;
- semente selecionada;
- tamanho atual do grid jogavel;
- tempo acumulado.

Nao agrega painel de configuracoes, audio mutado, painel da loja ou outros estados temporarios de interface.

#### `Servicos/InicializadorDaFazenda.hpp`

Inicializa a fazenda e cria o estado inicial do jogo.

#### `Servicos/ServicoDeFerramentas.hpp`

Coordena a aplicação de ferramentas sobre o domínio.

#### `Servicos/ServicoDeTempo.hpp`

Coordena o avanço de tempo do jogo.

Esses serviços coordenam o domínio sem tocar diretamente em SDL.

### `src/Apresentacao/`

Arquivos e módulos principais:

- `Animacao/AnimadorDoPersonagem.hpp`
- `Animacao/AnimacaoIdleDoPersonagem.hpp`
- `ConfiguracoesDoLayout.hpp`
- `Camera/CameraDoJogo.hpp`
- `Isometria/Isometrico.hpp`
- `Interface/AreaDeInteracao.hpp`
- `Interface/EstadoDaCenaFazenda.hpp`
- `Interface/BarraDeFerramentas/BarraDeFerramentas.hpp`
- `Renderizacao/Primitivas/PrimitivasSDL.hpp`
- `Renderizacao/Mundo/DesenhoDoMundo.hpp`
- `Renderizacao/Mundo/RenderizadorDaFazenda.hpp`
- `Renderizacao/UI/IconesDasFerramentas.hpp`
- `Renderizacao/UI/BarraDeFerramentasRenderer.hpp`
- `Renderizacao/UI/HudRenderer.hpp`
- `Renderizacao/Cursores/CursorCustomizado.hpp`

Essa camada concentra câmera, isometria, interface, estado temporario da cena e desenho visual do jogo.

### `src/Infraestrutura/`

#### `SDL/ContextoSDL.hpp`

Responsável pela inicialização SDL, janela, renderer e cursor oculto com RAII.

#### `Assets/GerenciadorDeAtivosSDL.hpp`

Gerencia ativos SDL carregados.

#### `Assets/LocalizadorDeAssets.hpp`

Localiza arquivos de assets usados pelo jogo.

#### `Assets/RecursosDaFazenda.hpp`

Agrupa recursos visuais da fazenda.

#### `Configuracao/LeitorDeConfiguracao.hpp`

Responsável pela leitura de configurações, incluindo `config.ini`.

---

## 5. Fluxo de dependências

O fluxo esperado de dependências é de cima para baixo, sem retorno indevido do domínio para camadas visuais ou técnicas.

### `Principal.cpp`

Pode depender de:

- `Infraestrutura`;
- `Apresentacao`;
- `Aplicacao`.

### `Infraestrutura`

Pode depender de `Apresentacao` quando precisa preencher configuração visual.

Também pode depender de `Dominio` apenas para mapear resultados de ações para sons ou texturas.

### `Apresentacao`

Pode depender de `Aplicacao` para ler `EstadoDoJogo`.

Também pode depender de `Dominio` para ler estados, grade e ferramenta selecionada.

Tambem e dona de estados temporarios da cena/interface, como `EstadoDaCenaFazenda`, consumidos por HUD e renderizadores.

### `Aplicacao`

Pode depender de:

- `Dominio`;
- `Compartilhado`.

### `Dominio`

Pode depender apenas de `Compartilhado`.

### Regra central

O domínio nunca depende de:

- SDL;
- apresentação;
- infraestrutura.

Representação resumida:

```text
Principal.cpp
  -> Infraestrutura, Apresentacao, Aplicacao

Infraestrutura
  -> Apresentacao quando precisa preencher configuracao visual
  -> Dominio apenas para mapear resultados de acoes para sons/texturas

Apresentacao
  -> Aplicacao para ler EstadoDoJogo
  -> Dominio para ler estados, grade e ferramenta selecionada
  -> EstadoDaCenaFazenda para estado temporario de interface da cena

Aplicacao
  -> Dominio
  -> Compartilhado

Dominio
  -> Compartilhado
```

---

## 6. Padrão de ferramentas

Toda ferramenta deriva de:

`Dominio::Ferramentas::Ferramenta`

E implementa o método:

```cpp
ResultadoDaFerramenta aplicar(ContextoDaFerramenta&, PosicaoNaGrade) const;
```

`RegistroDeFerramentas` guarda as instâncias por `TipoDeFerramenta`.

Esse padrão elimina o `switch` antigo de `SistemasDoJogo.hpp` e mantém cada regra dentro da própria ferramenta.

### Como adicionar uma ferramenta

Para adicionar uma nova ferramenta:

1. Criar uma classe em `Dominio/Ferramentas/`.
2. Implementar `tipo()`.
3. Implementar `aplicar()`.
4. Registrar a ferramenta em `RegistroDeFerramentas`.
5. Criar desenho e botão em `Apresentacao`, caso a ferramenta seja selecionável pela UI.

---

## 7. Padrão de plantas

`Planta` define:

- identificador da semente;
- nome;
- custo;
- tempo para crescer;
- tempo para maturar;
- tempo para morrer;
- recompensa.

`Planta` nao conhece pasta, sprite, caminho de asset ou convencao visual.

`Canteiro` contém uma planta polimórfica e consulta essa planta para saber a fase visual em cada segundo. A fase continua sendo derivada das regras de tempo da planta.

A infraestrutura resolve os assets por meio de `Infraestrutura/Assets/CatalogoVisualDePlantas.hpp`, indexado por `identificadorDaSemente`.

### Como adicionar uma planta

Para adicionar uma nova planta:

1. Criar a espécie em `Dominio/Plantas/Especies/`.
2. Implementar `clonar()`.
3. Definir o identificador de semente.
4. Definir tempos de crescimento, maturação e morte.
5. Definir a recompensa.
6. Adicionar a criação em `FabricaDePlantas`.
7. Mapear a configuracao visual no catalogo de `Infraestrutura/Assets`, usando o mesmo `identificadorDaSemente`.

---

## 8. Estado do canteiro

`Canteiro` é o dono das transições.

Código externo deve chamar métodos de intenção, como:

```text
arar()
plantar(planta)
acelerarParaMadura()
colher()
avancarUmSegundo()
```

Essa abordagem centraliza o State Pattern de forma simples.

No momento, não há classes separadas por estado porque o comportamento atual ainda é pequeno.

Se as transições ganharem efeitos específicos por estado, elas devem ser extraídas para objetos de estado sem expor mutação direta.

---

## 9. Grade encapsulada

`GradeGlobalDeCanteiros` guarda internamente:

```text
tiles_
posicoesDeTilesExistentes_
posicoesDeCanteirosEmCrescimento_
```

Essas listas são expostas somente como referências constantes.

As mutações passam por métodos controlados:

```text
ativarTile()
removerTile()
sincronizarCrescimentoDoCanteiro()
removerCanteiroDaListaDeCrescimento()
```

Essa estrutura impede alteração direta da grade por código externo e preserva as validações internas.

---

## 10. Progressão do grid

O grid visual de `12 x 12`, definido por `TAMANHO_INICIAL_GRID`, representa a área jogável potencial da fase atual.

Ele não representa a quantidade de tiles disponíveis no início.

Por design, `criarEstadoInicialDoJogo()` ativa apenas um núcleo central de `2 x 2` tiles, criando um espaço inicial pequeno e legível.

O jogador usa a `Enxada` para ativar novos tiles dentro dessa área de `12 x 12`.

Essa expansão progressiva é a mecânica central de crescimento da fazenda neste momento do jogo.

Quando o sistema de progressão por nível for implementado, o tamanho máximo do grid jogável poderá crescer além dos `12 x 12` iniciais, respeitando o limite definido por `TAMANHO_MAXIMO_GRID`.

---

## 11. Build

O `CMakeLists.txt` usa:

```cmake
file(GLOB_RECURSE MINIFAZENDA_HEADERS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.hpp")
```

Com isso, novos headers entram nos targets automaticamente.

O projeto continua tendo:

- `src/Principal.cpp` como único `.cpp` da aplicação;
- `tests/TestesLogica.cpp` para testes de lógica.

---

## 12. Validações recomendadas

Comandos recomendados para validar build, testes e regras arquiteturais:

```powershell
cmake -S . -B build
cmake --build build --config Debug
ctest --test-dir build --output-on-failure -C Debug
rg "SDL|SDL_|IMG_|TTF_|Mix_" src/Dominio
rg "switch" src/Dominio
rg "frame|sprite|animacao|spritesheet|pisc" src/Dominio/Personagem src/Dominio/Animacao
```

As buscas de arquitetura devem retornar vazio.

Isso confirma que:

- o domínio não está dependendo de SDL ou bibliotecas relacionadas;
- regras de domínio não voltaram a ser decididas por `switch/case`.

---

## 13. Pontos legados aceitos por enquanto

Alguns pontos ainda são aceitos temporariamente porque não quebram o contrato principal da arquitetura.

### `Principal.cpp` ainda contém o loop SDL completo

`Principal.cpp` está menor e já usa serviços e módulos, mas ainda concentra o loop SDL completo.

Uma próxima etapa pode extrair uma `CenaFazenda` dedicada.

### A apresentação ainda usa `switch` para ícones e cores

Esse uso é permitido porque está em borda visual.

Ele não decide regra de domínio.

### A infraestrutura ainda mapeia ações para sons com `switch`

Esse uso também é permitido porque representa um mapeamento de borda.

Ele não é regra de gameplay.

