
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
const int ENA = 5;  // RIGHT
const int IN1 = A2;
const int IN2 = A5;
const int IN3 = A4;
const int IN4 = A3;
const int ENB = 6; // LEFT

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
const int ultrasonic_45_threshold = 7;
const int front_ultrasonic_threshold = 30;

signed int distance_change; // The difference between left and right sensors

int Should_Start; // Start the moving

const int max_speed = 255; // Maximum speed of the motors

int motor_speed = 195;

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
  pinMode (ENA, OUTPUT);
  pinMode (ENB, OUTPUT);
  
  // Serial port setup
  Serial.begin(9600); // Starts the serial communication

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

          if(speed_change > 60)
          {
            motorB_left_speed = max_speed;
            motorA_right_speed = 0;
          }
          else
          {
            motorA_right_speed = motor_speed - speed_change;
            motorB_left_speed = motor_speed + speed_change;
          }
          break;
          
    case Move_left:
          
          if(speed_change > 60)
          {
            motorB_left_speed = 0;
            motorA_right_speed = max_speed;
          }
          else
          {
            motorA_right_speed = motor_speed + speed_change;
            motorB_left_speed = motor_speed - speed_change;
          }
          break;

    case Go_straight:

          speed_change = 30;
          motorA_right_speed = motor_speed + speed_change;
          motorB_left_speed = motor_speed + speed_change;
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


int Motors_movemet_direction(enum State_motors_dir motor_dir)
{
  switch (motor_dir)
  {
    case Left_forward:
          digitalWrite(IN3, LOW);
          digitalWrite(IN4, HIGH);
          break; 
    case Left_backward:
          digitalWrite(IN3, HIGH);
          digitalWrite(IN4, LOW);
          break;
    case Left_stop:
          digitalWrite(IN3, HIGH);
          digitalWrite(IN4, HIGH);
          break; 
    case Left_off:
          digitalWrite(IN3, LOW);
          digitalWrite(IN4, LOW);
          break;
    case Right_forward:
          digitalWrite(IN1, LOW);
          digitalWrite(IN2, HIGH);
          break;
    case Right_backward:
          digitalWrite(IN1, HIGH);
          digitalWrite(IN2, LOW);
          break;
    case Right_stop:
          digitalWrite(IN1, HIGH);
          digitalWrite(IN2, HIGH);
          break;
    case Right_off:
          digitalWrite(IN1, LOW);
          digitalWrite(IN2, LOW);
          break;
    default:
          return 0;
          break;
  }
  return 1;
}

void loop() 
{
  //What_To_Do = Moving;

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

  //if((distance_front_right > 20) && (distance_front_left > 20) && (distance_front < 40))

  if((distance_right > side_ultrasonic_threshold) && (distance_left > side_ultrasonic_threshold) && (distance_front < front_ultrasonic_threshold) && (distance_front_right > 20) && (distance_front_left > 20))  // At a T junction the robot needs to stop first
    What_To_Do = Stop;

  if(What_To_Do == Moving)
  {
    if((distance_front_left >= ultrasonic_45_threshold) && (distance_front_right >= ultrasonic_45_threshold))
    { 
      distance_change = distance_right - distance_left;

      Find_direction();

      speed_change = pow(distance_change, 2);
  
      Find_motors_AB_speed();

      Motors_movemet_direction(Right_forward);
      Motors_movemet_direction(Left_forward);
      
      analogWrite(ENA, motorA_right_speed);
      analogWrite(ENB, motorB_left_speed);
    }
    else 
    {
      if(distance_front_left < ultrasonic_45_threshold)
      {
        motorA_right_speed = 0;
        motorB_left_speed = motor_speed;

        Motors_movemet_direction(Right_forward);
        Motors_movemet_direction(Left_forward);
        
        if((distance_front_left < ultrasonic_45_threshold) || (distance_left < 5))
        {
          do{
            motorA_right_speed = motor_speed;
            motorB_left_speed = 0;
            Motors_movemet_direction(Right_backward);
            Motors_movemet_direction(Left_off);
            
            analogWrite(ENA, motorA_right_speed);
            analogWrite(ENB, motorB_left_speed);
          }while(ultrasonic_45_threshold > ultrasonic_distance(trigPin_front_left, echoPin_front_left));
          delay(150);
        }
      }
      else
      {
        motorA_right_speed = motor_speed;
        motorB_left_speed = 0;
        if((distance_front_right < ultrasonic_45_threshold) || (distance_right < 5))
        {
          do{
            motorA_right_speed = 0;
            motorB_left_speed = motor_speed;
            
            Motors_movemet_direction(Right_off);
            Motors_movemet_direction(Left_backward);
            
            analogWrite(ENA, motorA_right_speed);
            analogWrite(ENB, motorB_left_speed);
          }while(ultrasonic_45_threshold > ultrasonic_distance(trigPin_front_right, echoPin_front_right));
          delay(150);
        }
      }
      analogWrite(ENA, motorA_right_speed);
      analogWrite(ENB, motorB_left_speed);
    }
  }
  else if(What_To_Do == Stop)
  {
    // The robot must stop and figure out where it is.
    Motors_movemet_direction(Right_stop);
    Motors_movemet_direction(Left_stop);
      
    analogWrite(ENA, 255);
    analogWrite(ENB, 255);
    delay(50);
    What_To_Do = T_junction;
  }
  else if(What_To_Do == T_junction)
  {
    // In this section the robot should scan for objects on the wall and look for a heat source.
    Motors_movemet_direction(Right_off);
    Motors_movemet_direction(Left_off);
      
    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
    while(1);
    //What_To_Do = Ready_to_move;
  }
  else
  {
    // This section is when the robot is done with the T_junction and now it has to either turn right or left.
    
  }
}
