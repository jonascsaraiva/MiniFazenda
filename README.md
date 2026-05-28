# MiniFazenda2

Base nativa em C++17 com SDL2 para a arquitetura v1.0.0.

## Estrutura

- `src/Principal.cpp`: inicializacao SDL2 e game loop.
- `src/Constantes.hpp`: constantes globais de janela, grade e tempo.
- `src/Tipos.hpp`: estruturas, estados e areas de interacao.
- `src/Isometrico.hpp`: conversao entre tela e grade isometrica.
- `src/Desenho.hpp`: funcoes de renderizacao SDL2.
- `src/Ativos.hpp`: cache de texturas, fontes, sons e musicas.
- `src/Configuracao.hpp`: leitor simples de `assets/config.ini`.
- `assets/`: midia separada do codigo.

## Assets esperados

O jogo roda com desenho de fallback quando estes arquivos ainda nao existem:

- `assets/background/fazenda.png`
- `assets/fonts/interface.ttf`
- `assets/sounds/ambiente.ogg`
- `assets/sounds/clique.wav`
- `assets/sounds/interacao.wav`
- `assets/sprites/casa.png`
- `assets/sprites/casinha_cachorro.png`
- `assets/sprites/terra_vazia.png`
- `assets/sprites/terra_arada.png`
- `assets/sprites/semente_plantada.png`
- `assets/sprites/planta_crescendo.png`
- `assets/sprites/planta_madura.png`
- `assets/sprites/planta_morta.png`

## Build

Instale CMake, um compilador C++17 e as bibliotecas `SDL2`, `SDL2_image`, `SDL2_ttf` e `SDL2_mixer`.

```powershell
cmake -S . -B build
cmake --build build
.\build\bin\MiniFazenda2.exe
```

Durante o jogo, edite `assets/config.ini` e pressione `F5` para recarregar as posicoes da grade, casa e casinha.
