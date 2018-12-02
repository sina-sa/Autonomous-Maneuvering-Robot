""" run serial_test on the arduino """

import serial
import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(18,GPIO.OUT)

ser = serial.Serial('/dev/ttyUSB0',9600)
s = [0,1]
n = [0,1]
while True:
	read_serial=ser.readline()
	n[0] = int (read_serial,10)
	s[0] = str(n[0])
	if n[0] == 1:
            print "LED on"
            GPIO.output(18,GPIO.HIGH)
        else:
            print "LED off"
            GPIO.output(18,GPIO.LOW)
	print s[0]
	print read_serial
