import os
from rembg import remove
from PIL import Image

caminhos = [
    r"C:\dev\MiniFazenda\assets\sprites\personagem\bonequinho.png",
]

# Definição do tamanho padrão baseado nas primeiras fases (ex: 1024x512 ou o tamanho real delas)
# O script vai pegar o tamanho da fase 1 automaticamente se ela existir
imagem_guia = Image.open(caminhos[0])
LARGURA_PADRAO, ALTURA_PADRAO = imagem_guia.size

for caminho_entrada in caminhos:
    if os.path.exists(caminho_entrada):
        print(f"Processando e padronizando: {caminho_entrada}")
        
        imagem_original = Image.open(caminho_entrada)
        imagem_sem_fundo = remove(imagem_original)
        
        # Cria uma nova tela transparente no tamanho padrão exato
        nova_imagem = Image.new("RGBA", (LARGURA_PADRAO, ALTURA_PADRAO), (0, 0, 0, 0))
        
        # Calcula a posição para garantir que o sprite fique perfeitamente centralizado na base
        pos_x = (LARGURA_PADRAO - imagem_sem_fundo.width) // 2
        pos_y = ALTURA_PADRAO - imagem_sem_fundo.height  # Alinha pela base (chão do sprite)
        
        # Se o sprite for maior que o padrão, ele é redimensionado para caber mantendo a proporção
        if imagem_sem_fundo.width > LARGURA_PADRAO or imagem_sem_fundo.height > ALTURA_PADRAO:
            imagem_sem_fundo.thumbnail((LARGURA_PADRAO, ALTURA_PADRAO), Image.Resampling.LANCZOS)
            pos_x = (LARGURA_PADRAO - imagem_sem_fundo.width) // 2
            pos_y = ALTURA_PADRAO - imagem_sem_fundo.height
            
        nova_imagem.paste(imagem_sem_fundo, (pos_x, pos_y), imagem_sem_fundo)
        
        # Salva sobrescrevendo com o formato correto e tamanho padronizado
        nome_limpo = os.path.splitext(caminho_entrada)[0] + ".png"
        nova_imagem.save(nome_limpo, "PNG")
        
        # Remove o arquivo .jpg antigo se necessário
        if caminho_entrada.endswith(".jpg"):
            os.remove(caminho_entrada)
            
        print("Concluído!")
    else:
        print(f"Arquivo não encontrado: {caminho_entrada}")