import cv2
import numpy as np


# carregar as três imagens
portao_fechado = cv2.imread("fechado.jpg")
portao_aberto = cv2.imread("aberto.jpg")
nova_imagem = cv2.imread("aberto2.jpg")

# definir as coordenadas x e y da área de comparação
x = 470
y = 250
width = 450
height = 350

# selecionar a área de comparação em cada imagem
area_portao_fechado = portao_fechado[y:y+height, x:x+width]
area_portao_aberto = portao_aberto[y:y+height, x:x+width]
area_nova_imagem = nova_imagem[y:y+height, x:x+width]

# converter as imagens para escala de cinza
area_portao_fechado_gray = cv2.cvtColor(area_portao_fechado, cv2.COLOR_BGR2GRAY)
area_portao_aberto_gray = cv2.cvtColor(area_portao_aberto, cv2.COLOR_BGR2GRAY)
area_nova_imagem_gray = cv2.cvtColor(area_nova_imagem, cv2.COLOR_BGR2GRAY)

# calcular o histograma das imagens em escala de cinza
portao_fechado_hist = cv2.calcHist([area_portao_fechado_gray], [0], None, [256], [0, 256])
portao_aberto_hist = cv2.calcHist([area_portao_aberto_gray], [0], None, [256], [0, 256])
nova_imagem_hist = cv2.calcHist([area_nova_imagem_gray], [0], None, [256], [0, 256])

# comparar as imagens usando o método correlation
resultado_fechado = cv2.compareHist(portao_fechado_hist, nova_imagem_hist, cv2.HISTCMP_CORREL)
resultado_aberto = cv2.compareHist(portao_aberto_hist, nova_imagem_hist, cv2.HISTCMP_CORREL)

# determinar se a nova imagem corresponde a um portão aberto ou fechado
if resultado_fechado > resultado_aberto:
    print("O portão está fechado")
    estado = 1
else:
    print("O portão está aberto.")
    estado = 0

print("fechado: ", resultado_fechado)
print("Aberto: ", resultado_aberto)

# desenhar um retângulo vermelho na área de comparação
#cv2.rectangle(portao_fechado, (x, y), (x+width, y+height), (0, 0, 255), 2)
#cv2.rectangle(portao_aberto, (x, y), (x+width, y+height), (0, 0, 255), 2)
#cv2.rectangle(nova_imagem, (x, y), (x+width, y+height), (0, 0, 255), 2)

# mostrar as imagens com a área de comparação desenhada
#cv2.imshow("Portão fechado", portao_fechado)
#cv2.imshow("Portão aberto", portao_aberto)
#cv2.imshow("Nova imagem", nova_imagem)
#cv2.waitKey(0)
#cv2.destroyAllWindows()
