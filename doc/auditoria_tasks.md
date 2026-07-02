# Auditoria tecnica do MiniFazenda2

Data da auditoria: 2026-07-01

Em caso de nova auditoria, registra data e trocar a zona de exlusão pela nova auditoria.

-------------------------ZONA-DE-EXCLUSÃO-------------------------

## 1. Visao geral do estado atual

O MiniFazenda2 esta em um estado funcional e compilavel, com uma separacao de camadas ja bem aplicada na maior parte do codigo. O projeto tem um unico ponto de entrada em `src/Principal.cpp`, usa C++17 com SDL2, SDL2_image, SDL2_ttf e SDL2_mixer, e organiza a logica nas pastas `Compartilhado`, `Dominio`, `Aplicacao`, `Apresentacao` e `Infraestrutura`.

Foram analisados:

- 45 arquivos em `src/`.
- 1 arquivo de teste em `tests/`.
- 3 documentos existentes em `doc/`.
- `README.md`, `CMakeLists.txt`, `.vscode`, `.gitignore`, `assets/config.ini`, assets principais e `remover_fundo.py`.

Validacoes executadas:

- `rg "SDL|SDL_|IMG_|TTF_|Mix_" src/Dominio`: sem ocorrencias.
- `rg "switch" src/Dominio`: sem ocorrencias.
- Configuracao CMake em `build-codex`: sucesso.
- Build de `MiniFazenda2` e `MiniFazenda2Tests`: sucesso.
- `ctest --test-dir build-codex --output-on-failure`: 1/1 teste passou.
- `cmake --build build-codex --target validar-arquitetura`: sucesso.
- Leitura de dimensoes dos PNGs principais: personagem `1250x250`, tiles/plantas `1774x887`, background fallback `3344x1882`.

Nao foram encontrados problemas criticos de compilacao, inicializacao basica do dominio ou quebra direta da regra "Dominio sem SDL". O principal risco tecnico atual e arquitetural: o dominio ainda carrega conceitos visuais e de assets em alguns pontos, mesmo sem depender fisicamente de SDL.

## 2. Mapa das funcionalidades por fluxo

### 2.1. Inicializacao

Inicio: `src/Principal.cpp::main`.

Arquivos envolvidos:

- `Infraestrutura/SDL/ContextoSDL.hpp`
- `Compartilhado/Constantes.hpp`
- `Infraestrutura/Assets/LocalizadorDeAssets.hpp`
- `Infraestrutura/Configuracao/LeitorDeConfiguracao.hpp`
- `Aplicacao/Servicos/InicializadorDaFazenda.hpp`

Fluxo atual:

1. `InicializacaoSDL::inicializar()` chama `SDL_Init`, `IMG_Init`, `TTF_Init` e tenta `Mix_OpenAudio`.
2. `Principal.cpp` cria janela e renderer.
3. Cursor do sistema e ocultado por `CursorOculto`.
4. Diretorio de assets e localizado.
5. `assets/config.ini` e carregado em `ConfiguracoesDoLayout`.
6. Estado inicial do jogo e criado.
7. Recursos visuais, fonte, icone de HUD e musica sao carregados.

Responsabilidades:

- Infraestrutura inicializa bibliotecas externas e localiza arquivos.
- Aplicacao cria o agregado inicial do jogo.
- `Principal.cpp` coordena tudo.

Acoplamento observado:

- `Principal.cpp` ainda concentra toda a inicializacao e todo o loop.
- Nao existe `CenaFazenda`, embora a documentacao cite essa extracao como etapa futura.

### 2.2. Carregamento de assets

Inicio: `Principal.cpp`, ao chamar `Assets::carregarRecursosDaFazenda()` e `Assets::carregarRecursosDeHud()`.

Arquivos envolvidos:

- `Infraestrutura/Assets/GerenciadorDeAtivosSDL.hpp`
- `Infraestrutura/Assets/LocalizadorDeAssets.hpp`
- `Infraestrutura/Assets/RecursosDaFazenda.hpp`
- `Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp`
- `Dominio/Plantas/FabricaDePlantas.hpp`
- `Dominio/Plantas/Planta.hpp`

Fluxo atual:

- O background vem de `arquivoBackgroundPrincipal`; se nao existir, tenta `background/fundo_gramado.png`.
- Texturas de terra sao carregadas por estado.
- Sprites de plantas sao carregados pelo catalogo visual de `Infraestrutura/Assets`, indexado por `identificadorDaSemente`.
- Ancora de planta e calculada por varredura de pixels opacos.
- Texturas do personagem sao carregadas por configuracao visual centralizada.
- Icones de toolbar, sementes, fonte e icone de configuracoes tem fallback parcial.

Acoplamento observado:

- `Dominio::Plantas::Planta` fornece `identificadorDaSemente`; a resolucao de pasta e arquivos visuais fica no catalogo de assets.
- `Infraestrutura::Assets::RecursosDaFazenda` depende de `Dominio::Plantas::FabricaDePlantas` para descobrir especies carregaveis.
- `Apresentacao::Renderizacao` recebe estruturas concretas de `Infraestrutura::Assets`, entao a apresentacao nao esta totalmente isolada de detalhes de infraestrutura.

### 2.3. Criacao do estado inicial

Inicio: `Aplicacao/Servicos/InicializadorDaFazenda.hpp::criarEstadoInicialDoJogo`.

Arquivos envolvidos:

- `Aplicacao/Estado/EstadoDoJogo.hpp`
- `Aplicacao/Servicos/InicializadorDaFazenda.hpp`
- `Dominio/Grade/GradeGlobalDeCanteiros.hpp`
- `Dominio/Grade/TileDeTerra.hpp`

Fluxo atual:

- Cria uma `GradeGlobalDeCanteiros`.
- Ativa um nucleo inicial `2x2`.
- Define `tamanhoAtualDoGrid` como `TAMANHO_INICIAL_GRID` normalizado.
- O personagem ja nasce no centro global `(128,128)`, dentro do nucleo inicial ativado.

Estado inicial confirmado pelos testes:

- Sem semente selecionada.
- Grid atual `12x12`.
- 4 tiles existentes.
- 0 canteiros em crescimento.

### 2.4. Renderizacao da fazenda

