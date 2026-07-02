# Segunda auditoria tecnica do MiniFazenda2

Data da auditoria: 2026-07-02

Esta auditoria substitui a auditoria anterior deste arquivo. A analise abaixo foi feita a partir do codigo atual do projeto, nao da documentacao antiga. Quando documento e codigo discordam, a divergencia esta registrada explicitamente.

## 1. Escopo e validacoes

Foram revisados:

- `src/Principal.cpp`
- `src/Compartilhado`
- `src/Dominio`
- `src/Aplicacao`
- `src/Apresentacao`
- `src/Infraestrutura`
- `tests/TestesLogica.cpp`
- `CMakeLists.txt`
- `README.md`
- documentos em `doc/`
- `assets/config.ini`
- assets principais de personagem, tiles, plantas, HUD, toolbar, fundo e sons
- configuracoes em `.vscode/`

Contagem da base no momento da auditoria:

- 50 arquivos em `src/`
- 1 arquivo em `tests/`
- 6 documentos em `doc/`
- 59 arquivos em `assets/`
- 3 arquivos em `.vscode/`

Validacoes executadas:

```powershell
rg "SDL|SDL_|IMG_|TTF_|Mix_" src/Dominio
rg "switch" src/Dominio
rg "frame|sprite|animacao|spritesheet|pisc" src/Dominio/Personagem src/Dominio/Animacao
rg "switch" src
C:\msys64\ucrt64\bin\cmake.exe -S . -B build-codex -G Ninja -DCMAKE_PREFIX_PATH=C:/msys64/ucrt64 -DCMAKE_C_COMPILER=C:/msys64/ucrt64/bin/gcc.exe -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe
C:\msys64\ucrt64\bin\cmake.exe --build build-codex
C:\msys64\ucrt64\bin\ctest.exe --test-dir build-codex --output-on-failure
C:\msys64\ucrt64\bin\cmake.exe --build build-codex --target validar-arquitetura
```

Resultados:

- `rg "SDL|SDL_|IMG_|TTF_|Mix_" src/Dominio`: sem ocorrencias.
- `rg "switch" src/Dominio`: sem ocorrencias.
- `rg "frame|sprite|animacao|spritesheet|pisc" src/Dominio/Personagem src/Dominio/Animacao`: sem ocorrencias. `src/Dominio/Animacao` existe apenas com `.gitkeep`.
- `rg "switch" src`: ocorrencias somente em `Apresentacao` e `Infraestrutura`, usadas para evento SDL, animacao visual, icones, cores e mapeamentos de assets/sons.
- CMake configurou em `build-codex` com sucesso. Houve aviso de `CMAKE_C_COMPILER` nao usado porque o projeto declara apenas linguagem C++.
- Build de `MiniFazenda2` e `MiniFazenda2Tests`: sucesso.
- CTest: `1/1` teste passou.
- Alvo `validar-arquitetura`: sucesso.

Dimensoes conferidas em PNGs principais:

- `assets/sprites/personagem/Boneco_piscando_olhos.png`: `1250 x 250`, coerente com 5 frames de `250 x 250`.
- `assets/sprites/tiles/tile_terra_seca.png`: `1774 x 887`.
- `assets/sprites/tiles/tile_terra_arada.png`: `1774 x 887`.
- `assets/sprites/tiles/tile_terra_restos.png`: `1774 x 887`.
- `assets/background/fundo_gramado.png`: `3344 x 1882`.
- sprites de mirtilo auditados: `1774 x 887`.

## 2. Visao geral do estado atual

O MiniFazenda2 esta compilavel, testavel e com a arquitetura em camadas aplicada de forma consistente na parte mais sensivel: o dominio permanece puro, sem SDL, sem renderer, sem textura, sem audio e sem logica visual de personagem.

O ponto de entrada `src/Principal.cpp` hoje faz bootstrap SDL, carrega assets/configuracoes, cria `CenaFazenda` e executa o loop principal. A antiga concentracao de input e cena no `Principal.cpp` foi reduzida: `src/Apresentacao/Cenas/CenaFazenda.hpp` agora concentra a cena principal.

O projeto atual esta funcionalmente centrado em:

- fazenda isometrica com area jogavel inicial `12 x 12`;
- nucleo inicial de `2 x 2` canteiros;
- criacao de novos canteiros pela enxada;
- arar, plantar, crescer, colher, morrer, limpar restos e remover terra;
- personagem com posicao logica pelos pes e movimento em segmentos;
- UI com toolbar, loja simples de sementes, HUD e painel de configuracoes;
- camera com zoom, pan e inercia;
- assets SDL com fallback parcial;
- testes de logica e apresentacao pura.

Nao foram encontrados achados criticos. Os principais riscos atuais sao de manutencao e evolucao: `CenaFazenda` tende a crescer, alguns contratos de UX/ferramenta ainda estao implicitos, a ordem visual ainda nao usa uma lista unica por profundidade e a apresentacao conhece estruturas concretas da infraestrutura de assets.

## 3. Mapa das funcionalidades por fluxo

### 3.1. Inicializacao

Inicio: `src/Principal.cpp::main`.

Arquivos envolvidos:

- `src/Infraestrutura/SDL/ContextoSDL.hpp`
- `src/Infraestrutura/Assets/LocalizadorDeAssets.hpp`
- `src/Infraestrutura/Configuracao/LeitorDeConfiguracao.hpp`
- `src/Infraestrutura/Assets/GerenciadorDeAtivosSDL.hpp`
- `src/Infraestrutura/Assets/RecursosDaFazenda.hpp`
- `src/Apresentacao/Cenas/CenaFazenda.hpp`
- `src/Aplicacao/Servicos/InicializadorDaFazenda.hpp`

Fluxo confirmado:

1. `InicializacaoSDL::inicializar()` inicializa SDL video/audio, SDL_image, SDL_ttf e tenta abrir SDL_mixer.
2. `Principal.cpp` cria janela e renderer.
3. O cursor do sistema e ocultado.
4. O diretorio de assets e localizado.
5. `assets/config.ini` e carregado em `ConfiguracoesDoLayout`.
6. O gerenciador de ativos SDL e criado.
7. A fabrica de plantas fornece especies para loja e catalogo visual.
8. Recursos da fazenda e HUD sao carregados.
9. A musica ambiente toca se audio inicializou.
10. `CenaFazenda` e criada e passa a receber eventos, atualizacao e renderizacao.

Responsabilidades:

- Infraestrutura inicializa SDL e carrega recursos.
- Aplicacao cria o estado inicial.
- Apresentacao conduz a cena.
- `Principal.cpp` ficou como compositor de alto nivel.

