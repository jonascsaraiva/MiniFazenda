# Script para remoção de fundo com rembg
import os
from rembg import remove
from PIL import Image

# Lista de caminhos das imagens
caminhos = [
    r"C:\dev\MiniFazenda\assets\sprites\tiles\tile_terra_seca.png",
    r"C:\dev\MiniFazenda\assets\sprites\tiles\tile_terra_restos.png",
    r"C:\dev\MiniFazenda\assets\sprites\tiles\tile_terra_arada.png",
    r"C:\dev\MiniFazenda\assets\sprites\tiles\tile_grama.png"
]

# Processamento em lote
for caminho_entrada in caminhos:
    if os.path.exists(caminho_entrada):
        print(f"Processando: {caminho_entrada}")
        
        imagem_original = Image.open(caminho_entrada)
        imagem_sem_fundo = remove(imagem_original)
        
        imagem_sem_fundo.save(caminho_entrada)
        print("Concluído!")
    else:
        print(f"Arquivo não encontrado: {caminho_entrada}")