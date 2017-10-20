
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

enum State_turn State_array[101]; // Keep track of the previous states

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

  // setting the initial value for the whole array
  for(int i=100; i>=0; i--)
  {
    State_array[i] = Go_straight;
  }
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

int find_change_speed (void)
{
  if(State_array[2] == State_array[0])
  {
    speed_change = speed_change + 5;
    if(State_array[20] == State_array[0])
    {
      speed_change = speed_change + 10;
      if(State_array[40] == State_array[0])
      {
        speed_change = speed_change + 20;
        if(State_array[60] == State_array[0])
        {
          speed_change = speed_change + 35;
          if(State_array[80] == State_array[0])
          {
            speed_change = speed_change + 55;
            if(State_array[100] == State_array[0])
            {
              speed_change = speed_change + 80;
            }
          }
        }
      }
    }
  }
  return speed_change;
}


void loop() 
{
  Should_Start = 1;

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
  {
    Should_Start = 0;
  
    // FORWARD A RIGHT
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    // FORWARD B LEFT
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
      
    analogWrite(ENA, 0);
    analogWrite(ENB, 0);   
    while(1);
  }
  /*if(distance_left > 20)  // Avoiding overshoot in the value of ultrasonic sensors
    distance_left = 40;
  if(distance_right > 20)
    distance_right = 40;*/

  if(Should_Start)
  {
    if((distance_front_left >= ultrasonic_45_threshold) && (distance_front_right >= ultrasonic_45_threshold))
    { 
      distance_change = distance_right - distance_left;
      if(distance_change > 0)
      {
        State_array[0] = Move_right;
      }
      else if(distance_change < 0)
      {
        distance_change = -distance_change;
        State_array[0] = Move_left;
      }
      else
        State_array[0] = Go_straight;

      //distance_change = distance_change * 0.5;
      speed_change = pow(distance_change, 2);
  
      switch (State_array[0])
      {
        case Move_right:

              //speed_change = find_change_speed();
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
              
              //speed_change = find_change_speed();
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
              //
              //speed_change = find_change_speed();
              speed_change = 30;
              motorA_right_speed = motor_speed + speed_change;
              motorB_left_speed = motor_speed + speed_change;
              break;
  
        default:
              Serial.print("\n\n Unknown case !!! \n\n");
      }
      
      for(int i=99; i>=0; i--)
      {
        State_array[i+1] = State_array[i];
      }
      
      // FORWARD A RIGHT
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      // FORWARD B LEFT
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
      
      analogWrite(ENA, motorA_right_speed);
      analogWrite(ENB, motorB_left_speed);
    }
    else 
    {
      if(distance_front_left < ultrasonic_45_threshold)
      {
        motorA_right_speed = 0;
        motorB_left_speed = motor_speed;
        // FORWARD A RIGHT
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        // FORWARD B LEFT
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        if((distance_front_left < ultrasonic_45_threshold) || (distance_left < 5))
        {
          do{
            motorA_right_speed = motor_speed;
            motorB_left_speed = 0;
            // FORWARD A RIGHT
            digitalWrite(IN1, HIGH);
            digitalWrite(IN2, LOW);
            // FORWARD B LEFT
            digitalWrite(IN3, LOW);
            digitalWrite(IN4, LOW);
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
            // FORWARD A RIGHT
            digitalWrite(IN1, LOW);
            digitalWrite(IN2, LOW);
            // FORWARD B LEFT
            digitalWrite(IN3, HIGH);
            digitalWrite(IN4, LOW);
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
  //delay(10);
}