### 3.2. Carregamento de assets

Inicio: `Assets::carregarRecursosDaFazenda()` e `Assets::carregarRecursosDeHud()`.

Arquivos envolvidos:

- `src/Infraestrutura/Assets/GerenciadorDeAtivosSDL.hpp`
- `src/Infraestrutura/Assets/LocalizadorDeAssets.hpp`
- `src/Infraestrutura/Assets/RecursosDaFazenda.hpp`
- `src/Infraestrutura/Assets/CatalogoVisualDePlantas.hpp`
- `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp`

Fluxo confirmado:

- Background tenta o arquivo configurado e depois `background/fundo_gramado.png`.
- Texturas de terra sao mapeadas por `EstadoVisualDoCanteiro`.
- Sprites de planta sao resolvidos por catalogo visual externo ao dominio, indexado por `identificadorDaSemente`.
- Ancora da base da planta e calculada por varredura de pixels opacos.
- Personagem usa configuracao centralizada de frames, destino e ponto dos pes.
- Toolbar, sementes, fonte e HUD tem fallback parcial.
- Sons sao carregados sob demanda.

Estado dos fallbacks:

- Fundo: fallback verde se nenhuma textura carregar.
- Terra/planta: fallback vetorial para canteiro/planta.
- Toolbar/semente/configuracoes: fallback vetorial parcial.
- Personagem: sem fallback visual; se a textura faltar, o personagem nao e desenhado.

### 3.3. Criacao do estado inicial

Inicio: `src/Aplicacao/Servicos/InicializadorDaFazenda.hpp::criarEstadoInicialDoJogo`.

Arquivos envolvidos:

- `src/Aplicacao/Estado/EstadoDoJogo.hpp`
- `src/Dominio/Mapa/MapaDaFazenda.hpp`
- `src/Dominio/Ocupacao/GridDeOcupacao.hpp`
- `src/Dominio/Jogador/Jogador.hpp`
- `src/Dominio/Personagem/Personagem.hpp`

Fluxo confirmado:

- Cria `EstadoDoJogo`.
- Cria `MapaDaFazenda` com nucleo inicial de `2 x 2` canteiros.
- O nucleo inicial fica em `(127,127)`, `(128,127)`, `(127,128)` e `(128,128)`.
- O grid jogavel atual inicia em `12`.
- O jogador inicia com `200` moedas, `0` XP e nivel `1`.
- O personagem inicia com os pes no centro global `(128,128)`.

### 3.4. Renderizacao da fazenda

Inicio: `CenaFazenda::renderizar()`.

Arquivos envolvidos:

- `src/Apresentacao/Cenas/CenaFazenda.hpp`
- `src/Apresentacao/Renderizacao/Mundo/DesenhoDoMundo.hpp`
- `src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp`
- `src/Apresentacao/Renderizacao/Mundo/RenderizadorDoPersonagem.hpp`
- `src/Apresentacao/Renderizacao/UI/BarraDeFerramentasRenderer.hpp`
- `src/Apresentacao/Renderizacao/UI/HudRenderer.hpp`
- `src/Apresentacao/Renderizacao/Cursores/CursorCustomizado.hpp`

Fluxo confirmado:

1. Calcula a posicao realcada pelo mouse.
2. Desenha fundo.
3. Desenha canteiros existentes dentro da area jogavel.
4. Desenha personagem.
5. Desenha limite da grade jogavel.
6. Desenha preview de criacao de terra.
7. Desenha toolbar.
8. Desenha painel da loja se aberto.
9. Desenha HUD e botao de configuracoes.
10. Desenha painel de configuracoes se aberto.
11. Desenha cursor customizado.

Observacao importante:

- Canteiros sao iterados na ordem de `jogo.mapa().entidades()`, nao por profundidade visual.
- O personagem e desenhado depois da grade inteira, nao em uma lista unica ordenada por base.

### 3.5. Camera, zoom e pan

Arquivos envolvidos:

- `src/Apresentacao/Camera/CameraDoJogo.hpp`
- `src/Apresentacao/Isometria/Isometrico.hpp`
- `src/Apresentacao/Cenas/CenaFazenda.hpp`

Fluxo confirmado:

- Mouse wheel aplica zoom no ponto do cursor.
- Zoom e limitado entre `0.5x` e `2.0x`.
- Botao direito ou meio inicia pan.
- Soltar o botao correspondente finaliza pan.
- Pan tem velocidade e desaceleracao.
- HOME recentraliza camera e zoom.
- F5 recarrega configuracoes visuais e fundo.

Risco visual confirmado por leitura:

- Dimensoes de tile usam `round`, mas conversoes usam `largura / 2` e `altura / 2` inteiros.
- A inercia soma `static_cast<int>(velocidade * deltaTime)`, perdendo subpixel em velocidades baixas.

### 3.6. Interface, toolbar e loja

Arquivos envolvidos:

- `src/Apresentacao/Interface/AreaDeInteracao.hpp`
- `src/Apresentacao/Interface/EstadoDaCenaFazenda.hpp`
- `src/Apresentacao/Interface/BarraDeFerramentas/BarraDeFerramentas.hpp`
- `src/Apresentacao/Renderizacao/UI/BarraDeFerramentasRenderer.hpp`
- `src/Apresentacao/Renderizacao/UI/HudRenderer.hpp`
- `src/Apresentacao/Cenas/CenaFazenda.hpp`

Fluxo confirmado:

- Toolbar tem cursor, enxada, remover terra, semente e loja.
- Clique em botao da UI retorna antes de processar mundo.
- Clique em painel de configuracoes retorna antes de processar mundo.
- Clique em semente dentro da loja seleciona a semente, troca ferramenta para `Semente` e fecha a loja.
- Clique dentro do fundo da loja consome o clique.
- Clique fora do painel da loja, com a loja aberta, cai no fluxo normal de toolbar/mundo.

Contrato confirmado:

- Clique em UI nao move personagem.

Contrato implicito:

- Com a loja aberta, clique fora do painel pode mover o personagem e manter a loja aberta, dependendo da posicao.

### 3.7. Ferramentas

Inicio: `src/Aplicacao/Servicos/ServicoDeFerramentas.hpp::aplicarFerramentaNoJogo`.

Arquivos envolvidos:

- `src/Dominio/Ferramentas/Ferramenta.hpp`
- `src/Dominio/Ferramentas/RegistroDeFerramentas.hpp`
- `src/Dominio/Ferramentas/Enxada.hpp`
- `src/Dominio/Ferramentas/FerramentaDeSemente.hpp`
- `src/Dominio/Ferramentas/CursorDeColheita.hpp`
- `src/Dominio/Ferramentas/RemovedorDeTerra.hpp`
- `src/Dominio/Ferramentas/FerramentaDaLoja.hpp`

