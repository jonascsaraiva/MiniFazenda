# MiniFazenda2

Base nativa em C++17 com SDL2 para a arquitetura v1.0.0.

## Estrutura

- `src/Principal.cpp`: inicializacao SDL2, game loop, fluxo de eventos, atualizacao por frame e orquestracao de renderizacao.
- `src/CaminhosDosAssets.hpp`: localizacao da pasta `assets` e caminhos centralizados de background/tiles.
- `src/Constantes.hpp`: constantes globais de janela, grade global 256x256 e tempo.
- `src/Tipos.hpp`: tipos comuns, estados, ferramentas, areas de interacao e configuracoes de layout.
- `src/CameraDoJogo.hpp`: zoom, pan, origem visual do grid, limite de camera e dimensoes renderizadas.
- `src/GradeDeCanteiros.hpp`: armazenamento da grade global, indices de tiles ativos e lista de canteiros em crescimento.
- `src/SistemasDoJogo.hpp`: estado do jogo, regras das ferramentas, moedas/experiencia e crescimento dos canteiros.
- `src/InterfaceDoJogo.hpp`: layout e processamento dos botoes da barra inferior.
- `src/Isometrico.hpp`: conversao entre tela, grade local e grade global isometrica.
- `src/Desenho.hpp`: funcoes de renderizacao SDL2.
- `src/Ativos.hpp`: cache de texturas, fontes, sons e musicas.
- `src/RecursosDoJogo.hpp`: carregamento dos recursos visuais e mapeamento de sons usados pelo jogo.
- `src/Configuracao.hpp`: leitor simples de `assets/config.ini`.
- `tests/TestesLogica.cpp`: testes da logica pura de grade, ferramentas e crescimento.
- `assets/`: midia separada do codigo.

## Assets esperados

O background principal deve ficar em:

- `assets/background/background.png`

Tiles temporarios suportados:

- `assets/sprites/tiles/tile_terra_seca.png`
- `assets/sprites/tiles/tile_terra_arada.png`
- `assets/sprites/tiles/tile_terra_restos.png`

O jogo continua com desenho de fallback quando os sprites ainda nao existem.
O background usa fallback verde se `assets/background/background.png` nao for localizado ou nao puder ser carregado.

## Build

Instale CMake, um compilador C++17 e as bibliotecas `SDL2`, `SDL2_image`, `SDL2_ttf` e `SDL2_mixer`.

```powershell
cmake -S . -B build
cmake --build build
.\build\bin\MiniFazenda2.exe
```

Para validar a logica pura:

```powershell
ctest --test-dir build --output-on-failure -C Debug
```

Durante o jogo, edite `assets/config.ini` e pressione `F5` para recarregar offsets e background.

## Camera

- Scroll do mouse: aproxima/afasta o grid entre 0.5x e 2.0x.
- Botao do meio ou direito: arrasta a camera sem interagir com tiles.
- Home: centraliza a camera e volta o zoom para 1.0x.
- `centroVisualBackgroundX/Y` em `assets/config.ini`: ajusta o centro visual usado para encaixar o grid no background.

## Ferramentas atuais

- Cursor: colhe planta madura.
- Enxada: cria terra seca quando o tile ainda nao existe; em terra seca/restos, transforma em terra arada.
- Remover terra: remove o tile existente da grade global invisivel.
- Semente: planta em terra arada, consumindo moedas.
- Presente: acelera crescimento para planta madura.
