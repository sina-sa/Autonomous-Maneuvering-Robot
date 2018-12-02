""" run serial_test on the arduino """

import serial
import time

ser = serial.Serial('/dev/ttyUSB0',9600)
s = [0,1]
n = [0,1]
counter = 0
while True:
	
	ser.write('2')
        time.sleep(1)
        ser.write('4')
        time.sleep(1)