Fluxo confirmado:

- Ferramentas sao polimorficas.
- `RegistroDeFerramentas` guarda instancias por `TipoDeFerramenta`.
- Aplicacao monta `ContextoDaFerramenta`.
- A ferramenta selecionada aplica sua propria regra.

Contratos atuais:

- `Cursor`: colhe apenas planta madura e aplica recompensa.
- `Enxada`: cria canteiro inexistente, limpa planta morta/restos ou ara terra vazia.
- `Semente`: exige canteiro arado, semente selecionada, planta conhecida e moedas suficientes.
- `Loja`: nao altera dominio.
- `RemoverTerra`: remove qualquer canteiro existente dentro da area jogavel, independentemente do estado agricola.

### 3.8. Plantio, crescimento e colheita

Arquivos envolvidos:

- `src/Dominio/Canteiros/Canteiro.hpp`
- `src/Dominio/Canteiros/EstadoDoCanteiro.hpp`
- `src/Dominio/Plantas/Planta.hpp`
- `src/Dominio/Plantas/Especies/PlantaMirtilo.hpp`
- `src/Aplicacao/Servicos/ServicoDeTempo.hpp`
- `src/Dominio/Jogador/Jogador.hpp`

Fluxo confirmado:

```text
TerraVazia -> arar -> TerraArada
TerraArada -> plantar -> SementePlantada
SementePlantada -> PlantaCrescendo -> PlantaJovem -> PlantaMadura -> PlantaMorta
PlantaMadura -> colher -> Restos
PlantaMorta -> limparPlantaMorta -> Restos
Restos -> limparRestos -> TerraVazia
```

Pontos positivos:

- `Canteiro` centraliza transicoes.
- `Planta` e polimorfica.
- `PlantaMirtilo` nao conhece sprite, pasta ou textura.
- Recompensa e aplicada pelo jogador no fluxo de colheita.
- Lista de canteiros em crescimento evita varrer o mapa inteiro.

### 3.9. Personagem, movimento e animacoes

Arquivos envolvidos:

- `src/Dominio/Personagem/Personagem.hpp`
- `src/Apresentacao/Animacao/AnimadorDoPersonagem.hpp`
- `src/Apresentacao/Animacao/AnimacaoIdleDoPersonagem.hpp`
- `src/Apresentacao/Renderizacao/Mundo/RenderizadorDoPersonagem.hpp`
- `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp`

Fluxo confirmado:

- O dominio guarda estado logico: parado/andando, direcao, posicao decimal dos pes e caminho.
- Movimento usa caminho em segmentos, em formato de L isometrico.
- A aplicacao avanca movimento em `ServicoDeTempo`.
- A apresentacao escolhe animacao visual a partir do estado logico.
- Idle com piscadas aleatorias fica em apresentacao.
- Configuracao de sprite, recorte, destino e ponto dos pes fica em infraestrutura de assets.

Contrato preservado:

- A ancora logica do personagem continua sendo os pes.

### 3.10. HUD, audio, configuracoes e encerramento

Arquivos envolvidos:

- `src/Apresentacao/Renderizacao/UI/HudRenderer.hpp`
- `src/Apresentacao/Interface/EstadoDaCenaFazenda.hpp`
- `src/Infraestrutura/Assets/GerenciadorDeAtivosSDL.hpp`
- `src/Infraestrutura/Assets/RecursosDaFazenda.hpp`
- `src/Infraestrutura/SDL/ContextoSDL.hpp`
- `src/Principal.cpp`

Fluxo confirmado:

- HUD mostra moedas, XP e nivel.
- Botao de configuracoes abre/fecha painel.
- Painel de configuracoes controla audio mutado.
- Mute altera volume de canais e musica.
- Sons de acao sao mapeados por `AcaoDaFerramenta`.
- `SDL_QUIT` e ESC encerram a cena.
- RAII libera musica, sons, fontes, texturas, renderer, janela e subsistemas SDL.

## 4. Avaliacao por camada

### 4.1. Compartilhado

Estado:

- Puro e sem SDL.
- Contem posicoes, retangulo logico e constantes.

Ponto fragil:

- `Constantes.hpp` mistura janela, background, grade, ocupacao, zoom, pan, personagem e UI. Ainda e administravel, mas tende a virar um ponto global de alteracoes desconectadas.

### 4.2. Dominio

Estado:

- Sem SDL, SDL_image, SDL_ttf, SDL_mixer, renderer, textura ou audio.
- Sem `switch`.
- Ferramentas e plantas usam polimorfismo.
- `Canteiro` controla transicoes agricolas.
- `MapaDaFazenda` e a fonte da verdade de entidades.
- `GridDeOcupacao` e indice espacial interno.
- Personagem nao contem animacao visual.

Violacoes confirmadas:

- Nenhuma violacao direta da regra "dominio puro" foi encontrada.

Pontos de atencao:

- `EstadoDoCanteiro` e `TipoDeFerramenta` sao usados como indices de arrays em bordas externas. O dominio fornece conversores simples, mas as bordas nao validam enum invalido.

### 4.3. Aplicacao

Estado:

- Nao depende de SDL, apresentacao ou infraestrutura.
- Orquestra estado, ferramentas e tempo.
- `EstadoDoJogo` separa gameplay de estado de cena; loja, painel e audio mutado ficam em `EstadoDaCenaFazenda`.

Ponto fragil:

- `EstadoDoJogo` expoe referencias mutaveis para mapa, jogador e personagem. Isso facilita os servicos atuais, mas amplia a superficie de mutacao fora de casos de uso especificos.

### 4.4. Apresentacao

Estado:

- Contem cena, input SDL, camera, isometria, UI, renderizacao, cursor e animacao visual.
- Pode usar SDL.
- Le estado de aplicacao/dominio e chama servicos.

Pontos frageis:

- `CenaFazenda` concentra muitos fluxos da cena.
- Renderizadores conhecem tipos concretos de `Infraestrutura::Assets`.
- `AnimadorDoPersonagem` depende de `ConfigVisualDoPersonagem`, que esta em infraestrutura.
- A ordem de desenho ainda nao e uma lista unica por profundidade.

### 4.5. Infraestrutura

Estado:

- Inicializa SDL e gerencia recursos externos.
- Carrega textura, fonte, som, musica e config.
- Usa cache de assets.
- Resolve catalogo visual de plantas e personagem.

Pontos frageis:

