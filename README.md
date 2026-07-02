# MiniFazenda2

MiniFazenda2 é um jogo em C++17 com SDL2. O ambiente oficial de desenvolvimento é Windows com MSYS2/UCRT64, GCC, CMake, Ninja e as bibliotecas SDL2 usadas pelo projeto.

## Estrutura principal

- `src/`: código-fonte do jogo.
- `assets/`: imagens, fontes, sons e `config.ini` usados em tempo de execução.
- `tests/`: testes de lógica registrados no CTest como `MiniFazenda2Tests`.
- `doc/`: documentação complementar do projeto.
- `.vscode/tasks.json`: atalhos opcionais para configurar, compilar e rodar.
- `build/`: diretório gerado pelo CMake. Os assets são copiados para `build/bin/assets` após a compilação.

## Requisitos

Instale o MSYS2 e use o ambiente UCRT64 em:

```text
C:\msys64\ucrt64
```

No terminal MSYS2 UCRT64, instale as dependências:

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja mingw-w64-ucrt-x86_64-SDL2 mingw-w64-ucrt-x86_64-SDL2_image mingw-w64-ucrt-x86_64-SDL2_ttf mingw-w64-ucrt-x86_64-SDL2_mixer mingw-w64-ucrt-x86_64-libwebp
```

O `CMakeLists.txt` exige CMake 3.20 ou superior e compila o projeto como C++17.

O alvo `validar-arquitetura` usa `rg` quando ele está disponível no ambiente. Se `rg` não for encontrado, o CMake mantém o alvo, mas apenas informa que as validações de arquitetura foram ignoradas.

## Configurar, compilar e testar

A partir da raiz do projeto, use os binários do UCRT64 no PowerShell:

```powershell
C:\msys64\ucrt64\bin\cmake.exe -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=C:/msys64/ucrt64 -DCMAKE_C_COMPILER=C:/msys64/ucrt64/bin/gcc.exe -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe
```

Compile:

```powershell
C:\msys64\ucrt64\bin\cmake.exe --build build
```

Execute os testes:

```powershell
C:\msys64\ucrt64\bin\ctest.exe --test-dir build --output-on-failure
```

Execute a validação de arquitetura:

```powershell
C:\msys64\ucrt64\bin\cmake.exe --build build --target validar-arquitetura
```

Para limpar o diretório de build antes de configurar novamente:

```powershell
Remove-Item -Recurse -Force build -ErrorAction SilentlyContinue
```

## Executar o jogo

Depois da compilação:

```powershell
.\build\bin\MiniFazenda2.exe
```

## VSCode

As tarefas do VSCode são opcionais. `Ctrl + Shift + B` executa a task padrão, que automatiza o fluxo de limpar `build`, configurar CMake, compilar com Ninja e iniciar `MiniFazenda2.exe`.

O README documenta os comandos oficiais; as tasks apenas chamam esses comandos a partir do workspace.

## Observação importante

Use sempre os binários do UCRT64:

```text
C:\msys64\ucrt64\bin\cmake.exe
C:\msys64\ucrt64\bin\ninja.exe
C:\msys64\ucrt64\bin\g++.exe
```

Evite misturar com o CMake instalado em `C:\Program Files\CMake`, porque isso pode fazer o CMake não encontrar corretamente dependências instaladas pelo MSYS2, como `libwebp`, `SDL2_image`, `SDL2_ttf` e `SDL2_mixer`.
