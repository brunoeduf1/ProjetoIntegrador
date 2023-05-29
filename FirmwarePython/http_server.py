import usocket as socket
import network

# Configuração da rede Wi-Fi
ssid = 'IMMF'
password = 'Marcelle2017'

# Configuração do servidor web
host = '0.0.0.0'
port = 80 

# Inicializa a conexão Wi-Fi
wlan = network.WLAN(network.STA_IF)
wlan.active(True)
if not wlan.isconnected():
    print('connecting to network...')
    wlan.connect('IMMF', 'Marcelle2017')
    while not wlan.isconnected():
        pass
print('network config:', wlan.ifconfig())

# Configura o servidor web
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((host, port))
s.listen(1)

# Função para lidar com as requisições
def handle_request(client_socket):
    request = client_socket.recv(1024)
    request = request.decode('utf-8')

    # Aqui você pode processar a requisição e gerar uma resposta
    response = 'HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\nHello, ESP32-CAM!'
    client_socket.send(response.encode('utf-8'))
    client_socket.close()

# Loop principal
while True:
    client, addr = s.accept()
    print('Conexão de', addr)
    handle_request(client)