- `LeitorDeConfiguracao.hpp` depende de `Apresentacao::Camera` para centralizar grade durante leitura.
- `LocalizadorDeAssets.hpp` e `RecursosDaFazenda.hpp` dependem de tipos de apresentacao.
- `RecursosDaFazenda.hpp` depende de dominio para mapear especies, estados, ferramentas e sons. Isso e aceitavel como borda de infraestrutura, mas deve permanecer restrito a mapeamento.

## 5. Achados classificados por severidade

### Critico

Nenhum achado critico confirmado.

### Alto

Nenhum achado alto confirmado como erro atual. Existem riscos medios que podem virar altos quando o jogo ganhar mais entidades, salvamento, pathfinding ou economia persistente.

### Medio

#### M1. `CenaFazenda` concentra muitos fluxos de runtime

Arquivo/fluxo:

- `src/Apresentacao/Cenas/CenaFazenda.hpp`

Problema:

- A cena processa eventos SDL, pan/zoom, teclas, loja, painel de configuracoes, audio, clique no mundo, aplicacao de ferramentas, movimento do personagem, atualizacao e renderizacao.

Por que importa:

- A ordem dos retornos em `processarCliqueEsquerdo()` define contratos importantes, como "UI nao move personagem".
- Novas telas, modais, ferramentas e interacoes podem aumentar a chance de regressao.

Impacto provavel:

- Crescimento do arquivo e dificuldade para testar input sem janela SDL.

Recomendacao:

- Extrair em tarefas futuras controladores pequenos de input da cena, painel de configuracoes e loja, mantendo `CenaFazenda` como orquestradora.

#### M2. `RemovedorDeTerra` remove qualquer canteiro sem considerar estado agricola

Arquivo/fluxo:

- `src/Dominio/Ferramentas/RemovedorDeTerra.hpp:25`
- `src/Dominio/Canteiros/Canteiro.hpp`

Problema:

- A ferramenta remove o canteiro se ele existir, sem checar se esta vazio, arado, plantado, maduro, morto ou com restos.

Por que importa:

- Se isso nao for uma decisao de design, o jogador pode perder uma planta viva ou madura sem confirmacao, custo ou regra explicita.

Impacto provavel:

- Perda silenciosa de progresso e testes insuficientes para contrato destrutivo.

Recomendacao:

- Decidir o contrato: remover tudo intencionalmente, remover apenas terra vazia/arada, ou exigir estado seguro/confirmacao. Depois documentar e testar.

#### M3. Loja aberta permite clique fora cair no mundo

Arquivo/fluxo:

- `src/Apresentacao/Cenas/CenaFazenda.hpp:253`
- `src/Apresentacao/Cenas/CenaFazenda.hpp:278`
- `src/Apresentacao/Cenas/CenaFazenda.hpp:309`

Problema:

- Se a loja esta aberta e o clique nao acerta uma semente nem o fundo do painel, o fluxo continua para toolbar/mundo.

Por que importa:

- Hoje isso pode mover o personagem enquanto a ferramenta selecionada e `Loja`, que nao aplica acao. Pode ser aceitavel, mas e um contrato implicito e sem teste especifico.

Impacto provavel:

- UX inconsistente quando a loja tiver mais opcoes, precos, detalhes ou bloqueio de mundo.

Recomendacao:

- Definir se clique fora fecha a loja, bloqueia o mundo ou permite interagir com o mundo. Registrar e cobrir com teste de fluxo de input.

#### M4. Apresentacao conhece estruturas concretas da infraestrutura de assets

Arquivo/fluxo:

- `src/Apresentacao/Cenas/CenaFazenda.hpp`
- `src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp`
- `src/Apresentacao/Renderizacao/Mundo/RenderizadorDoPersonagem.hpp`
- `src/Apresentacao/Renderizacao/UI/BarraDeFerramentasRenderer.hpp`
- `src/Apresentacao/Animacao/AnimadorDoPersonagem.hpp`

Problema:

- Renderizadores e animacao visual dependem de tipos como `RecursosDaFazenda`, `SpriteDaPlanta`, `TexturasDoPersonagem` e `ConfigVisualDoPersonagem`.

Por que importa:

- A camada de apresentacao fica acoplada ao formato interno do carregamento de assets.

Impacto provavel:

- Trocar backend de assets, carregar pacotes, skins ou mocks de renderizacao fica mais caro.

Recomendacao:

- Em uma evolucao futura, criar DTOs de apresentacao para recursos renderizaveis ou uma interface estreita de consulta visual.

#### M5. Infraestrutura depende de apresentacao para carregar configuracao

Arquivo/fluxo:

- `src/Infraestrutura/Configuracao/LeitorDeConfiguracao.hpp`
- `src/Infraestrutura/Assets/LocalizadorDeAssets.hpp`
- `src/Infraestrutura/Assets/RecursosDaFazenda.hpp`

Problema:

- O leitor de configuracao chama `Apresentacao::Camera::aplicarOrigemCentradaDaGrade()` durante a leitura.

Por que importa:

- Carregar arquivo e decidir layout/camera sao responsabilidades diferentes. A dependencia e permitida pela documentacao atual, mas torna o fluxo menos previsivel.

Impacto provavel:

- Dificuldade para testar config isoladamente e risco de precedencia confusa entre valores do `config.ini` e centralizacao automatica.

Recomendacao:

- Separar leitura crua do arquivo de uma etapa de normalizacao/aplicacao de layout feita pela apresentacao ou pela cena.

#### M6. Ordem de desenho ainda nao usa profundidade unica

Arquivo/fluxo:

- `src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp:148`
- `src/Apresentacao/Cenas/CenaFazenda.hpp:115`
- `src/Apresentacao/Cenas/CenaFazenda.hpp:123`

Problema:

- Canteiros sao desenhados na ordem de entidades do mapa; personagem e desenhado depois da grade inteira.

Por que importa:

- Em isometria, sobreposicao correta deve considerar a base/profundidade. O dominio ja calcula `profundidadeDaBase()`, mas a apresentacao ainda nao monta uma lista unica de renderizaveis.

Impacto provavel:

- Arvores, casas, animais, decoracoes e personagem podem ficar na frente/atras em ordem errada quando forem adicionados.

Recomendacao:

- Criar uma fila de renderizaveis na apresentacao, com base logica e profundidade, incluindo canteiro, planta, personagem e futuros objetos.

#### M7. Configuracao de origem da grade tem precedencia confusa

Arquivo/fluxo:

- `assets/config.ini:2`
- `src/Infraestrutura/Configuracao/LeitorDeConfiguracao.hpp:93`
- `src/Infraestrutura/Configuracao/LeitorDeConfiguracao.hpp:95`