Inicio: loop de render em `Principal.cpp`.

Arquivos envolvidos:

- `Apresentacao/Renderizacao/Mundo/DesenhoDoMundo.hpp`
- `Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp`
- `Apresentacao/Isometria/Isometrico.hpp`
- `Apresentacao/Camera/CameraDoJogo.hpp`
- `Infraestrutura/Assets/RecursosDaFazenda.hpp`

Fluxo atual:

1. Desenha fundo.
2. Itera `grade.posicoesDeTilesExistentes()`.
3. Filtra tiles fora da grade jogavel atual e fora da tela.
4. Resolve textura de terra e sprite de planta.
5. Desenha canteiro, planta, highlight e preview de criacao de terra.
6. Desenha limite da grade jogavel.

Observacoes:

- O renderizador so itera tiles existentes, o que evita varrer a grade global `256x256`.
- Existe fallback vetorial para fundo, canteiro, planta e icones.
- Ha duplicacao entre `estadoVisualTemPlanta()` em apresentacao e `estadoEhFaseVisualDaPlanta()` em infraestrutura.

### 2.5. Camera, zoom e pan

Inicio:

- Mouse wheel em `Principal.cpp` chama `Camera::aplicarZoomNoPonto`.
- Botao direito ou meio inicia pan.
- Movimento do mouse durante pan chama `Camera::moverPanDaCamera`.
- Loop chama `Camera::atualizarInerciaDaCamera`.

Arquivos envolvidos:

- `Apresentacao/Camera/CameraDoJogo.hpp`
- `Apresentacao/Isometria/Isometrico.hpp`
- `Compartilhado/Constantes.hpp`

Fluxo atual:

- Zoom limitado entre `0.5x` e `2.0x`.
- Pan limitado por margem minima visivel de 25% da janela.
- HOME recentraliza camera e zoom.
- F5 recarrega configuracao e recentraliza grade.

Riscos visuais:

- Dimensoes de tile usam `round`, mas varios calculos usam `largura / 2` e `altura / 2` inteiros. A propria documentacao ja registra assimetrias de 1 px em zooms com dimensoes impares.
- Inercia de pan acumula deslocamento com `static_cast<int>(velocidade * deltaTime)`, podendo perder movimento subpixel e gerar sensacao de travamento ou tremor em velocidades baixas.

### 2.6. Interface, toolbar e loja

Inicio: eventos de mouse em `Principal.cpp`.

Arquivos envolvidos:

- `Apresentacao/Interface/BarraDeFerramentas/BarraDeFerramentas.hpp`
- `Apresentacao/Renderizacao/UI/BarraDeFerramentasRenderer.hpp`
- `Apresentacao/Renderizacao/UI/IconesDasFerramentas.hpp`
- `Aplicacao/Estado/EstadoDoJogo.hpp`

Fluxo atual:

- Toolbar tem 5 botoes: cursor, enxada, remover terra, semente e loja.
- `processarCliqueNaInterface` altera a ferramenta selecionada via parametro de saida e abre/fecha a loja.
- Painel da loja lista especies vindas de `FabricaDePlantas::todasAsEspecies()`.
- Clique em semente seleciona identificador da semente e troca a ferramenta para `Semente`.

Comportamento confirmado:

- Clique em botao de toolbar interrompe o fluxo e nao move personagem.
- Clique dentro do painel da loja interrompe o fluxo e nao move personagem.
- Clique em semente seleciona semente e nao planta diretamente.

Ponto a observar:

- Clique fora do painel enquanto a loja esta aberta cai no fluxo normal do mundo; a loja permanece aberta e o personagem pode se mover. Isso pode ser comportamento aceitavel, mas o contrato nao esta documentado nem testado.

### 2.7. Ferramentas

Inicio: `Aplicacao/Servicos/ServicoDeFerramentas.hpp::aplicarFerramentaNoJogo`.

Arquivos envolvidos:

- `Dominio/Ferramentas/Ferramenta.hpp`
- `Dominio/Ferramentas/RegistroDeFerramentas.hpp`
- `Dominio/Ferramentas/Enxada.hpp`
- `Dominio/Ferramentas/FerramentaDeSemente.hpp`
- `Dominio/Ferramentas/CursorDeColheita.hpp`
- `Dominio/Ferramentas/RemovedorDeTerra.hpp`
- `Dominio/Ferramentas/FerramentaDaLoja.hpp`

Fluxo atual:

- `RegistroDeFerramentas` registra instancias polimorficas.
- Aplicacao monta `ContextoDaFerramenta`.
- Ferramenta selecionada aplica sua regra.

Contratos atuais:

- `Enxada`: cria tile inexistente dentro da area jogavel, limpa `PlantaMorta`/`Restos` ou ara `TerraVazia`.
- `Semente`: exige canteiro arado, semente selecionada e moedas suficientes.
- `Cursor`: colhe apenas canteiro maduro.
- `RemoverTerra`: remove qualquer tile existente dentro da area jogavel.
- `Loja`: nao altera o dominio.

Risco:

- `RemovedorDeTerra` nao diferencia tile vazio, arado, plantado, maduro ou morto. Se isso for intencional, falta explicitar o contrato; se nao for, ele permite perda silenciosa de planta.

### 2.8. Plantio, crescimento e colheita

Inicio:

- Plantio: `FerramentaDeSemente::aplicar`.
- Crescimento: `ServicoDeTempo::avancarTempoDoJogo`.
- Colheita: `CursorDeColheita::aplicar`.

Arquivos envolvidos:

- `Dominio/Canteiros/Canteiro.hpp`
- `Dominio/Plantas/Planta.hpp`
- `Dominio/Plantas/Especies/PlantaMirtilo.hpp`
- `Dominio/Grade/GradeGlobalDeCanteiros.hpp`
- `Dominio/Jogador/Jogador.hpp`

Fluxo atual:

- `Canteiro` e dono das transicoes internas.
- Planta e polimorfica e define tempos/recompensa.
- Grade mantem lista de canteiros em crescimento.
- Servico de tempo consome acumulador em passos de 1 segundo.
- Ao colher, `Jogador` recebe recompensa e o canteiro passa para `Restos`.

Comportamento confirmado por teste:

- Plantio reduz moedas pelo custo.
- Crescimento passa por semente, crescendo, jovem, madura e morta.
- Colheita da planta madura soma moedas e experiencia.
- Planta morta sai da lista de crescimento.

### 2.9. Personagem, movimento e animacoes

Inicio:

- Clique em area jogavel chama `jogo.personagem().caminharAte(posicaoNaGrade)`.
- Loop chama `ServicoDeTempo::avancarTempoDoJogo`.

Arquivos envolvidos:

- `Dominio/Personagem/Personagem.hpp`
- `Apresentacao/Animacao/AnimadorDoPersonagem.hpp`
- `Apresentacao/Animacao/AnimacaoIdleDoPersonagem.hpp`
- `Apresentacao/Renderizacao/Mundo/RenderizadorDoPersonagem.hpp`
- `Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp`

Fluxo atual:

- Posicao logica do personagem representa os pes em coordenadas de grade decimal.
- Movimento e feito por waypoints em L isometrico.
- Renderizacao converte a posicao dos pes para tela e ancora o sprite por esse ponto.
- Idle tem animacao aleatoria na apresentacao.
- Caminhadas usam configuracao visual de idle como fallback.

Contrato preservado:

- A ancora logica pelos pes esta preservada.

Acoplamento corrigido:

- O dominio do personagem nao possui `AnimacaoVisualDoPersonagem`, indice de frame, piscada ou maquina de spritesheet.
- A apresentacao escolhe a animacao visual a partir de `estadoAtual()` e `direcaoAtual()`.

### 2.10. HUD, audio e configuracoes

Inicio:

- HUD e desenhada no final do loop de render.
- Botao de configuracoes e tratado no fluxo de mouse de `Principal.cpp`.

Arquivos envolvidos:

- `Apresentacao/Renderizacao/UI/HudRenderer.hpp`
- `Aplicacao/Estado/EstadoDoJogo.hpp`
- `Infraestrutura/Assets/RecursosDaFazenda.hpp`
- `Principal.cpp`

Fluxo atual:

- HUD mostra moedas, XP e nivel.
- Botao de configuracoes abre painel.
- Painel tem botao de som.
- Mute altera `EstadoDoJogo::audioMutado` e chama `Mix_Volume`/`Mix_VolumeMusic`.
- Musica ambiente toca se audio inicializou.
- Sons de acao sao mapeados por `AcaoDaFerramenta`.

Acoplamento observado:

- Estado de painel de configuracoes e audio mutado ficam em `Aplicacao::EstadoDoJogo`, embora sejam estados de UI/infraestrutura.
- Hitboxes de configuracoes sao duplicadas entre `Principal.cpp` e `HudRenderer.hpp`.

### 2.11. Encerramento

Inicio:

- `SDL_QUIT` ou tecla `ESC` no loop principal.

Arquivos envolvidos:

- `Principal.cpp`
- `Infraestrutura/SDL/ContextoSDL.hpp`
- `Infraestrutura/Assets/GerenciadorDeAtivosSDL.hpp`

Fluxo atual:

- Loop define `executando = false`.
- Objetos RAII liberam texturas, fontes, sons, musicas, renderer, janela, audio, TTF, IMG e SDL.

Estado:

- Encerramento esta bem encapsulado por RAII.

## 3. Avaliacao por camada

### 3.1. Compartilhado

Arquivos:

- `Compartilhado/Constantes.hpp`
- `Compartilhado/Geometria/Posicoes.hpp`

Estado:

- Camada simples, pura e sem SDL.
- Concentra constantes globais e tipos de posicao.

Risco:

- `Constantes.hpp` mistura constantes de janela, grade, camera, personagem e UI. Ainda aceitavel pelo tamanho atual, mas pode virar arquivo de configuracao global dificil de governar.

### 3.2. Dominio

Arquivos principais:

- `Canteiros`, `Ferramentas`, `Grade`, `Jogador`, `Personagem`, `Plantas`.

Estado:

- Nao ha inclusoes de SDL, SDL_image, SDL_ttf ou SDL_mixer.
- Nao ha `switch` no dominio.
- Ferramentas e plantas usam polimorfismo.
- Canteiro centraliza transicoes.
- Grade protege vetores internos com referencias constantes.

Violacoes ou desvios:

- A dependencia visual do dominio de plantas foi removida; especies expoem apenas dados de gameplay e `identificadorDaSemente`.
- A dependencia visual do dominio do personagem foi removida; personagem expoe apenas estado logico de movimento.

Conclusao:

- O dominio esta puro em termos de SDL, infraestrutura e semantica visual do personagem.

### 3.3. Aplicacao

Arquivos:

- `Aplicacao/Estado/EstadoDoJogo.hpp`
- `Aplicacao/Servicos/InicializadorDaFazenda.hpp`
- `Aplicacao/Servicos/ServicoDeFerramentas.hpp`
- `Aplicacao/Servicos/ServicoDeTempo.hpp`

Estado:

- Orquestra bem o dominio.
- Nao depende de SDL.
- Casos de uso sao pequenos e legiveis.

Desvios:

- `EstadoDoJogo` guarda `painelConfiguracoesAberto_` e `audioMutado_`, que sao estados de UI/audio, nao de gameplay.
- `EstadoDoJogo` expoe acesso mutavel a `grade`, `jogador` e `personagem`; isso facilita integracao, mas amplia a superficie para mutacoes fora dos casos de uso.

### 3.4. Apresentacao

Arquivos:

- Camera, isometria, interface, renderizacao do mundo, UI, HUD e cursor.

Estado:

- Desenha a fazenda lendo estado de aplicacao/dominio.
- Implementa hit-test visual e layout de toolbar.
- Contem `switch` visuais aceitaveis para cores, icones e mapeamentos.

Desvios:

- `RenderizadorDaFazenda.hpp`, `RenderizadorDoPersonagem.hpp` e `BarraDeFerramentasRenderer.hpp` dependem de tipos concretos de `Infraestrutura::Assets`.
- Existem muitos numeros visuais locais, especialmente em HUD, toolbar, icones vetoriais e fallback de plantas.

### 3.5. Infraestrutura

Arquivos:

- SDL, assets, configuracao.

