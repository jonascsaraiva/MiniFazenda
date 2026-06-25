=======================================================================
                         O ENTRY POINT
                         [Principal.cpp]
  (Apenas inicia o loop, cria a CenaFazenda e passa pro SDL rodar)
=======================================================================
                               |
                               V
+---------------------------------------------------------------------+
|                  CAMADA DE BORDAS EXTERNAS (I/O)                    |
|             (Tudo que toca no mundo real e no SDL2)                 |
|                                                                     |
|    [Apresentacao/]                        [Infraestrutura/]         |
|    - Renderizacao (Mundo, UI)             - SDL (Init, Janela)      |
|    - Isometria (Matemática visual)        - Assets (Texturas, Som)  |
|    - Camera, Botoes, HUD                  - Leitor INI              |
+---------------------------------------------------------------------+
                               |
         (Lê botões clicados)  |  (Desenha o estado atual)
                               V
+---------------------------------------------------------------------+
|                CAMADA DE APLICAÇÃO (Orquestração)                   |
|          (Faz a ponte entre a tela e as regras do jogo)             |
|                                                                     |
|    [Aplicacao/]                                                     |
|    - Cenas (CenaFazenda: coordena update e render)                  |
|    - Entrada (Traduz "Clique X/Y" -> "Usar Ferramenta")             |
|    - Servicos (avancarTempo(), inicializarFazenda())                |
|    - EstadoDoJogo (Agregador)                                       |
+---------------------------------------------------------------------+
                               |
          (Chama ações lógicas)| (Consulta dados pra renderizar)
                               V
+---------------------------------------------------------------------+
|              CAMADA DE DOMÍNIO (O Coração do Jogo)                  |
|             (C++ Puro! Zero SDL, Zero Tela, Zero Som)               |
|                                                                     |
|    [Dominio/]                                                       |
|    - Grade (Matriz de Tiles)                                        |
|    - Canteiros (Máquina de estados: Vazio -> Arado -> Plantado)     |
|    - Ferramentas (Polimorfismo: Enxada, Semente, Removedor)         |
|    - Plantas (Herança: Milho, Morango, Fases de Crescimento)        |
|    - Jogador (Moedas, XP) e Economia                                |
+---------------------------------------------------------------------+

=======================================================================
                        [Compartilhado/]
    (Tipos puros e utilitários globais: PosicaoNaGrade, Geometria)
       * Acessível por TODAS as camadas acima, sem restrições.
=======================================================================

Guia de Arquitetura e Padrões de Código
Projeto: MiniFazenda2 (Engine Customizada C++ / SDL2)
Status: Ativo / Em Refatoração
Padrões Adotados: Clean Architecture, Domain-Driven Design (DDD), POO (Polimorfismo e State Pattern).

1. Introdução e Propósito
Este documento descreve a organização de pastas, arquivos e a arquitetura de código do projeto MiniFazenda2. O objetivo desta estrutura é resolver problemas de "God Functions" (funções que fazem de tudo), alto acoplamento com a biblioteca SDL2 e falta de encapsulamento.

A nova arquitetura garante que a lógica de negócio (Domínio) do jogo seja 100% agnóstica de tecnologia visual, permitindo expansões futuras (como dezenas de novas plantas e ferramentas) sem risco de quebrar o que já funciona (Regressão).

2. Regras de Ouro da Arquitetura (Invariantes)
Para que essa arquitetura funcione, as seguintes regras nunca devem ser quebradas:

A Regra da Dependência: O código flui de fora para dentro. A pasta Dominio/ NÃO PODE incluir bibliotecas visuais ou de áudio (#include <SDL.h>). Se um erro de compilação exigir SDL no domínio, o design está errado.

Encapsulamento Restrito: Estruturas de dados globais (como a Grade) não podem expor seus std::vector publicamente. Toda mutação deve ser feita por métodos da própria classe.

Fim dos Switches de Domínio: O uso de switch/case para checar tipos de ferramentas ou fases de sementes é proibido. Deve-se usar Polimorfismo (classes derivadas de Ferramenta ou Planta) e State Pattern para os canteiros.

Isolamento de Namespaces: O código deve estar encapsulado no namespace correspondente à sua pasta (ex: MiniFazenda::Dominio::Plantas).

3. Estrutura de Diretórios e Responsabilidades
A árvore do código-fonte (dentro de src/) é dividida em 5 grandes áreas:

🔹 Compartilhado/
Tipos puros e utilitários pequenos que circulam por todas as camadas do projeto.

Geometria/: PosicaoNaTela, PosicaoNaGrade. Não possuem regras de jogo, apenas matemática básica e comparadores.

🔹 Dominio/ (O Coração do Jogo)
Onde as regras do jogo vivem. O C++ aqui é puro.

Grade/: GradeGlobalDeCanteiros, TileDeTerra. Classes que controlam onde existem canteiros e validam posições.

Canteiros/: Máquina de estados do canteiro. Regras de transição de Vazio -> Arado -> Plantado.

Plantas/: Classe base Planta, com subclasses em Especies/ (PlantaMilho, PlantaMorango). Fases de crescimento definidas por tempo.

Ferramentas/: Interface abstrata Ferramenta. Especializações: Enxada, Semente, etc.

Jogador/ & Economia/: Regras de cálculo de nível, moedas recebidas, descontos, experiência.

🔹 Aplicacao/ (A Orquestração)
Faz a ponte entre a entrada do usuário e as regras do domínio.

Estado/: EstadoDoJogo agregado que junta a grade, a economia e as configurações atuais.

Servicos/: Casos de uso como aplicarFerramenta(coordenada), avancarTempoGlobal().

Entrada/: Tradução de botões apertados/mouse para intenções no jogo.

Cenas/: A CenaFazenda coordena os updates da lógica e manda a Apresentação renderizar tudo.

🔹 Apresentacao/ (A Tela)
Exclusivo para ler os estados do Dominio e desenhar na tela.

Isometria/: A matemática visual (exclusiva para o desenho) que converte matriz em tela.

Renderizacao/: O desenho real com SDL_Renderer. Subdividido em Mundo (chão e plantas), UI (botões) e Cursores.

Camera/: Controle do zoom, limite do pan e inércia do mapa.

🔹 Infraestrutura/ (A Máquina e os Arquivos)
Tudo o que fala com o Sistema Operacional e o hardware.

SDL/: Inicialização da janela principal, limpeza de memória RAII (destrutores de janela/renderer).

Assets/: Carregadores usando Template genérico. LocalizadorDeAssets (lê os PNGs da pasta e mapeia).

Configuracao/: O parser que lê o config.ini e entrega os dados em um struct seguro.

4. Plano de Implementação e Refatoração
A transição dos arquivos legados (SistemasDoJogo.hpp, Desenho.hpp, etc) para a nova arquitetura deve seguir esta ordem para evitar que o código quebre:

Fase 1: Infraestrutura e Compartilhado

Extrair os structs básicos de Tipos.hpp para Compartilhado/Geometria/.

Criar a classe de leitura universal no Infraestrutura/Assets/ (eliminando o código duplicado de texturas/fontes/sons).

Isolar a incialização do SDL saindo de Principal.cpp para Infraestrutura/SDL/.

Fase 2: Isolamento do Domínio (O mais crítico)

Transformar GradeDeCanteiros.hpp em uma classe dentro de Dominio/Grade/. Esconder os vetores como private.

Substituir os structs passivos do canteiro pela classe Canteiro.hpp com validação de estado (State Pattern).

Migração de Ferramentas: Criar a classe base Ferramenta. Mover cada case do antigo SistemasDoJogo.hpp para uma classe específica (ex: Enxada.cpp).

Fase 3: Apresentação (Renderização Desacoplada)

Quebrar o gigante Desenho.hpp em renderizadores específicos (Apresentacao/Renderizacao/Mundo, UI).

Os métodos de desenho não podem mais decidir qual estado o canteiro está, apenas ler do Domínio e pintar a textura correspondente.

Fase 4: Aplicação (Unindo as Pontas)

O loop principal em Principal.cpp será enxugado. Ele passará a instanciar a CenaFazenda e rodar seu update() e render().

5. Tratamento de Regressões (Prevenção de Bugs)
A refatoração levanta um risco temporário de Regressão (quebrar lógicas que hoje funcionam). Para mitigar isso:

Tática de Estrangulamento (Strangler Pattern): Não apague o bloco inteiro do SistemasDoJogo.hpp de uma vez. Migre uma ferramenta (ex: Enxada). Faça o jogo usar o novo sistema para a Enxada, mas mantenha o switch legado para as demais. Teste, valide e depois migre a próxima.

Blindagem de Invariantes: Ao encapsular a Grade (Dominio/Grade), erros de índice e vetores corrompidos não passarão do compilador, evitando crashes silenciosos no loop da câmera.

Testes Frequentes no CMake: Devido à criação de muitos arquivos, garanta que o CMakeLists.txt usa inclusão em lote (file(GLOB_RECURSE SOURCES "src/*.cpp")) ou atualize-o a cada novo arquivo criado. Após cada classe migrada, rode o comando ninja para garantir que a lintagem e as dependências #include estão corretas.

Versão do Documento: 1.0
Autor: Jonas saraiva
Data: 25/06/2026 17:00