Problema:

- `deslocamentoGradeHorizontal` e `deslocamentoGradeVertical` sao lidos, mas logo depois a origem e sobrescrita por `aplicarOrigemCentradaDaGrade()`. Somente `origemGradeHorizontal` e `origemGradeVertical`, se existirem, vencem depois disso.

Por que importa:

- O `config.ini` parece aceitar um ajuste que, na pratica atual, nao tem efeito final.

Impacto provavel:

- Ajustes visuais feitos no arquivo podem parecer quebrados.

Recomendacao:

- Remover chaves obsoletas, renomear para as chaves efetivas ou documentar explicitamente que a centralizacao vence.

#### M8. Arredondamento inteiro em zoom/pan pode gerar tremor ou perda de movimento

Arquivo/fluxo:

- `src/Apresentacao/Camera/CameraDoJogo.hpp:44`
- `src/Apresentacao/Camera/CameraDoJogo.hpp:165`
- `src/Apresentacao/Camera/CameraDoJogo.hpp:222`
- `src/Apresentacao/Isometria/Isometrico.hpp:23`
- `src/Apresentacao/Isometria/Isometrico.hpp:38`

Problema:

- Zoom usa dimensoes arredondadas e conversoes inteiras. Inercia de pan converte deslocamento por frame para `int`.

Por que importa:

- Em velocidades baixas, movimento subpixel pode ser descartado. Em alguns zooms, metades impares podem ficar truncadas.

Impacto provavel:

- Sensacao de pan travado/tremido ou pequenas assimetrias visuais.

Recomendacao:

- Se o problema aparecer visualmente, manter offset acumulado em `float` e converter para inteiro apenas no desenho.

#### M9. Falhas de asset sao cacheadas como `nullptr`

Arquivo/fluxo:

- `src/Infraestrutura/Assets/GerenciadorDeAtivosSDL.hpp:38`
- `src/Infraestrutura/Assets/GerenciadorDeAtivosSDL.hpp:61`
- `src/Infraestrutura/Assets/GerenciadorDeAtivosSDL.hpp:79`
- `src/Infraestrutura/Assets/GerenciadorDeAtivosSDL.hpp:97`

Problema:

- Quando textura, fonte, som ou musica falham, a chave e salva no cache com `nullptr`.

Por que importa:

- Durante hot reload visual, se o arquivo aparecer ou for corrigido depois da primeira falha, a mesma execucao nao tenta recarregar aquele caminho.

Impacto provavel:

- F5 pode nao recuperar assets corrigidos sem reiniciar o jogo.

Recomendacao:

- Nao cachear falhas permanentes, ou fornecer uma funcao de invalidacao/reload para desenvolvimento.

### Baixo

#### B1. Background configurado nao existe como arquivo principal

Arquivo/fluxo:

- `assets/config.ini:8`
- `src/Apresentacao/ConfiguracoesDoLayout.hpp:15`
- `src/Infraestrutura/Assets/LocalizadorDeAssets.hpp:74`

Problema:

- `arquivoBackgroundPrincipal = background.png`, que vira `assets/background/background.png`, mas o asset real usado e o fallback `assets/background/fundo_gramado.png`.

Por que importa:

- A configuracao ativa nao descreve o asset efetivamente usado.

Impacto provavel:

- Confusao ao trocar o fundo.

Recomendacao:

- Atualizar o `config.ini` para `fundo_gramado.png` ou tornar o log de fallback mais explicito.

#### B2. Personagem nao tem fallback visual

Arquivo/fluxo:

- `src/Apresentacao/Renderizacao/Mundo/RenderizadorDoPersonagem.hpp:120`

Problema:

- Se a textura do personagem faltar, `desenharPersonagem()` apenas retorna.

Por que importa:

- Outros elementos principais tem fallback visual; o personagem pode sumir sem representacao substituta.

Impacto provavel:

- Dificuldade de diagnostico visual em ambiente sem asset.

Recomendacao:

- Adicionar fallback simples de debug ou log mais destacado ao carregar textura ausente.

#### B3. Arrays indexados por enum nao validam valores invalidos

Arquivo/fluxo:

- `src/Dominio/Ferramentas/RegistroDeFerramentas.hpp:24`
- `src/Dominio/Ferramentas/TipoDeFerramenta.hpp:17`
- `src/Dominio/Canteiros/EstadoDoCanteiro.hpp:20`
- `src/Infraestrutura/Assets/RecursosDaFazenda.hpp:49`

Problema:

- Conversores fazem `static_cast<std::size_t>` e os arrays usam o indice diretamente.

Por que importa:

- Hoje os valores sao controlados, mas casts futuros ou dados carregados de arquivo podem gerar indice invalido.

Impacto provavel:

- Acesso fora de faixa se enum invalido entrar em bordas.

Recomendacao:

- Manter enums internos controlados ou criar validadores antes de usar valores vindos de fora.

#### B4. Acumulador de tempo avanca crescimento segundo a segundo

Arquivo/fluxo:

- `src/Aplicacao/Servicos/ServicoDeTempo.hpp:63`

Problema:

- `avancarTempoDoJogo()` consome o acumulador em passos de 1 segundo.

Por que importa:

- Funciona no jogo atual e passou nos testes, mas deltas grandes ou muitos canteiros podem custar caro.

Impacto provavel:

- Performance ruim se houver fast-forward, pausa longa, offline progress ou muitos cultivos.

Recomendacao:

- Manter por simplicidade agora; revisar quando houver simulacao acelerada ou persistencia de tempo.

#### B5. `Constantes.hpp` concentra dominios diferentes

Arquivo/fluxo:

- `src/Compartilhado/Constantes.hpp`

Problema:

- O arquivo mistura constantes de janela, fundo, grade, ocupacao, camera, personagem e UI.

Por que importa:

- Mudancas de areas diferentes passam pelo mesmo arquivo global.

Impacto provavel:

- Conflitos e dificuldade de governanca se o projeto crescer.

Recomendacao:

- Separar gradualmente por assunto quando a quantidade de constantes aumentar.

### Observacoes

#### O1. `AcelerarCrescimento` existe sem fluxo de UI/ferramenta

Arquivos:

- `src/Dominio/Ferramentas/ResultadoDaFerramenta.hpp`
- `src/Dominio/Canteiros/Canteiro.hpp`
- `src/Infraestrutura/Assets/RecursosDaFazenda.hpp`

`AcaoDaFerramenta::AcelerarCrescimento`, `Canteiro::acelerarParaMadura()` e som associado existem, mas nao ha ferramenta ou UI chamando esse fluxo.