Estado:

- Isola inicializacao SDL e ciclo de vida de recursos.
- Tem fallback para audio indisponivel, assets ausentes e icones vetoriais.
- Faz cache de texturas/fontes/sons/musicas.

Desvios:

- `LeitorDeConfiguracao.hpp` depende de `Apresentacao::Camera`.
- `RecursosDaFazenda.hpp` depende de dominio para mapear estados e especies para assets.
- Falhas de asset configurado podem ficar silenciosas quando ha fallback existente.

## 4. Achados classificados por severidade

### Critico

Nenhum achado critico confirmado.

### Alto

#### A1. Dependencia visual do dominio de plantas corrigida

Arquivo/fluxo:

- `src/Dominio/Plantas/Planta.hpp`
- `src/Dominio/Plantas/Especies/PlantaMirtilo.hpp`
- `src/Infraestrutura/Assets/RecursosDaFazenda.hpp`
- `src/Infraestrutura/Assets/CatalogoVisualDePlantas.hpp`

Estado atual:

- A interface `Planta` expoe somente dados de gameplay e `identificadorDaSemente`.
- `PlantaMirtilo` nao guarda pasta, sprite, caminho de asset ou convencao visual.
- A infraestrutura usa `CatalogoVisualDePlantas.hpp` para resolver pasta, arquivos de sprite e icone da semente a partir do identificador.

Por que importa:

- O dominio modela apenas regra de gameplay.
- A documentacao diz que o dominio deve permanecer sem dependencia visual, e o contrato de plantas agora cumpre esse ponto.

Impacto provavel:

- Adicionar nova planta ainda exige criar a especie no dominio por regra de jogo e cadastrar sua configuracao visual na infraestrutura.
- Mudancas de pipeline visual ficam concentradas no catalogo visual e no carregamento de assets.

Recomendacao:

- Manter no dominio apenas identificador, nome de gameplay, custo, tempos e recompensa.
- Manter metadados visuais de planta no catalogo de infraestrutura, indexado por `identificadorDaSemente`.

#### A2. Acoplamento visual do personagem no dominio corrigido

Arquivo/fluxo:

- `src/Dominio/Personagem/Personagem.hpp`
- `src/Apresentacao/Animacao/AnimadorDoPersonagem.hpp`
- `src/Apresentacao/Animacao/AnimacaoIdleDoPersonagem.hpp`
- `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp`
- `src/Apresentacao/Renderizacao/Mundo/RenderizadorDoPersonagem.hpp`

Status:

- Corrigido. O dominio do personagem expoe apenas estado logico: parado, andando, direcao e posicao dos pes.
- `AnimacaoVisualDoPersonagem`, indice de frame, piscada e temporizacao visual ficam fora do dominio.
- A maquina de idle visual vive em `Apresentacao/Animacao/AnimacaoIdleDoPersonagem.hpp`.

Por que importa:

- Trocar spritesheet, numero de frames ou modelo de animacao nao exige mudanca no dominio.
- Movimento e animacao visual podem ser testados separadamente.

Impacto provavel:

- Novas animacoes de caminhada podem ser adicionadas alterando configuracao visual e apresentacao.

Regra preservada:

- Manter no dominio apenas estado fisico/logico: parado, andando, direcao e posicao dos pes.

### Medio

#### M1. `Principal.cpp` concentra cena, eventos, UI, camera, audio e aplicacao

Arquivo/fluxo:

- `src/Principal.cpp`
- Documentacao em `doc/arquitetura.md`, secao de pontos legados.

Problema:

- O arquivo ainda contem loop SDL completo, roteamento de eventos, regras de bloqueio de UI, camera, loja, configuracoes, audio e chamada de casos de uso.

Por que importa:

- A ordem dos `continue` define comportamento funcional importante.
- Fluxos como "clique em UI nao move personagem" ficam dificeis de testar sem rodar o loop SDL.

Impacto provavel:

- Regressao em interacoes ao adicionar menus, novas ferramentas ou novas cenas.
- Crescimento natural para arquivo central dificil de revisar.

Recomendacao:

- Extrair uma `CenaFazenda` ou controlador de cena em tarefa separada.
- Separar roteamento de input, estado de UI transiente e chamada de casos de uso.

#### M2. Estado de UI/audio vive dentro de `EstadoDoJogo`

Arquivo/fluxo:

- `src/Aplicacao/Estado/EstadoDoJogo.hpp`
- `src/Principal.cpp`
- `src/Apresentacao/Renderizacao/UI/HudRenderer.hpp`

Problema:

- `painelConfiguracoesAberto_` e `audioMutado_` ficam no estado agregado do jogo.

Por que importa:

- Esses dados nao sao regra de fazenda, grade, personagem ou economia.
- Misturam estado de aplicacao/gameplay com estado de apresentacao/infraestrutura.

Impacto provavel:

- Salvamento de jogo, testes de dominio/aplicacao e novas telas podem herdar estado de UI sem querer.

Recomendacao:

- Criar um estado de UI/cena separado para loja, painel de configuracoes e audio.
- Deixar `EstadoDoJogo` focado em gameplay.

#### M3. Hitboxes de configuracoes sao duplicadas entre render e input

Arquivo/fluxo:

- `src/Principal.cpp::calcularAreaDoBotaoConfiguracoes`
- `src/Principal.cpp::calcularAreaDoPainelConfiguracoes`
- `src/Apresentacao/Renderizacao/UI/HudRenderer.hpp`

Problema:

- O mesmo layout do botao/painel de configuracoes existe no renderizador e em funcoes locais do `Principal.cpp`.

Por que importa:

- Uma alteracao visual no HUD pode deixar a area clicavel divergente da area desenhada.

Impacto provavel:

- Clique aparentemente correto pode falhar, ou clique fora do painel pode ser tratado como dentro.

Recomendacao:

- Centralizar layout de configuracoes em uma estrutura compartilhada pela renderizacao e pelo input.

#### M4. `deslocamentoGradeHorizontal/Vertical` do `config.ini` sao lidos e depois sobrescritos

Arquivo/fluxo:

- `assets/config.ini`
- `src/Infraestrutura/Configuracao/LeitorDeConfiguracao.hpp`
- `src/Principal.cpp`

