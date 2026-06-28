# MiniFazenda2

Base nativa em C++17 com SDL2 para a arquitetura v1.0.0.

## Estrutura

- `src/Compartilhado/`: constantes e tipos puros como posicoes de tela/grade.
- `src/Dominio/`: regras de jogo em C++ puro, incluindo grade, canteiros, plantas, ferramentas, jogador e economia.
- `src/Aplicacao/`: estado agregado e servicos de caso de uso, como aplicar ferramenta e avancar tempo.
- `src/Apresentacao/`: camera, isometria, interface e renderizacao SDL2.
- `src/Infraestrutura/`: inicializacao SDL, janela, renderer, assets, audio, filesystem e configuracao.
- `src/Principal.cpp`: entry point SDL, loop principal e conexao entre as camadas.
- `tests/TestesLogica.cpp`: testes da logica pura de grade, ferramentas e crescimento.
- `assets/`: midia separada do codigo.
- `doc/ARCHITECTURE.md`: regras arquiteturais atuais e mapa completo de responsabilidades.

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
cmake --build build --config Debug
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
- Semente: planta a especie selecionada na Loja em terra arada, consumindo moedas.
- Loja: abre a selecao de sementes disponiveis.