#### O2. Assets existentes nao significam funcionalidades implementadas

Ha sprites e sons de animais, venda, agua, pesticida, missoes e outros eventos que nao aparecem como feature ativa no codigo atual. Isso deve ser tratado como backlog de asset.

#### O3. `.vscode/tasks.json` e util, mas usa comandos diretos

As tasks chamam `ninja.exe` diretamente e uma task limpa `build` via `cmd.exe`. Isso e aceitavel como ferramenta local, mas a fonte de verdade de build continua sendo CMake/CTest.

## 6. Divergencias entre documentacao e codigo

1. `doc/auditoria_tasks.md` anterior estava desatualizado e foi substituido por esta segunda auditoria.

2. A auditoria anterior dizia que `Principal.cpp` concentrava a cena e que `CenaFazenda` nao existia. Codigo atual tem `src/Apresentacao/Cenas/CenaFazenda.hpp`; `Principal.cpp` agora faz bootstrap e loop de alto nivel.

3. A auditoria anterior citava `Dominio/Grade/GradeGlobalDeCanteiros.hpp` e `TileDeTerra`. Codigo atual usa `Dominio/Mapa/MapaDaFazenda.hpp` e `Dominio/Ocupacao/GridDeOcupacao.hpp`.

4. `doc/arquitetura.md` esta majoritariamente alinhado ao codigo atual: dominio puro, personagem sem animacao visual no dominio, `EstadoDaCenaFazenda` separado e `CenaFazenda` como cena de borda.

5. `doc/Tamanhos.md` registra `background.png` como arquivo configurado e `fundo_gramado.png` como fallback. Isso bate com o codigo, mas o arquivo configurado nao existe nos assets atuais; logo, o fundo real e o fallback.

6. `assets/config.ini` usa `deslocamentoGradeHorizontal/Vertical`, mas o leitor sobrescreve esses valores com centralizacao automatica antes do uso final, salvo se `origemGradeHorizontal/Vertical` tambem forem informados.

7. `README.md` esta coerente com o CMake atual e os comandos UCRT64. O unico detalhe observado foi o aviso de `CMAKE_C_COMPILER` nao usado porque o projeto declara apenas C++.

## 7. Pontos fortes

- Dominio sem SDL, renderer, textura e audio.
- Dominio sem `switch`.
- Ferramentas polimorficas.
- Plantas polimorficas e sem metadados visuais no dominio.
- `Canteiro` centraliza transicoes agricolas.
- `MapaDaFazenda` e `GridDeOcupacao` separam entidade real e indice espacial.
- Personagem mantem ancora logica pelos pes.
- Animacao visual do personagem esta fora do dominio.
- Estado de cena/UI esta separado de `EstadoDoJogo`.
- `Principal.cpp` esta mais enxuto que a auditoria anterior descrevia.
- RAII de SDL/assets esta simples e efetivo.
- Build, testes e validacao arquitetural passam.
- Testes cobrem plantio, crescimento, colheita, restos, morte, remocao, camera, hit-test, loja, personagem e animacao idle.
- Assets principais conferidos batem com os contratos de recorte/escala mais importantes.

## 8. Pontos frageis

- `CenaFazenda` tende a acumular muitas responsabilidades de cena.
- Contrato de `RemovedorDeTerra` e destrutivo e ainda pouco explicito.
- Contrato da loja aberta ao clicar fora do painel nao esta documentado/testado.
- Renderizacao ainda nao ordena tudo por profundidade.
- Apresentacao depende de tipos concretos de infraestrutura de assets.
- Infraestrutura aplica centralizacao de camera durante leitura de configuracao.
- Fallback de personagem e mais fraco que fallback de canteiros/plantas.
- Hot reload visual pode nao se recuperar de falha de asset cacheada como `nullptr`.
- Arredondamentos inteiros de camera/isometria podem gerar pequenos artefatos.
- `Constantes.hpp` mistura assuntos de varias partes do jogo.

## 9. Riscos futuros

1. Novas entidades altas podem quebrar sobreposicao visual sem fila de profundidade.
2. Novas ferramentas podem aumentar ambiguidade de clique se o roteamento de input continuar concentrado.
3. Loja mais complexa pode ter comportamento confuso se clique fora continuar caindo no mundo.
4. Salvamento ou replay pode sofrer com mutacoes diretas demais em `EstadoDoJogo`.
5. Skins, pacotes de asset ou mocks visuais serao mais dificeis com apresentacao acoplada a `Infraestrutura::Assets`.
6. Hot reload de asset pode exigir reinicio se uma falha ja foi cacheada.
7. Tempo acelerado/offline pode tornar o crescimento segundo a segundo caro.
8. Novos valores de enum vindos de configuracao podem expor acessos fora de faixa.

## 10. Recomendacoes priorizadas

### Prioridade 1

1. Definir e documentar o contrato do `RemovedorDeTerra`.
2. Definir o comportamento da loja aberta quando o jogador clica fora do painel.
3. Adicionar testes focados nesses dois contratos de input/ferramenta, preferencialmente sem abrir janela SDL.

### Prioridade 2

4. Extrair partes de `CenaFazenda`: roteamento de input, painel de configuracoes e loja.
5. Criar fila de renderizaveis por profundidade, incluindo personagem.
6. Separar leitura de configuracao da aplicacao de centralizacao/layout.
7. Corrigir `config.ini` para apontar para o fundo real ou explicitar o fallback.

### Prioridade 3

8. Criar DTO/interface estreita de recursos visuais para reduzir acoplamento entre apresentacao e infraestrutura.
9. Revisar cache de falhas de assets para melhorar hot reload.
10. Adicionar fallback visual simples para personagem ausente.
11. Revisar camera/isometria com acumuladores subpixel se houver tremor visual real.
12. Separar constantes por assunto quando o arquivo voltar a crescer.

## 11. Proximas tasks sugeridas

Ordem sugerida:

1. Task de contrato de gameplay: `RemovedorDeTerra` e loja aberta.
2. Task de testes de input puro: cobrir toolbar, loja, painel, mundo e movimento.
3. Task de organizacao de cena: quebrar `CenaFazenda` em controladores menores.
4. Task visual: fila unica de profundidade para mundo/personagem/objetos futuros.
5. Task de configuracao/assets: ajustar background, precedencia de config e cache de falhas.
6. Task de desacoplamento: DTOs de recursos visuais para renderizadores.
7. Task de polimento de camera: revisar arredondamentos apenas se houver sintoma visual.

## 12. Limitacoes da auditoria

