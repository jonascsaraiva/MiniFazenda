# Arquitetura do MiniFazenda2

Projeto: MiniFazenda2, engine customizada em C++17 com SDL2.
Status: ativo, com arquitetura em camadas ja aplicada de forma incremental.

## Objetivo

O MiniFazenda2 separa regra de jogo, orquestracao, renderizacao e infraestrutura para evitar arquivos monoliticos, acoplamento com SDL2 no dominio e mutacoes globais sem validacao.

As camadas principais vivem dentro de `src/`:

```text
Compartilhado/     Tipos puros e constantes pequenas usadas por varias camadas.
Dominio/           Regras do jogo em C++ puro, sem SDL2.
Aplicacao/         Estado agregado e casos de uso que orquestram o dominio.
Apresentacao/      Camera, isometria, interface e renderizacao com SDL.
Infraestrutura/    SDL, janela, renderer, assets, audio, filesystem e config.ini.
```

## Regras Obrigatorias

1. `Dominio/` nao pode incluir `SDL.h`, `SDL_image.h`, `SDL_ttf.h`, `SDL_mixer.h` nem tipos visuais como renderer, textura, janela ou audio.
2. A regra de jogo fica no dominio. A apresentacao apenas le estado e desenha.
3. A grade nao expoe seus `std::vector` para mutacao externa. Acesso externo a listas internas deve ser const.
4. Ferramentas de jogo usam polimorfismo. Regras de ferramenta nao devem voltar para um `switch/case` procedural.
5. Plantas usam uma classe base `Planta`; especies novas devem herdar dela.
6. Canteiros centralizam suas transicoes. Codigo externo nao deve alterar estado visual, tempo ou planta diretamente.
7. Namespaces seguem a pasta: `MiniFazenda::Dominio`, `MiniFazenda::Aplicacao`, `MiniFazenda::Apresentacao`, `MiniFazenda::Infraestrutura` e `MiniFazenda::Compartilhado`.
8. `switch/case` e aceitavel em bordas visuais ou mapeamentos de infraestrutura, como escolher icone ou som. Ele nao deve decidir regra de dominio.

## Estrutura Atual

```text
src/
  Principal.cpp
    Entry point SDL. Cria a janela, recursos, estado inicial e roda o loop.

  Compartilhado/
    Constantes.hpp
      Constantes primitivas compartilhadas.
    Geometria/Posicoes.hpp
      PosicaoNaTela, PosicaoNaGrade e comparadores puros.

  Dominio/
    Canteiros/
      EstadoDoCanteiro.hpp
      Canteiro.hpp
        Estado e transicoes validas: arar, plantar, acelerar, colher e avancar crescimento.

    Plantas/
      Planta.hpp
        Interface base polimorfica para especies.
      FabricaDePlantas.hpp
        Criacao centralizada de plantas a partir da semente atual.
      Especies/PlantaMirtilo.hpp
        Especie inicial, preservando tempos e recompensa legados.

    Ferramentas/
      Ferramenta.hpp
      TipoDeFerramenta.hpp
      ResultadoDaFerramenta.hpp
      RegistroDeFerramentas.hpp
      Enxada.hpp
      FerramentaDeSemente.hpp
      CursorDeColheita.hpp
      PresenteAcelerador.hpp
      RemovedorDeTerra.hpp
        Cada ferramenta implementa sua propria regra de aplicacao.

    Grade/
      TileDeTerra.hpp
      GradeGlobalDeCanteiros.hpp
        Armazenamento da grade global, indices internos e validacoes de posicao.

    Jogador/Jogador.hpp
      Moedas, experiencia, nivel e aplicacao de recompensa.

  Aplicacao/
    Estado/EstadoDoJogo.hpp
      Agrega grade, jogador, ferramenta selecionada e tempo acumulado.
    Servicos/
      InicializadorDaFazenda.hpp
      ServicoDeFerramentas.hpp
      ServicoDeTempo.hpp
        Casos de uso que coordenam dominio sem tocar em SDL.

  Apresentacao/
    ConfiguracoesDoLayout.hpp
    Camera/CameraDoJogo.hpp
    Isometria/Isometrico.hpp
    Interface/
      AreaDeInteracao.hpp
      BarraDeFerramentas/BarraDeFerramentas.hpp
    Renderizacao/
      Primitivas/PrimitivasSDL.hpp
      Mundo/DesenhoDoMundo.hpp
      Mundo/RenderizadorDaFazenda.hpp
      UI/IconesDasFerramentas.hpp
      UI/BarraDeFerramentasRenderer.hpp
      Cursores/CursorCustomizado.hpp

  Infraestrutura/
    SDL/ContextoSDL.hpp
      Inicializacao SDL, janela, renderer e cursor oculto com RAII.
    Assets/
      GerenciadorDeAtivosSDL.hpp
      LocalizadorDeAssets.hpp
      RecursosDaFazenda.hpp
    Configuracao/LeitorDeConfiguracao.hpp
```