Problema:

- O leitor aplica `deslocamentoGradeHorizontal` e `deslocamentoGradeVertical`, mas logo depois chama `Camera::aplicarOrigemCentradaDaGrade`, sobrescrevendo os valores.
- O `config.ini` atual usa exatamente essas chaves.

Por que importa:

- Quem editar o `config.ini` pode esperar que esses campos reposicionem a grade, mas eles nao produzem efeito final.

Impacto provavel:

- F5 parece recarregar configuracao, mas parte da configuracao visual e ignorada.

Recomendacao:

- Remover essas chaves se a centralizacao for o contrato atual, ou trocar para `origemGradeHorizontal/Vertical` e documentar a precedencia.

#### M5. Testes usam `assert`, que pode ser desabilitado por `NDEBUG`

Arquivo/fluxo:

- `tests/TestesLogica.cpp`
- `CMakeLists.txt`

Problema:

- Todas as verificacoes de teste usam `assert`.
- Em build Release com `NDEBUG`, as assercoes podem ser removidas.

Por que importa:

- O binario de teste pode passar sem validar comportamento.

Impacto provavel:

- Falsa confianca em CI ou build local Release.

Recomendacao:

- Migrar testes para uma macro propria que nao dependa de `assert` ou adotar um framework leve de teste.

#### M6. Apresentacao depende de tipos concretos de infraestrutura de assets

Arquivo/fluxo:

- `src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp`
- `src/Apresentacao/Renderizacao/Mundo/RenderizadorDoPersonagem.hpp`
- `src/Apresentacao/Renderizacao/UI/BarraDeFerramentasRenderer.hpp`
- `src/Infraestrutura/Assets/RecursosDaFazenda.hpp`

Problema:

- Renderizadores recebem e manipulam tipos definidos em `Infraestrutura::Assets`.

Por que importa:

- A apresentacao fica acoplada ao formato de cache/carregamento dos assets.

Impacto provavel:

- Alterar gerenciamento de assets pode exigir alteracao em renderizadores.

Recomendacao:

- Em uma etapa futura, considerar um catalogo visual/DTO de apresentacao ou interfaces mais estreitas para texturas e metadados.

#### M7. Contrato do `RemovedorDeTerra` e amplo demais ou pouco documentado

Arquivo/fluxo:

- `src/Dominio/Ferramentas/RemovedorDeTerra.hpp`
- `src/Dominio/Grade/GradeGlobalDeCanteiros.hpp`

Problema:

- A ferramenta remove qualquer tile existente dentro da area jogavel, independentemente do estado do canteiro.

Por que importa:

- Pode apagar planta em crescimento ou madura sem regra explicita de confirmacao/custo/penalidade.

Impacto provavel:

- Caso o design espere restricao, ha perda silenciosa de progresso do jogador.

Recomendacao:

- Decidir o contrato de design: remover tudo e intencional, ou apenas terra vazia/arada/morta.
- Se for intencional, documentar e testar.

#### M8. Fluxo de input da loja tem comportamento nao documentado fora do painel

Arquivo/fluxo:

- `src/Principal.cpp`
- `src/Apresentacao/Interface/BarraDeFerramentas/BarraDeFerramentas.hpp`

Problema:

- Com a loja aberta, clique fora do painel continua para o fluxo do mundo; o personagem pode andar e a loja continua aberta.

Por que importa:

- Pode ser intencional, mas nao esta documentado nem testado.

Impacto provavel:

- Regressao de UX quando a loja ganhar mais opcoes ou bloquear interacao do mundo.

Recomendacao:

- Definir o contrato: clique fora fecha loja, bloqueia mundo, ou permite jogar com loja aberta.
- Adicionar teste ou extrair roteamento de input para validar.

### Baixo

#### B1. `assets/config.ini` aponta para background inexistente

Arquivo/fluxo:

- `assets/config.ini`
- `src/Infraestrutura/Assets/LocalizadorDeAssets.hpp`
- `assets/background/fundo_gramado.png`

Problema:

- `arquivoBackgroundPrincipal = background.png`, mas nao existe `assets/background/background.png`.
- O jogo usa `background/fundo_gramado.png` como fallback.

Por que importa:

- A configuracao ativa nao representa o asset realmente usado.

Impacto provavel:

- Confusao ao trocar arte de fundo; fallback mascara erro de configuracao.

Recomendacao:

- Atualizar `config.ini` para o arquivo real ou adicionar log quando o configurado nao existir e fallback for usado.

#### B2. Comentario de recompensa do mirtilo contradiz o codigo

Arquivo/fluxo:

- `src/Dominio/Plantas/Especies/PlantaMirtilo.hpp`

Problema:

- O codigo retorna `RecompensaDaColheita{35,3}`, mas o comentario diz "40 moedas e 10 de experiencia".

Por que importa:

- Comentarios incorretos confundem balanceamento e testes.

Impacto provavel:

- Ajustes de economia podem partir de premissa errada.

Recomendacao:

- Corrigir comentario ou revisar valor real desejado em tarefa pequena separada.

#### B3. Contrato de `PlantaMorta` e `Restos` corrigido

Arquivo/fluxo:

- `src/Infraestrutura/Assets/RecursosDaFazenda.hpp`
- `src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp`
- `assets/sprites/tiles/tile_terra_restos.png`

Estado corrigido:

- `PlantaMorta` continua sendo fase visual de planta: base `tile_terra_arada.png` mais sprite de planta morta.
- `Restos` virou estado proprio do canteiro e usa exclusivamente `tile_terra_restos.png`.
- Colher planta madura e limpar planta morta levam o canteiro para `Restos`.
- O novo plantio exige limpar `Restos`, arar novamente e so entao plantar.

#### B4. Duplicacao de mapeamento de fase visual de planta

Arquivo/fluxo:

- `src/Apresentacao/Renderizacao/Mundo/DesenhoDoMundo.hpp`
- `src/Infraestrutura/Assets/RecursosDaFazenda.hpp`

Problema:

- Ha funcoes separadas para responder se um estado visual possui planta.

Por que importa:

- Novos estados de canteiro podem ser atualizados em um lugar e esquecidos em outro.

Impacto provavel:

- Divergencia entre fallback visual e carregamento de sprite.

Recomendacao:

- Centralizar esse mapeamento em uma funcao de borda visual unica.

#### B5. `verificarCliqueNoBotao` inclui o limite direito/inferior

Arquivo/fluxo:

- `src/Apresentacao/Interface/BarraDeFerramentas/BarraDeFerramentas.hpp`

Problema:

- O hit-test usa `<= x + largura` e `<= y + altura`.

Por que importa:

- Em retangulos adjacentes, isso pode criar sobreposicao de 1 px.

Impacto provavel:

- Baixo hoje, porque a toolbar tem espacamento.

Recomendacao:

- Usar semantica usual de retangulo: `x <= ponto < x + largura`.

#### B6. `remover_fundo.py` usa caminho absoluto antigo

Arquivo/fluxo:

- `remover_fundo.py`

Problema:

- O script aponta para `C:\dev\MiniFazenda\assets\...`, nao para o workspace atual.

Por que importa:

- Executar o script como esta pode nao processar o asset esperado.

Impacto provavel:

- Baixo no jogo, medio para pipeline manual de arte.

Recomendacao:

- Parametrizar caminho ou resolver a partir da raiz do projeto.

#### B7. Configuracao do VS Code desativa squiggles de C/C++

Arquivo/fluxo:

- `.vscode/settings.json`

Problema:

- `C_Cpp.errorSquiggles` esta desabilitado.

Por que importa:

- O editor pode deixar de mostrar erros locais de include/tipos.

Impacto provavel:

- Baixo se build/testes forem usados com disciplina.

Recomendacao:

- Manter build e testes como fonte de verdade; reavaliar a configuracao se a equipe depender de IntelliSense.

### Observacoes

#### O1. Assets existentes nao significam funcionalidades implementadas

Ha assets de animais, varios sons e sprites de UI que nao sao referenciados pelo codigo atual. Exemplos: animais, `plant.mp3`, `water.wav`, `levelup.wav`, `mission_complete.wav`, `pesticide.wav`, `sell.wav`, `cow.wav`, `chicken.wav`, `sheep.wav`, `dog_bark.wav`.

Isso nao e erro, mas deve ser tratado como backlog de asset, nao como feature ativa.

#### O2. `AcelerarCrescimento` existe sem fluxo atual

`AcaoDaFerramenta::AcelerarCrescimento`, som `gift_open.wav` e `Canteiro::acelerarParaMadura()` existem, mas nao ha ferramenta ou UI chamando esse fluxo.

#### O3. README usa caminho generico antigo

O README instrui `cd C:\dev\MiniFazenda`, enquanto a pasta auditada e `MiniFazenda 2`. Isso e comum em docs locais, mas pode confundir novos ambientes.

## 5. Divergencias entre documentacao e codigo

1. Corrigido: `doc/arquitetura.md` afirma que o dominio deve ficar sem dependencia visual, e o dominio do personagem agora cumpre esse contrato.

2. `doc/arquitetura.md` lista uma futura extracao de `CenaFazenda`. O codigo atual ainda nao possui `CenaFazenda`; `Principal.cpp` concentra a cena.

3. `doc/Tamanhos.md` registra `background.png` como arquivo configurado e `fundo_gramado.png` como fallback. O `config.ini` realmente aponta para `background.png`, mas esse arquivo nao existe nos assets; portanto o fallback e o background real atual.

4. `assets/config.ini` usa `deslocamentoGradeHorizontal/Vertical`, mas o leitor sobrescreve esses campos com centralizacao automatica antes do uso final.

5. A recompensa de `PlantaMirtilo.hpp` esta documentada no proprio retorno como `{35,3}`.

6. Corrigido: a animacao idle real sorteia tempos em `Apresentacao/Animacao/AnimacaoIdleDoPersonagem.hpp`, fora do dominio.

## 6. Pontos fortes

- Dominio nao inclui SDL, SDL_image, SDL_ttf ou SDL_mixer.
- Nao ha `switch` no dominio.
- Ferramentas estao bem separadas por polimorfismo.
- Plantas usam classe base polimorfica.
- Canteiro centraliza transicoes importantes.
- Grade evita varrer `256x256` na renderizacao ao manter listas de tiles ativos/crescimento.
- RAII de SDL, renderer, janela, cursor e assets esta simples e efetivo.
- Teste atual cobre fluxo de plantio, crescimento, colheita, remocao, movimento, hit-test isometrico, camera e loja.
- Assets principais de personagem e tiles batem com configuracoes documentadas.
- CMake inclui headers automaticamente e possui alvo de validacao arquitetural.

## 7. Pontos frageis

- `Principal.cpp` e o centro de quase todos os fluxos de runtime.
- Estado de animacao visual do personagem ainda fica no loop principal, ate uma futura extracao de cena.
- Estado de UI/audio esta junto do estado de gameplay.
- Renderizadores conhecem estruturas concretas de infraestrutura de assets.
- Testes dependem de `assert`.
- Fluxo de input real do `Principal.cpp` nao tem teste automatizado.
- Fallbacks de asset podem esconder configuracao errada.
- Algumas convencoes visuais estao espalhadas em switches e numeros locais.
- Zoom/pan usam arredondamentos inteiros em varios pontos.

## 8. Riscos futuros

1. Ao adicionar novas plantas, o risco principal e esquecer o cadastro correspondente no catalogo visual de infraestrutura.
2. Ao adicionar novas animacoes, o risco principal e esquecer de ajustar a configuracao visual ou o modo de reproducao da apresentacao.
3. Ao adicionar novas telas, `Principal.cpp` pode se tornar o gargalo de manutencao.
4. Ao adicionar salvamento, `EstadoDoJogo` pode persistir indevidamente estado de UI/audio.
5. Ao adicionar CI Release, testes com `assert` podem virar falsos positivos.
6. Ao trocar assets, `background.png` ausente e fallbacks silenciosos podem mascarar erros.
7. Ao adicionar estados de canteiro, mapeamentos duplicados podem divergir.
8. Ao refinar camera, arredondamento inteiro pode causar drift visual ou pan irregular.

## 9. Recomendacoes priorizadas

### Prioridade 1