- O jogo nao foi executado interativamente em janela.
- Nao houve captura de screenshot nem validacao por pixel.
- Audio nao foi validado em dispositivo real.
- Nao foram criados testes novos, conforme foco solicitado para diagnostico/documentacao.
- Nao foram usados sanitizers, analisadores estaticos externos ou profiling.
- A avaliacao visual foi feita por leitura de codigo, dimensoes dos PNGs principais e comparacao com os contratos documentados.

Tasks:


1. Definir contrato do RemovedorDeTerra

corrigir:
Hoje o removedor apaga qualquer canteiro existente, inclusive plantado, maduro, morto ou com restos. Definir uma regra oficial para a ferramenta.

Regra recomendada:
- Remover apenas canteiro vazio, arado ou com restos.
- Bloquear remoção de planta viva, planta crescendo e planta madura.
- Para planta morta, decidir se a enxada limpa ou se o removedor também pode remover.

onde:
src/Dominio/Ferramentas/RemovedorDeTerra.hpp
src/Dominio/Canteiros/Canteiro.hpp
src/Dominio/Mapa/MapaDaFazenda.hpp

prioridade:
Alta


2. Testar contrato do RemovedorDeTerra

corrigir:
Criar testes cobrindo todos os estados relevantes do canteiro para travar o comportamento definido na task anterior.

Estados que devem ser testados:
- terra vazia
- terra arada
- semente plantada
- planta crescendo
- planta madura
- planta morta
- restos

onde:
tests/TestesLogica.cpp
src/Dominio/Ferramentas/RemovedorDeTerra.hpp

prioridade:
Alta


3. Definir comportamento da loja aberta ao clicar fora do painel

corrigir:
Hoje, com a loja aberta, um clique fora do painel pode cair no mundo. Definir o contrato oficial.

Regra recomendada:
- Clique fora da loja fecha a loja.
- Esse mesmo clique não deve mover o personagem.
- Esse mesmo clique não deve aplicar ferramenta no mundo.

onde:
src/Apresentacao/Cenas/CenaFazenda.hpp
src/Apresentacao/Interface/EstadoDaCenaFazenda.hpp
src/Apresentacao/Interface/BarraDeFerramentas/BarraDeFerramentas.hpp

prioridade:
Alta


4. Testar fluxo de clique da loja, UI e mundo

corrigir:
Criar testes puros para garantir que o input respeita a ordem correta de consumo.

Casos que devem ser testados:
- Clique em UI não move personagem.
- Clique em botão de ferramenta consome evento.
- Clique em painel aberto consome evento.
- Clique em semente seleciona a ferramenta correta.
- Clique fora da loja segue o contrato definido.
- Clique no mundo só acontece quando nenhuma camada de UI consumiu o evento.

onde:
tests/TestesLogica.cpp
src/Apresentacao/Interface/
src/Apresentacao/Cenas/CenaFazenda.hpp

prioridade:
Alta


5. Extrair roteamento de input da CenaFazenda

corrigir:
CenaFazenda ainda concentra evento SDL, teclado, mouse, pan, zoom, loja, painel, ferramentas e clique no mundo. Criar um componente específico para rotear input.

Objetivo:
A cena deve perguntar quem consome o evento e depois executar apenas a ação resultante.

onde:
src/Apresentacao/Cenas/CenaFazenda.hpp

novo possível:
src/Apresentacao/Cenas/InputDaCenaFazenda.hpp
src/Apresentacao/Cenas/RoteadorDeInputDaFazenda.hpp

prioridade:
Média


6. Extrair controlador da loja

corrigir:
Separar regra de abrir loja, fechar loja, selecionar semente, consumir clique no painel e tratar clique fora.

Objetivo:
CenaFazenda não deve conter detalhes internos do fluxo da loja.

onde:
src/Apresentacao/Cenas/CenaFazenda.hpp
src/Apresentacao/Interface/EstadoDaCenaFazenda.hpp
src/Apresentacao/Interface/BarraDeFerramentas/BarraDeFerramentas.hpp

novo possível:
src/Apresentacao/Interface/Loja/ControladorDaLoja.hpp

prioridade:
Média


7. Extrair controlador do painel de configurações

corrigir:
Separar abertura, fechamento, botão de som, mute/unmute e consumo de eventos do painel de configurações.

Objetivo:
CenaFazenda não deve saber os detalhes internos do painel.

onde:
src/Apresentacao/Cenas/CenaFazenda.hpp
src/Apresentacao/Interface/EstadoDaCenaFazenda.hpp
src/Apresentacao/Renderizacao/UI/HudRenderer.hpp

novo possível:
src/Apresentacao/Interface/Configuracoes/ControladorDoPainelDeConfiguracoes.hpp

prioridade:
Média


8. Criar fila única de renderização por profundidade

corrigir:
Hoje os canteiros são desenhados pela ordem das entidades e o personagem é desenhado depois da grade inteira. Isso pode quebrar sobreposição quando existirem árvores, casas, animais e decorações.

Objetivo:
Criar uma lista de renderizáveis ordenada por profundidade/base isométrica.

A fila deve incluir:
- canteiros
- plantas
- personagem
- objetos altos futuros
- animais futuros
- construções futuras

onde:
src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp
src/Apresentacao/Renderizacao/Mundo/RenderizadorDoPersonagem.hpp
src/Apresentacao/Cenas/CenaFazenda.hpp
src/Dominio/Mapa/MapaDaFazenda.hpp

prioridade:
Média


9. Preparar renderização para entidades altas futuras

corrigir:
Definir contrato visual para profundidade de árvores, casas, animais, decoração e personagem.

Regra:
A base no chão determina a ordem de desenho, não o topo do sprite e nem a ordem de criação da entidade.

onde:
src/Apresentacao/Renderizacao/Mundo/
src/Dominio/Mapa/
doc/

prioridade:
Média


10. Separar leitura de config.ini da aplicação de layout/câmera

corrigir:
LeitorDeConfiguracao lê o arquivo e também aplica centralização da grade. Separar em duas responsabilidades.

Nova divisão recomendada:
- Infraestrutura lê os dados crus do config.ini.
- Apresentação normaliza e aplica esses dados no layout/câmera.

onde:
src/Infraestrutura/Configuracao/LeitorDeConfiguracao.hpp
src/Apresentacao/Camera/CameraDoJogo.hpp
src/Apresentacao/Cenas/CenaFazenda.hpp
src/Apresentacao/ConfiguracoesDoLayout.hpp

prioridade:
Média


11. Corrigir precedência da origem da grade no config.ini

corrigir:
deslocamentoGradeHorizontal e deslocamentoGradeVertical são lidos, mas a centralização automática sobrescreve o efeito.

