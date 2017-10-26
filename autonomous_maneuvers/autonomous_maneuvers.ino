#include <Servo.h>
#include <PID_v1.h>

Servo servo_left;
Servo servo_right;

// Ultrasonic sensors pins connected to Arduino
const int trigPin_left = 13;
const int echoPin_left = 12;
const int trigPin_right = 9;
const int echoPin_right = 8;
const int trigPin_front = 4;
const int echoPin_front = 7;
const int trigPin_front_left = 3;
const int echoPin_front_left = 2;
const int trigPin_front_right = 11;
const int echoPin_front_right = 10;

// Motor controller pins connected to Arduino
//const int ENA = 5;  // RIGHT
const int IN1 = A2;
const int IN2 = A5;
const int IN3 = A4;
const int IN4 = A3;
//const int ENB = 6; // LEFT

// Servo pins
const int servo_L = 6;
const int servo_R = 5;

// Distance value return from each ultrasonic sensor
long int distance_front;
long int distance_left;
long int distance_right;
long int distance_front_left;
long int distance_front_right;

// enum for the direction which the robot should go
enum State_turn
{
  Go_straight,
  Move_right,
  Move_left,
  Turn_right,
  Turn_left
};

// enum for the direction which the robot should go
enum State_robot
{
  Moving,
  Stop,
  T_junction,
  Turn_right_from_T,
  Turn_left_from_T,
  Ready_to_move
};

// This enum sets the correct pins for the motors to turn in a given direction
enum State_motors_dir
{
  Left_forward,
  Left_backward,
  Left_stop,
  Left_off,
  Right_forward,
  Right_backward,
  Right_stop,
  Right_off 
};

// Motor's speed (PWM signal)
int motorA_right_speed = 0;
int motorB_left_speed = 0;

// Threshold values for all the ultrasonic sensors
const int side_ultrasonic_threshold = 30;
const int ultrasonic_45_threshold = 4;
const int front_ultrasonic_threshold = 30;

signed int distance_change; // The difference between left and right sensors

int Should_Start; // Start the moving

const int max_speed = 255; // Maximum speed of the motors

const int max_speed_left = 1700; // Maximum speed of the left motor
const int max_speed_right = 1300; // Maximum speed of the right motor
const int min_speed = 1500; // Maximum speed of the right motor


int motor_speed_left = 1560;
int motor_speed_right = 1440;

int speed_change = 0; // The difference in speed to give each of the motors

enum State_turn direction_to_go; // Keep track of the previous states

enum State_robot What_To_Do; // Keep track of the robot's state. If it has to move or stop

void setup() 
{
  // Ultrasonic sensor setup
  pinMode(trigPin_left, OUTPUT);
  pinMode(echoPin_left, INPUT);
  pinMode(trigPin_right, OUTPUT);
  pinMode(echoPin_right, INPUT);
  pinMode(trigPin_front, OUTPUT);
  pinMode(echoPin_front, INPUT);
  pinMode(trigPin_front_left, OUTPUT);
  pinMode(echoPin_front_left, INPUT);
  pinMode(trigPin_front_right, OUTPUT);
  pinMode(echoPin_front_right, INPUT);

  // Motor controller setup
  pinMode (IN1, OUTPUT);
  pinMode (IN2, OUTPUT);
  pinMode (IN3, OUTPUT);
  pinMode (IN4, OUTPUT);
  //pinMode (ENA, OUTPUT);
  //pinMode (ENB, OUTPUT);
  
  // Serial port setup
  Serial.begin(9600); // Starts the serial communication

  servo_left.attach(servo_L);
  servo_right.attach(servo_R);

  motorA_right_speed = 1300;
  motorB_left_speed = 1700;
  delay(2); 
  servo_left.write(motorB_left_speed);
  servo_right.write(motorA_right_speed);
  delay(2);

  What_To_Do = Moving;
}

int ultrasonic_distance (int trigPin, int echoPin)
{
  long duration;
  int distanceCm;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH, 10000);
  distanceCm= duration*0.034/2;
  if(distanceCm == 0)   // In case of a timeout the pulsein function will return a 0 therefore we set the distance 100 which is more than the maxixmum value, manually.
    distanceCm = 100;

  return distanceCm;
}

