# MiniFazenda2

Base nativa em C++17 com SDL2 para a arquitetura v1.0.0.

## Estrutura

- `src/Principal.cpp`: inicializacao SDL2, game loop, aplicacao das ferramentas e renderizacao da grade ativa.
- `src/CaminhosDosAssets.hpp`: localizacao da pasta `assets` e caminhos centralizados de background/tiles.
- `src/Constantes.hpp`: constantes globais de janela, grade global 256x256 e tempo.
- `src/Tipos.hpp`: estruturas, estados, ferramentas, areas de interacao e operacoes basicas da grade global.
- `src/Isometrico.hpp`: conversao entre tela, grade local e grade global isometrica.
- `src/Desenho.hpp`: funcoes de renderizacao SDL2.
- `src/Ativos.hpp`: cache de texturas, fontes, sons e musicas.
- `src/Configuracao.hpp`: leitor simples de `assets/config.ini`.
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

Durante o jogo, edite `assets/config.ini` e pressione `F5` ou clique no botao de configuracao para recarregar offsets e background.

## Ferramentas atuais

- Cursor: colhe planta madura.
- Enxada: cria terra seca quando o tile ainda nao existe; em terra seca/restos, transforma em terra arada.
- Remover terra: remove o tile existente da grade global invisivel.
- Semente: planta em terra arada, consumindo moedas.
- Presente: acelera crescimento para planta madura.
