/********************************************************
 * PID Adaptive Tuning Example
 * One of the benefits of the PID library is that you can
 * change the tuning parameters at any time.  this can be
 * helpful if we want the controller to be aggressive at some
 * times, and conservative at others.   in the example below
 * we set the controller to use Conservative Tuning Parameters
 * when we're near setpoint and more aggressive Tuning
 * Parameters when we're farther away.
 ********************************************************/

#include <PID_v1.h>

// Ultrasonic sensors pins connected to Arduino
const int trigPin_left = 13; // 13
const int echoPin_left = 12; // 12
const int trigPin_right = 9; // 9
const int echoPin_right = 8;

// Motor controller pins connected to Arduino
const int ENA = 5;  // RIGHT
const int IN1 = A2;
const int IN2 = A5;
const int IN3 = A4;
const int IN4 = A3;
const int ENB = 6; // LEFT

//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Define the aggressive and conservative Tuning Parameters
double aggKp=4, aggKi=0.2, aggKd=1;
double consKp=1, consKi=0.05, consKd=0.25;

//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint, consKp, consKi, consKd, DIRECT);

void setup()
{
  pinMode(trigPin_left, OUTPUT);
  pinMode(echoPin_left, INPUT);
  pinMode(trigPin_right, OUTPUT);
  pinMode(echoPin_right, INPUT);

  // Motor controller setup
  pinMode (IN1, OUTPUT);
  pinMode (IN2, OUTPUT);
  pinMode (IN3, OUTPUT);
  pinMode (IN4, OUTPUT);
  pinMode (ENA, OUTPUT);
  pinMode (ENB, OUTPUT);
  
  
  Setpoint = 0;

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
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

void loop()
{
  long int distance_left;
  long int distance_right;
  signed int distance_change;

  distance_left = ultrasonic_distance(trigPin_left, echoPin_left);
  distance_right = ultrasonic_distance(trigPin_right, echoPin_right);
  Input = distance_right - distance_left;

  double gap = abs(Setpoint-Input); //distance away from setpoint
  if(gap<10)
  {  //we're close to setpoint, use conservative tuning parameters
    myPID.SetTunings(consKp, consKi, consKd);
  }
  else
  {
     //we're far from setpoint, use aggressive tuning parameters
     myPID.SetTunings(aggKp, aggKi, aggKd);
  }

  myPID.Compute();

  digitalWrite (IN1, HIGH);
  digitalWrite (IN2, LOW);
  digitalWrite (IN3, HIGH);
  digitalWrite (IN4, LOW);
  
  if(distance_change > 0)
  {
    analogWrite(ENA,Output);
    analogWrite(ENB,0);
  }
  else if(distance_change < 0)
  {
    analogWrite(ENA,0);
    analogWrite(ENB,Output);
  }
  else
  {
    analogWrite(ENA,200);
    analogWrite(ENB,200);
  }
}
