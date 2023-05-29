import camera
import network
import machine
import time
import gc
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

    if request.startswith('GET / HTTP/1.1'):
        response = 'HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n'
        response += '<html><body>'
        response += '<h1>Hello, ESP32-CAM!</h1>'
        response += '<img src="/photo.jpg">'
        response += '</body></html>'
        client_socket.send(response.encode('utf-8'))
    # Aqui você pode processar a requisição e gerar uma resposta
#     response = 'HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\nHello, ESP32-CAM!'
#     client_socket.send(response.encode('utf-8'))
    
    elif request.startswith('GET /photo HTTP/1.1'):
        # Lê a foto armazenada na memória do ESP32
        with open('/photo.jpg', 'rb') as file:
            while True:
                data = file.read(1024)
                if not data:
                    break
                client_socket.write(data)
    
    
    client_socket.close()

def take_photo():
    print("Tirando foto...")
    try:
        led = machine.Pin(4, machine.Pin.OUT)
        led.on()
        camera.init()
        img = camera.capture()
        led.off()
        filename = '/photo.jpg'
        with open(filename, 'wb') as f:
            f.write(img)
        print("Foto tirada e salva em:", filename)
        
    finally:
        camera.deinit()
        led.off()
        gc.collect()
        
take_photo()
# Loop principal
while True:
    client, addr = s.accept()
    print('Conexão de', addr)
    handle_request(client)


