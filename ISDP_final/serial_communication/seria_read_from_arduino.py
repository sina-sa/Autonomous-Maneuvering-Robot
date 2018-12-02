""" run serial_test on the arduino """

import serial
import RPi.GPIO as GPIO
import time

ser = serial.Serial('/dev/ttyUSB0',9600)

while True:
	read_serial=ser.readline()
	n = int (read_serial,10)
	s = str(n)
	if n == 1:
            print "LED on"
        else:
            print "LED off"
	print s
	print read_serial