1. Manter o contrato novo de plantas: dominio fornece `identificadorDaSemente`, catalogo visual externo resolve assets.
2. Separar estado de UI/audio de `EstadoDoJogo`.
3. Extrair o roteamento de input ou iniciar a `CenaFazenda`, mantendo comportamento atual.
4. Trocar testes baseados em `assert` por verificacoes sempre ativas.

### Prioridade 2

5. Definir contrato de loja aberta e clique fora do painel.
6. Centralizar layout/hitbox do painel de configuracoes.
7. Corrigir precedencia das chaves de layout do `config.ini`.
8. Decidir se `RemovedorDeTerra` pode apagar plantas vivas.

### Prioridade 3

9. Corrigir comentario da recompensa do mirtilo.
10. Atualizar `arquivoBackgroundPrincipal` para asset real ou logar fallback.
11. Manter o contrato corrigido de `PlantaMorta` e `Restos`.
12. Parametrizar `remover_fundo.py`.
13. Reduzir duplicacao de mapeamentos visuais de fase de planta.

## 10. Proximos passos sugeridos

1. Task pequena: corrigir divergencias documentais e de configuracao sem mudar gameplay (background configurado, README/caminho, script auxiliar).
2. Task de arquitetura de plantas concluida: catalogo visual em infraestrutura, dominio fornece apenas `identificadorDaSemente`.
3. Task de estado: separar `EstadoDaInterface` ou `EstadoDaCenaFazenda` de `EstadoDoJogo`.
4. Task de input: extrair `CenaFazenda` ou controlador de input, cobrindo toolbar, loja, painel e mundo.
5. Task de testes: substituir `assert` por checagens sempre ativas e adicionar cobertura do fluxo de input sem SDL real quando possivel.
6. Task visual: revisar arredondamento de zoom/pan e centralizar contratos de layout/hitbox.

## 11. Limitacoes da auditoria

- Nao executei o jogo interativamente em janela.
- Nao capturei screenshot nem validei visual final por pixel.
- Nao testei audio em dispositivo real; apenas build e caminhos/mapeamentos.
- Nao criei novos testes, conforme solicitado.
- Nao refatorei nem corrigi codigo funcional.
- A analise visual foi feita por leitura de codigo, verificacao de dimensoes dos PNGs e comparacao com documentacao.


----------------------------------------------------

## 12. Tasks:

1. **Remover dependência visual do domínio de plantas** (CONCLUIDO)
   **corrigido:** Planta nao sabe nome de pasta, sprite ou convencao visual. O dominio fornece `identificadorDaSemente`; o catalogo visual externo resolve pasta, sprites e icone.
   **onde:** `src/Dominio/Plantas/Planta.hpp`, `src/Dominio/Plantas/Especies/PlantaMirtilo.hpp`, `src/Infraestrutura/Assets/CatalogoVisualDePlantas.hpp`, `src/Infraestrutura/Assets/RecursosDaFazenda.hpp`
   **prioridade:** Alta

2. **Remover lógica visual da animação do personagem no domínio** (CONCLUIDO)
   **corrigido:** O domínio guarda apenas estado lógico: parado, andando, direção, caminho e posição dos pés. Índice de frame, piscada, tempo visual e spritesheet foram movidos para apresentação/infraestrutura.
   **onde:** `src/Dominio/Personagem/Personagem.hpp`, `src/Apresentacao/Animacao/AnimadorDoPersonagem.hpp`, `src/Apresentacao/Animacao/AnimacaoIdleDoPersonagem.hpp`, `src/Apresentacao/Renderizacao/Mundo/RenderizadorDoPersonagem.hpp`, `src/Infraestrutura/Assets/ConfigVisualDoPersonagem.hpp`
   **prioridade:** Alta

3. **Separar estado de UI/audio do estado de gameplay** (CONCLUIDO)
   **corrigir:** Remover `painelConfiguracoesAberto_` e `audioMutado_` de `EstadoDoJogo`. Criar algo como `EstadoDaCenaFazenda` ou `EstadoDaInterface`.
   **onde:** `src/Aplicacao/Estado/EstadoDoJogo.hpp`, `src/Principal.cpp`, `src/Apresentacao/Renderizacao/UI/HudRenderer.hpp`
   **prioridade:** Alta

4. **Extrair o fluxo principal de cena/input do `Principal.cpp`** (CONCLUIDO)
   **corrigir:** Tirar do `Principal.cpp` o roteamento de evento, UI, loja, painel, câmera, áudio e interação com mundo. Criar `CenaFazenda` ou controladores menores mantendo o comportamento atual.
   **onde:** `src/Principal.cpp`, possível novo `src/Apresentacao/Cenas/CenaFazenda.hpp`
   **prioridade:** Média

5. **Trocar testes baseados em `assert` por checagens sempre ativas**(CONCLUIDO)
   **corrigir:** `assert` pode sumir em build Release com `NDEBUG`. Criar macro própria de teste ou usar framework leve.
   **onde:** `tests/TestesLogica.cpp`, `CMakeLists.txt`
   **prioridade:** Alta

6. **Definir contrato do `RemovedorDeTerra`**
   **corrigir:** Decidir se a ferramenta pode remover tile com planta viva, madura ou morta. Se puder, documentar e testar. Se não puder, bloquear por estado do canteiro.
   **onde:** `src/Dominio/Ferramentas/RemovedorDeTerra.hpp`, `src/Dominio/Grade/GradeGlobalDeCanteiros.hpp`
   **prioridade:** Média

7. **Definir comportamento da loja aberta ao clicar fora do painel**
   **corrigir:** Hoje o clique fora do painel pode mover o personagem e a loja continua aberta. Decidir se clique fora fecha loja, bloqueia mundo ou permite jogar com loja aberta. Depois documentar/testar.
   **onde:** `src/Principal.cpp`, `src/Apresentacao/Interface/BarraDeFerramentas/BarraDeFerramentas.hpp`
   **prioridade:** Média

8. **Centralizar layout e hitbox do painel de configurações**
   **corrigir:** O botão e o painel de configurações são desenhados em um lugar e clicados em outro. Criar uma estrutura única de layout usada pelo render e pelo input.
   **onde:** `src/Principal.cpp`, `src/Apresentacao/Renderizacao/UI/HudRenderer.hpp`
   **prioridade:** Média