Escolher uma solução:
- Remover as chaves mortas.
- Renomear para chaves realmente efetivas.
- Documentar claramente que a centralização automática vence.

onde:
assets/config.ini
src/Infraestrutura/Configuracao/LeitorDeConfiguracao.hpp
src/Apresentacao/ConfiguracoesDoLayout.hpp
doc/Tamanhos.md

prioridade:
Média


12. Corrigir background configurado inexistente

corrigir:
O config aponta para background.png, mas o jogo usa fallback background/fundo_gramado.png.

Solução recomendada:
Atualizar o config para apontar diretamente para o fundo real.

Alternativa:
Melhorar o log para deixar claro quando o jogo caiu em fallback.

onde:
assets/config.ini
assets/background/
src/Infraestrutura/Assets/LocalizadorDeAssets.hpp
src/Apresentacao/ConfiguracoesDoLayout.hpp

prioridade:
Baixa


13. Revisar cache de falha de assets

corrigir:
Quando textura, fonte, som ou música falham, o cache guarda nullptr. Isso atrapalha hot reload, porque se o arquivo for corrigido durante a execução, F5 pode não recuperar.

Soluções possíveis:
- Não cachear falhas.
- Criar invalidação explícita no reload.
- Criar método para limpar cache de assets inválidos.

onde:
src/Infraestrutura/Assets/GerenciadorDeAtivosSDL.hpp
src/Infraestrutura/Assets/RecursosDaFazenda.hpp
src/Apresentacao/Cenas/CenaFazenda.hpp

prioridade:
Média


14. Adicionar fallback visual para personagem ausente

corrigir:
Se a textura do personagem faltar, ele simplesmente não é desenhado.

Solução recomendada:
Criar fallback simples de debug no ponto dos pés, como um marcador, círculo, losango ou retângulo pequeno.

onde:
src/Apresentacao/Renderizacao/Mundo/RenderizadorDoPersonagem.hpp
src/Infraestrutura/Assets/RecursosDaFazenda.hpp
src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp

prioridade:
Baixa


15. Reduzir acoplamento entre apresentação e infraestrutura de assets

corrigir:
Renderizadores conhecem tipos concretos de Infraestrutura::Assets. Isso mistura apresentação com detalhes de carregamento.

Objetivo:
Criar DTOs de apresentação ou interfaces estreitas para recursos renderizáveis.

A infraestrutura deve carregar os assets e entregar dados prontos, mas a apresentação não deve depender demais dos tipos internos dela.

onde:
src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp
src/Apresentacao/Renderizacao/Mundo/RenderizadorDoPersonagem.hpp
src/Apresentacao/Renderizacao/UI/BarraDeFerramentasRenderer.hpp
src/Apresentacao/Animacao/AnimadorDoPersonagem.hpp
src/Infraestrutura/Assets/RecursosDaFazenda.hpp
src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp

prioridade:
Média


16. Validar enums antes de usar como índice

corrigir:
Alguns arrays usam enum convertido com static_cast<std::size_t>. Hoje é controlado, mas dados externos futuros podem quebrar.

Solução recomendada:
Criar helpers seguros para validar enum antes de usar como índice.

onde:
src/Dominio/Ferramentas/RegistroDeFerramentas.hpp
src/Dominio/Ferramentas/TipoDeFerramenta.hpp
src/Dominio/Canteiros/EstadoDoCanteiro.hpp
src/Infraestrutura/Assets/RecursosDaFazenda.hpp

prioridade:
Baixa


17. Revisar arredondamento de zoom e pan somente se houver sintoma visual

corrigir:
Zoom usa dimensões arredondadas e pan descarta subpixel ao converter deslocamento para int. Isso pode gerar tremor, sensação travada ou assimetria em zooms quebrados.

Regra:
Não mexer sem sintoma visual claro.

Solução futura:
Manter acumuladores em float e converter para inteiro só no desenho.

onde:
src/Apresentacao/Camera/CameraDoJogo.hpp
src/Apresentacao/Isometria/Isometrico.hpp
src/Apresentacao/Renderizacao/Primitivas/PrimitivasSDL.hpp

prioridade:
Baixa


18. Revisar avanço de crescimento segundo a segundo

corrigir:
O tempo agrícola consome o acumulador em passos de 1 segundo. Está bom agora, mas pode ficar caro com fast-forward, progresso offline, pausa longa ou muitos canteiros.

Solução futura:
Permitir avanço de tempo acumulado em bloco.

onde:
src/Aplicacao/Servicos/ServicoDeTempo.hpp
src/Dominio/Canteiros/Canteiro.hpp
src/Dominio/Mapa/MapaDaFazenda.hpp

prioridade:
Baixa


19. Separar Constantes.hpp por assunto

corrigir:
Constantes.hpp mistura janela, background, grade, ocupação, câmera, personagem e UI.

Solução recomendada:
Separar gradualmente por responsabilidade.

Possíveis divisões:
- ConstantesDaJanela.hpp
- ConstantesDaGrade.hpp
- ConstantesDaCamera.hpp
- ConstantesDaInterface.hpp
- ConstantesDoPersonagem.hpp

onde:
src/Compartilhado/Constantes.hpp

novo possível:
src/Compartilhado/Constantes/

prioridade:
Baixa


20. Decidir destino do fluxo AcelerarCrescimento

corrigir:
Existe ação, método no canteiro e som associado, mas não existe ferramenta ou UI chamando isso.

Decidir se:
- vira feature real
- vira ferramenta de debug/dev
- vira bônus futuro
- ou deve ser removido por enquanto

onde:
src/Dominio/Ferramentas/ResultadoDaFerramenta.hpp
src/Dominio/Canteiros/Canteiro.hpp
src/Infraestrutura/Assets/RecursosDaFazenda.hpp

prioridade:
Baixa


21. Catalogar assets existentes ainda não usados

corrigir:
Existem sprites e sons de animais, venda, água, pesticida, missões e outros eventos que ainda não são funcionalidades.

Objetivo:
Criar lista de backlog de assets para não confundir asset parado com bug.

onde:
assets/
doc/

novo possível:
doc/backlog_de_assets.md

prioridade:
Baixa


22. Reduzir mutação direta em EstadoDoJogo no futuro

corrigir:
EstadoDoJogo expõe referências mutáveis para mapa, jogador e personagem. Hoje isso ajuda os serviços, mas pode atrapalhar salvamento, replay, undo ou validação de regras.

Solução futura:
Concentrar mutações em casos de uso mais específicos.

onde:
src/Aplicacao/Estado/EstadoDoJogo.hpp
src/Aplicacao/Servicos/

prioridade:
Baixa