int Find_motors_AB_speed(void)
{
  switch (direction_to_go)
  {
    case Move_right:

          if(speed_change > 120)
          {
            motorB_left_speed = 1650;
            motorA_right_speed = 1480;
          }
          else
          {
            motorA_right_speed = motor_speed_right + speed_change;
            motorB_left_speed = motor_speed_left + speed_change;
          }
          break;
          
    case Move_left:
          
          if(speed_change > 120)
          {
            motorB_left_speed = 1520;
            motorA_right_speed = 1350;
          }
          else
          {
            motorA_right_speed = motor_speed_right - speed_change;
            motorB_left_speed = motor_speed_left - speed_change;
          }
          break;

    case Go_straight:

          speed_change = 30;
          motorA_right_speed = motor_speed_right - speed_change;
          motorB_left_speed = motor_speed_left + speed_change;
          break;

    default:
          return 0;
          break;
  }
  return 1;
}

void Find_direction(void)
{
  if(distance_change > 0)
  {
    direction_to_go = Move_right;
  }
  else if(distance_change < 0)
  {
    distance_change = -distance_change;
    direction_to_go = Move_left;
  }
  else
    direction_to_go = Go_straight;
}

void loop() 
{
  //What_To_Do = Stop;

  // Read the value from all 5 ultrasonic sensors
  distance_left = ultrasonic_distance(trigPin_left, echoPin_left);
  distance_right = ultrasonic_distance(trigPin_right, echoPin_right);
  distance_front = ultrasonic_distance(trigPin_front, echoPin_front);
  distance_front_left = ultrasonic_distance(trigPin_front_left, echoPin_front_left);
  distance_front_right = ultrasonic_distance(trigPin_front_right, echoPin_front_right);
/*
  Serial.print("\n distance_left = ");
  Serial.print(distance_left);
  Serial.print("\n distance_right = ");
  Serial.print(distance_right);
  Serial.print("\n distance_front = ");
  Serial.print(distance_front);
  Serial.print("\n distance_front_left = ");
  Serial.print(distance_front_left);
  Serial.print("\n distance_front_right = ");
  Serial.print(distance_front_right);
  Serial.print("\n\n");

  delay(500);
*/

  if(What_To_Do == Moving)
  {
    if((distance_right > side_ultrasonic_threshold) && (distance_left > side_ultrasonic_threshold) && (distance_front < front_ultrasonic_threshold) && (distance_front_right > 20) && (distance_front_left > 20))  // At a T junction the robot needs to stop first
      What_To_Do = Stop;
      //What_To_Do = Moving;
  }
  
  if(What_To_Do == Moving)
  {
    if((distance_front_left >= ultrasonic_45_threshold) && (distance_front_right >= ultrasonic_45_threshold))
    { 
      distance_change = distance_right - distance_left;

      Find_direction();

      //speed_change = abs(PID_Output);
      speed_change = pow(distance_change, 2);
      speed_change = speed_change / 4;
  
      Find_motors_AB_speed();

      servo_left.write(motorB_left_speed);
      servo_right.write(motorA_right_speed);
    }
    else 
    {
      if(distance_front_left < ultrasonic_45_threshold)
      {
        motorA_right_speed = min_speed;
        motorB_left_speed = motor_speed_left;

        //Motors_movemet_direction(Right_forward);
        //Motors_movemet_direction(Left_forward);
        
        if((distance_front_left < ultrasonic_45_threshold) || (distance_left < 4))
        {
          do{
            motorA_right_speed = max_speed_right + 400;
            motorB_left_speed = min_speed;
            //Motors_movemet_direction(Right_backward);
            //Motors_movemet_direction(Left_off);
            
            servo_left.write(motorB_left_speed);
            servo_right.write(motorA_right_speed);         
          }while(ultrasonic_45_threshold > ultrasonic_distance(trigPin_front_left, echoPin_front_left));
          delay(150);
        }
      }
      else
      {
        motorA_right_speed = motor_speed_right;
        motorB_left_speed = min_speed;
        if((distance_front_right < ultrasonic_45_threshold) || (distance_right < 4))
        {
          do{
            motorA_right_speed = min_speed;
            motorB_left_speed = max_speed_left - 400;        
            //Motors_movemet_direction(Right_off);
            //Motors_movemet_direction(Left_backward);
            
            servo_left.write(motorB_left_speed);
            servo_right.write(motorA_right_speed);
          }while(ultrasonic_45_threshold > ultrasonic_distance(trigPin_front_right, echoPin_front_right));
          delay(150);
        }
      }
      servo_left.write(motorB_left_speed);
      servo_right.write(motorA_right_speed);
    }
  }
  else if(What_To_Do == Stop)
  {
    // The robot must stop and figure out where it is.
    motorA_right_speed = min_speed;
    motorB_left_speed = min_speed;
      
    servo_left.write(motorB_left_speed);
    servo_right.write(motorA_right_speed);
    delay(500);
    What_To_Do = T_junction;
  }
  else if(What_To_Do == T_junction)
  {
    // In this section the robot should scan for objects on the wall and look for a heat source.
    motorA_right_speed = min_speed;
    motorB_left_speed = min_speed;
      
    servo_left.write(motorB_left_speed);
    servo_right.write(motorA_right_speed);
    
    What_To_Do = Turn_right_from_T;
  }
  else if(What_To_Do == Turn_right_from_T)
  {
    distance_left = ultrasonic_distance(trigPin_left, echoPin_left);
    distance_right = ultrasonic_distance(trigPin_right, echoPin_right);
    distance_front = ultrasonic_distance(trigPin_front, echoPin_front);
    distance_front_left = ultrasonic_distance(trigPin_front_left, echoPin_front_left);
    distance_front_right = ultrasonic_distance(trigPin_front_right, echoPin_front_right);
    
    do{
      motorA_right_speed = 1440;
      motorB_left_speed = 1560;
      delay(10);
      servo_left.write(motorB_left_speed);
      servo_right.write(motorA_right_speed);
      delay(10);
    }while(ultrasonic_distance(trigPin_front, echoPin_front) > 6);

    servo_left.write(min_speed);
    servo_right.write(min_speed);
    delay(100);

    do{
      motorA_right_speed = 1560;
      motorB_left_speed = 1560;
      delay(10); 
      servo_left.write(motorB_left_speed);
      servo_right.write(motorA_right_speed);
      delay(10);
    }while(ultrasonic_distance(trigPin_left, echoPin_left) > 10);
    while(ultrasonic_distance(trigPin_right, echoPin_right) < 30);
    while(ultrasonic_distance(trigPin_front_left, echoPin_front_left) < 100);
    servo_left.write(min_speed);
    servo_right.write(min_speed);
    delay(100);

    motorA_right_speed = 1440;
    motorB_left_speed = 1560;
    delay(5);
    servo_left.write(motorB_left_speed);
    servo_right.write(motorA_right_speed);
    delay(800);
    servo_left.write(min_speed);
    servo_right.write(min_speed);

    What_To_Do = Moving;
  }
  else if(What_To_Do == Turn_left_from_T)
  {
    distance_left = ultrasonic_distance(trigPin_left, echoPin_left);
    distance_right = ultrasonic_distance(trigPin_right, echoPin_right);
    distance_front = ultrasonic_distance(trigPin_front, echoPin_front);
    distance_front_left = ultrasonic_distance(trigPin_front_left, echoPin_front_left);
    distance_front_right = ultrasonic_distance(trigPin_front_right, echoPin_front_right);
    
    do{
      motorA_right_speed = 1440;
      motorB_left_speed = 1560;
      delay(10);
      servo_left.write(motorB_left_speed);
      servo_right.write(motorA_right_speed);
      delay(10);
    }while(ultrasonic_distance(trigPin_front, echoPin_front) > 6);

    servo_left.write(min_speed);
    servo_right.write(min_speed);
    delay(100);

    do{
      motorA_right_speed = 1440;
      motorB_left_speed = 1440;
      delay(10); 
      servo_left.write(motorB_left_speed);
      servo_right.write(motorA_right_speed);
      delay(10);
    }while(ultrasonic_distance(trigPin_right, echoPin_right) > 10);
    while(ultrasonic_distance(trigPin_left, echoPin_left) < 30);
    while(ultrasonic_distance(trigPin_front_right, echoPin_front_right) < 100);
    servo_left.write(min_speed);
    servo_right.write(min_speed);
    delay(100);

    motorA_right_speed = 1440;
    motorB_left_speed = 1560;
    delay(5);
    servo_left.write(motorB_left_speed);
    servo_right.write(motorA_right_speed);
    delay(800);
    servo_left.write(min_speed);
    servo_right.write(min_speed);

    What_To_Do = Moving;
  }
  else
  {
    // This section is when the robot is done with the T_junction and now it has to either turn right or left.
    
  }
}