9. **Corrigir precedência do `config.ini` na origem da grade**
   **corrigir:** `deslocamentoGradeHorizontal/Vertical` são lidos, mas depois sobrescritos pela centralização automática da câmera. Remover essas chaves ou documentar que a centralização vence.
   **onde:** `assets/config.ini`, `src/Infraestrutura/Configuracao/LeitorDeConfiguracao.hpp`, `src/Principal.cpp`, `src/Apresentacao/Camera/CameraDoJogo.hpp`
   **prioridade:** Média

10. **Desacoplar renderizadores dos tipos concretos de infraestrutura**
    **corrigir:** Renderizadores conhecem estruturas de `Infraestrutura::Assets`. Criar DTO/catálogo visual de apresentação ou interface mais estreita para texturas e metadados.
    **onde:** `src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp`, `src/Apresentacao/Renderizacao/Mundo/RenderizadorDoPersonagem.hpp`, `src/Apresentacao/Renderizacao/UI/BarraDeFerramentasRenderer.hpp`, `src/Infraestrutura/Assets/RecursosDaFazenda.hpp`
    **prioridade:** Média

11. **Corrigir background configurado inexistente**
    **corrigir:** `config.ini` aponta para `background.png`, mas o jogo usa fallback `background/fundo_gramado.png`. Atualizar o caminho real ou logar claramente quando cair em fallback.
    **onde:** `assets/config.ini`, `src/Infraestrutura/Assets/LocalizadorDeAssets.hpp`, `assets/background/`
    **prioridade:** Baixa

12. **Corrigir comentário errado da recompensa do mirtilo**(CONCLUIDO)
    **corrigir:** O código retorna `{35, 3}`, mas o comentário fala em `40 moedas e 10 experiência`. Corrigir comentário ou ajustar o balanceamento real.
    **onde:** `src/Dominio/Plantas/Especies/PlantaMirtilo.hpp`
    **prioridade:** Baixa

13. **Resolver representação visual de planta morta** (CONCLUIDO)
    **corrigido:** `PlantaMorta` e `Restos` agora sao estados diferentes. Planta morta usa `tile_terra_arada.png` com sprite de planta morta por cima; restos usa exclusivamente `tile_terra_restos.png`, sem sprite de planta.
    **onde:** `src/Dominio/Canteiros/EstadoDoCanteiro.hpp`, `src/Dominio/Canteiros/Canteiro.hpp`, `src/Infraestrutura/Assets/RecursosDaFazenda.hpp`, `src/Apresentacao/Renderizacao/Mundo/RenderizadorDaFazenda.hpp`, `src/Apresentacao/Renderizacao/Mundo/DesenhoDoMundo.hpp`, `assets/sprites/tiles/tile_terra_restos.png`
    **prioridade:** Baixa

14. **Unificar mapeamento de estado visual de planta** (CONCLUIDO)
    **corrigir:** Há lógica duplicada para dizer se um estado visual possui planta. Centralizar em uma função de borda visual única.
    **onde:** `src/Apresentacao/Renderizacao/Mundo/DesenhoDoMundo.hpp`, `src/Infraestrutura/Assets/RecursosDaFazenda.hpp`
    **prioridade:** Baixa

15. **Corrigir hit-test de botão com limite inclusivo**
    **corrigir:** `verificarCliqueNoBotao` usa `<= x + largura` e `<= y + altura`. Trocar para padrão mais seguro: `x <= ponto < x + largura`.
    **onde:** `src/Apresentacao/Interface/BarraDeFerramentas/BarraDeFerramentas.hpp`
    **prioridade:** Baixa

16. **Parametrizar o script de remover fundo**
    **corrigir:** O script usa caminho absoluto antigo. Fazer aceitar argumento de entrada/saída ou resolver caminhos pela raiz do projeto.
    **onde:** `remover_fundo.py`
    **prioridade:** Baixa

17. **Atualizar README com caminho e comandos atuais**
    **corrigir:** README ainda usa caminho genérico/antigo. Ajustar para `MiniFazenda2` e documentar comandos de configurar, buildar, testar e rodar.
    **onde:** `README.md`
    **prioridade:** Baixa

18. **Reavaliar `C_Cpp.errorSquiggles` desativado**
    **corrigir:** Squiggles desligados podem esconder erro local no VSCode. Ativar se o IntelliSense já estiver estável; se não, documentar que build/testes são a fonte da verdade.
    **onde:** `.vscode/settings.json`
    **prioridade:** Baixa

19. **Separar constantes globais por domínio de responsabilidade**
    **corrigir:** `Constantes.hpp` mistura janela, grade, câmera, personagem e UI. Separar gradualmente em arquivos menores quando começar a crescer mais.
    **onde:** `src/Compartilhado/Constantes.hpp`
    **prioridade:** Baixa

20. **Revisar arredondamento de zoom/pan**
    **corrigir:** Zoom usa dimensões arredondadas e divisões inteiras; pan com inércia usa conversão para `int`, podendo perder movimento subpixel. Revisar se houver tremor, drift ou sensação travada.
    **onde:** `src/Apresentacao/Camera/CameraDoJogo.hpp`, `src/Apresentacao/Isometria/Isometrico.hpp`, `src/Apresentacao/Renderizacao/Primitivas/PrimitivasSDL.hpp`
    **prioridade:** Baixa

21. **Documentar assets existentes mas ainda não usados**
    **corrigir:** Animais, sons e alguns sprites existem, mas não são funcionalidades ativas. Registrar como backlog para não parecer bug.
    **onde:** `assets/`, documentação de backlog ou README
    **prioridade:** Baixa

22. **Decidir destino do fluxo de acelerar crescimento**
    **corrigir:** Existem `AcaoDaFerramenta::AcelerarCrescimento`, som relacionado e `Canteiro::acelerarParaMadura()`, mas não há ferramenta/UI usando isso. Decidir se vira feature ou se remove por enquanto.
    **onde:** `src/Dominio/Canteiros/Canteiro.hpp`, arquivos de ferramenta/ação, assets de som relacionados
    **prioridade:** Baixa
