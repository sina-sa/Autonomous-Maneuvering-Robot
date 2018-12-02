#importing modules
from picamera.array import PiRGBArray
from picamera import PiCamera
import time
import cv2   
import numpy as np

#capturing video through webcam
#cap=cv2.VideoCapture(0)

# initialize the camera and grab a reference to the raw camera capture
camera = PiCamera()
camera.resolution = (512, 512)
camera.framerate = 25
rawCapture = PiRGBArray(camera, size=(512, 512))

# allow the camera to warmup
time.sleep(0.1)

area_rgby = [0, 0, 0, 0]

for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
	# grab the raw NumPy array representing the image, then initialize the timestamp
	# and occupied/unoccupied text
	img = frame.array
	#_, img = cap.read()
	    
	#converting frame(img i.e BGR) to HSV (hue-saturation-value)

	hsv=cv2.cvtColor(img,cv2.COLOR_BGR2HSV)

	#definig the range of red color
	red_lower=np.array([169,100,100],np.uint8)
	red_upper=np.array([189,255,255],np.uint8)

	#defining the Range of Blue color
	blue_lower=np.array([90,100,100],np.uint8)
	blue_upper=np.array([110,255,255],np.uint8)
	
	#defining the Range of yellow color
	yellow_lower=np.array([20,100,100],np.uint8)
	yellow_upper=np.array([50,255,255],np.uint8)

	#defining the Range of green color
	green_lower=np.array([51,100,100],np.uint8)
	green_upper=np.array([70,255,255],np.uint8)

	#finding the range of red,blue and yellow color in the image
	red=cv2.inRange(hsv, red_lower, red_upper)
	blue=cv2.inRange(hsv,blue_lower,blue_upper)
	yellow=cv2.inRange(hsv,yellow_lower,yellow_upper)
	green=cv2.inRange(hsv,green_lower,green_upper)
	
	cv2.imshow("red",red)
	cv2.imshow("blue",blue)
	cv2.imshow("yellow",yellow)
	cv2.imshow("green",green)
	
	#Morphological transformation, Dilation  	
	kernal = np.ones((5 ,5), "uint8")

        red=cv2.dilate(red, kernal)
	#res=cv2.bitwise_and(img, img, mask = red)
        
	blue=cv2.dilate(blue,kernal)
	#res1=cv2.bitwise_and(img, img, mask = blue)

	yellow=cv2.dilate(yellow,kernal)
	#res2=cv2.bitwise_and(img, img, mask = yellow)

	green=cv2.dilate(green,kernal)
	#res3=cv2.bitwise_and(img, img, mask = green) 
        
        area_rgby[0] = 0 #0 is red
        area_rgby[1] = 0 #1 is blue
        area_rgby[2] = 0 #2 is yellow
        area_rgby[3] = 0 #3 is green
	#Tracking the Red Color
	(_,contours,hierarchy)=cv2.findContours(red,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
	for pic, contour in enumerate(contours):
		area = cv2.contourArea(contour)
		if area > area_rgby[0]:
                    area_rgby[0] = area
			
	#Tracking the Blue Color
	(_,contours,hierarchy)=cv2.findContours(blue,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
	for pic, contour in enumerate(contours):
		area = cv2.contourArea(contour)
		if area > area_rgby[1]:
                    area_rgby[1] = area
		
	#Tracking the yellow Color
	(_,contours,hierarchy)=cv2.findContours(yellow,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
	for pic, contour in enumerate(contours):
		area = cv2.contourArea(contour)
		if area > area_rgby[2]:
                    area_rgby[2] = area

	#Tracking the green Color
	(_,contours,hierarchy)=cv2.findContours(green,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
	for pic, contour in enumerate(contours):
		area = cv2.contourArea(contour)
		if area > area_rgby[3]:
                    area_rgby[3] = area
       
        messege_list = ['RED', 'BLUE', 'YELLOW', 'GREEN']
        temp = area_rgby[0]
        i = 0
        while i < 4:
            if area_rgby[i] >= temp:
                temp = area_rgby[i]
                messege = messege_list[i]
            i += 1
        
        print messege + str(temp)
    	
    	cv2.imshow("Color Tracking",img)
    	# clear the stream in preparation for the next frame
	rawCapture.truncate(0)
    	#cv2.imshow("red",res) 	
    	if cv2.waitKey(10) & 0xFF == ord('q'):
    		#cap.release()
    		cv2.destroyAllWindows()
    		break  
