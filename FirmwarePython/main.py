import camera
import network
import machine
import time
import gc

def wifi_connect(): 
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    if not wlan.isconnected():
        print('connecting to network...')
        wlan.connect('IMMF', 'Marcelle2017')
        while not wlan.isconnected():
            pass
    print('network config:', wlan.ifconfig())

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

wifi_connect()
take_photo()