## Fluxo de Dependencias

```text
Principal.cpp
  -> Infraestrutura, Apresentacao, Aplicacao

Infraestrutura
  -> Apresentacao quando precisa preencher configuracao visual
  -> Dominio apenas para mapear resultados de acoes para sons/texturas

Apresentacao
  -> Aplicacao para ler EstadoDoJogo
  -> Dominio para ler estados, grade e ferramenta selecionada

Aplicacao
  -> Dominio
  -> Compartilhado

Dominio
  -> Compartilhado
```

O dominio nunca depende de SDL, apresentacao ou infraestrutura.

## Padrao de Ferramentas

Toda ferramenta deriva de `Dominio::Ferramentas::Ferramenta` e implementa:

```cpp
ResultadoDaFerramenta aplicar(ContextoDaFerramenta&, PosicaoNaGrade) const;
```

`RegistroDeFerramentas` guarda as instancias por `TipoDeFerramenta`, eliminando o `switch` antigo de `SistemasDoJogo.hpp`.

Para adicionar uma ferramenta:

1. Criar uma classe em `Dominio/Ferramentas/`.
2. Implementar `tipo()` e `aplicar()`.
3. Registrar no `RegistroDeFerramentas`.
4. Criar desenho e botao em `Apresentacao`, se a ferramenta for selecionavel pela UI.

## Padrao de Plantas

`Planta` define tempo para crescer, maturar, morrer e recompensa. `Canteiro` contem uma planta polimorfica e consulta essa planta para saber a fase visual em cada segundo.

Para adicionar uma planta:

1. Criar a especie em `Dominio/Plantas/Especies/`.
2. Implementar `clonar()`, identificador de semente, tempos e recompensa.
3. Adicionar a criacao em `FabricaDePlantas`.
4. Mapear sprites em `Infraestrutura/Assets` ou no catalogo visual correspondente.

## Estado do Canteiro

`Canteiro` e o dono das transicoes. Codigo externo chama metodos de intencao:

```text
arar()
plantar(planta)
acelerarParaMadura()
colher()
avancarUmSegundo()
```

Essa abordagem centraliza o State Pattern de forma simples. Nao ha classes separadas por estado no momento porque o comportamento atual ainda e pequeno; se as transicoes ganharem efeitos especificos por estado, elas devem ser extraidas para objetos de estado sem expor mutacao direta.

## Grade Encapsulada

`GradeGlobalDeCanteiros` guarda internamente:

```text
tiles_
posicoesDeTilesExistentes_
posicoesDeCanteirosEmCrescimento_
```

As listas sao expostas somente como referencias const. Mutacoes passam por:

```text
ativarTile()
removerTile()
sincronizarCrescimentoDoCanteiro()
removerCanteiroDaListaDeCrescimento()
```

## Build

O `CMakeLists.txt` usa:

```cmake
file(GLOB_RECURSE MINIFAZENDA_HEADERS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.hpp")
```

Assim, novos headers entram nos targets automaticamente. O projeto continua tendo `src/Principal.cpp` como unico `.cpp` da aplicacao e `tests/TestesLogica.cpp` para testes de logica.

## Validacoes Recomendadas

```powershell
cmake -S . -B build
cmake --build build --config Debug
ctest --test-dir build --output-on-failure -C Debug
rg "SDL|SDL_|IMG_|TTF_|Mix_" src/Dominio
rg "switch" src/Dominio
```

As duas ultimas buscas devem retornar vazio.

## Pontos Legados Aceitos Por Enquanto

- `Principal.cpp` ainda contem o loop SDL completo. Ele esta menor e usa servicos/modulos, mas uma proxima etapa pode extrair uma `CenaFazenda` dedicada.
- A apresentacao ainda tem `switch` para escolher icones e cores. Isso e permitido porque nao decide regra de dominio.
- A infraestrutura ainda mapeia acoes para sons com `switch`. Isso e um mapeamento de borda, nao regra de gameplay.
