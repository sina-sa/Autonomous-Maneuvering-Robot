# import the necessary packages
from imutils import contours
from picamera.array import PiRGBArray
from picamera import PiCamera
import time
import serial
import numpy as np
import imutils
import cv2

# initialize the camera and grab a reference to the raw camera capture
camera = PiCamera()
camera.resolution = (512, 512)
camera.framerate = 25
rawCapture = PiRGBArray(camera, size=(512, 512))

# allow the camera to warmup
time.sleep(0.1)

ser = serial.Serial('/dev/ttyUSB0',9600)

shape_C = 0
shape_R = 0
shape_D = 0

while True : 
    while True :
        read_serial=ser.readline()
        read_serial_num = int (read_serial, 10)
        if read_serial_num == 1:
            break
    
    # capture frames from the camera
    for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
            # grab the raw NumPy array representing the image, then initialize the timestamp
            # and occupied/unoccupied text
            image = frame.array
            gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
            gray = cv2.GaussianBlur(gray, (7, 7), 0)

            # perform edge detection, then perform a dilation + erosion to
            # close gaps in between object edges
            edged = cv2.Canny(gray, 20, 100) #(gray, 50, 100)
            edged = cv2.dilate(edged, None, iterations=1)
            #cv2.imshow('edged2', edged)
             
            # find contours in the edge map
            cnts = cv2.findContours(edged.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
            cnts = cnts[0] if imutils.is_cv2() else cnts[1]

            # loop over the contours individually
            for c in cnts:
                    # if the contour is not sufficiently large, ignore it
                    if cv2.contourArea(c) < 1500:
                            continue

                    # compute the rotated bounding box of the contour
                    box = cv2.minAreaRect(c)
                    (shape_W,shape_H) = box[1]
                    shape_angle = box[2]
                    
                    if abs(shape_W - shape_H) < 10:
                        #print "circle"
                        shape_C += 1
                    else:
                        if (abs(shape_angle) < 20) or (abs(shape_angle) > 70):
                            #print "rectangle"
                            shape_R += 1
                        else:
                            #print "diamond"
                            shape_D += 1
                        
                    #cv2.imshow('frame', image)
            rawCapture.truncate(0)
            if (shape_C > 15) or (shape_R > 15) or (shape_D > 15):
                if shape_C > 15:
                    print "shape_Circle"
                    ser.write('3')
                elif shape_R > 15:
                    print "shape_Recectangle"
                    ser.write('2')
                else:
                    print "shape_Diamond"
                    ser.write('2')
                #print "C " + str(shape_C)
                #print "R " + str(shape_R)
                #print "D " + str(shape_D)
                shape_C = 0
                shape_R = 0
                shape_D = 0
                break
            
            if cv2.waitKey(10) & 0xFF == ord('q'):
    		cv2.destroyAllWindows()
    		camera.close()
    		break  

camera.close()
cv2.destroyAllWindows()