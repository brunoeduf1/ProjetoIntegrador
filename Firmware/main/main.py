import machine
import time
import uctypes
from machine import UART

lib = uctypes.open("main.cpp")
led = machine.Pin(2, machine.Pin.OUT)
while True:
     led.value(1)
     time.sleep(1)
     led.value(0)
     time.sleep(1)