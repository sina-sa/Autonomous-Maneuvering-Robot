#include <Servo.h>
 
Servo servo_left;
Servo servo_right;
 
void setup() {
servo_left.attach(9);
servo_right.attach(8);
}
 
void loop() {
//myservo.write(0);
//delay(1000);
servo_left.write(1700);
servo_right.write(1380);
//delay(1000);
}
