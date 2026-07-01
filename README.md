## Como compilar e rodar o MiniFazenda2

O projeto usa CMake, Ninja, GCC via MSYS2/UCRT64 e SDL2.

Antes de compilar, confirme que o MSYS2/UCRT64 está instalado em:

```text
C:\msys64\ucrt64
```

Dependências usadas pelo projeto:

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja mingw-w64-ucrt-x86_64-SDL2 mingw-w64-ucrt-x86_64-SDL2_image mingw-w64-ucrt-x86_64-SDL2_ttf mingw-w64-ucrt-x86_64-SDL2_mixer mingw-w64-ucrt-x86_64-libwebp
```

### Rodar pelo VSCode

No VSCode, use:

```text
Ctrl + Shift + B
```

A task padrão executa o processo completo:

```text
Limpar build -> Configurar CMake -> Compilar -> Rodar o jogo
```

Ela usa explicitamente o caminho:

```text
C:/msys64/ucrt64
```

Isso evita erro de dependências como `webp`, `webpdemux`, SDL2_image e outros pacotes do MSYS2.

### Rodar por comando no PowerShell

A partir da raiz do projeto:

```powershell
cd C:\dev\MiniFazenda
```

Para limpar, configurar e compilar:

```powershell
Remove-Item -Recurse -Force build -ErrorAction SilentlyContinue

C:\msys64\ucrt64\bin\cmake.exe -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=C:/msys64/ucrt64 -DCMAKE_C_COMPILER=C:/msys64/ucrt64/bin/gcc.exe -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe

C:\msys64\ucrt64\bin\cmake.exe --build build
```

Para rodar o jogo:

```powershell
cd C:\dev\MiniFazenda\build
.\bin\MiniFazenda2.exe
```

### Rodar sem limpar o build

Se o projeto já foi configurado antes, basta usar:

```powershell
cd C:\dev\MiniFazenda\build

C:\msys64\ucrt64\bin\ninja.exe
.\bin\MiniFazenda2.exe
```

### Observação importante

Use sempre os binários do UCRT64:

```text
C:\msys64\ucrt64\bin\cmake.exe
C:\msys64\ucrt64\bin\ninja.exe
C:\msys64\ucrt64\bin\g++.exe
```

Evite misturar com o CMake instalado em `C:\Program Files\CMake`, porque isso pode fazer o CMake não encontrar corretamente dependências instaladas pelo MSYS2, como `libwebp`, `SDL2_image`, `SDL2_ttf` e `SDL2_mixer`.
