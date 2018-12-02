
// Motor controller pins connected to Arduino
const int ENA = 5;  // RIGHT
const int IN1 = A2;
const int IN2 = A5;
const int IN3 = A4;
const int IN4 = A3;
const int ENB = 6; // LEFT

int E1 = 5;     //Right Motor Power
int E2 = 6;     //Left Motor Power
//int M1 = 4;    //Right Motor Direction
//int M2 = 7;    //Left Motor Direction
const byte encoder0pinA = 2; //Interrupt 0
const byte encoder0pinB = 12;
const byte encoder1pinA = 3; //Interrupt 1
const byte encoder1pinB = 13; 
byte overflow;
byte encoder0PinALast;
byte encoder1PinALast;
int duration0;//Encoder0 number of pulses
int duration00;
int duration1;//Encoder1 number of pulses
int duration11;
boolean Direction0;
boolean Direction1;

int SetPoint = 0; // 0 would be straight ahead
double Kp = 25; // needs tunning
int error = 0;
int speed = 100; // set the speed of which the vechicle should move
int output = 0;

void setup()
{  
  //pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  //pinMode(7, OUTPUT);
  pinMode (IN1, OUTPUT);
  pinMode (IN2, OUTPUT);
  pinMode (IN3, OUTPUT);
  pinMode (IN4, OUTPUT);
  
  Serial.begin(9600);//Initialize the serial port
  EncoderInit();//Initialize the module 
  timer_init();
  
}

void loop()
{
//  Serial.print("Left Pulse:");
//  Serial.println(duration00);
//  Serial.print("Right Pulse:");
//  Serial.println(duration11);
  Preg(duration0, duration1); //Go Forward
  //digitalWrite(M1,LOW);
  //digitalWrite(M2,LOW);
  // FORWARD A RIGHT
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      // FORWARD B LEFT
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
  delay(2500);
  Preg(duration0, duration1); //Go Backward
  //digitalWrite(M1,HIGH);
  //digitalWrite(M2,HIGH);
  // FORWARD A RIGHT
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      // FORWARD B LEFT
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
  delay(2500);
  digitalWrite(E1,LOW); //STOP!!!!!!
  digitalWrite(E2,LOW);
  delay(2500);
}
ISR(TIMER1_OVF_vect)
{
  overflow++;
  if(overflow>=10)
  {
  duration00=duration0;
  duration0=0;
  duration11=duration1;
  duration1=0;
  overflow=0;
  }
}
void Preg(int SpeedL, int SpeedR)
{
  noInterrupts(); // Disable interrupts, no need to slow down the P regulator
  error = ((SpeedL-SpeedR)-SetPoint);
  output = (Kp*error);
  if(error >= 0) // its turning left of the setpoint, reduce E1 (right motor) 
  {
    analogWrite(E2, speed);
    analogWrite(E1, (speed-output)); // Subtract the error value multiplied by Kp from E1
  }
  if(error < 0) // turning right of our setpoint, reduce E0
  {
    analogWrite(E2, (speed+output)); // This time we add the error since its negative
    analogWrite(E1, speed);
  }
  interrupts(); //Enable interrupts again
}

void timer_init(void)
{
  TIMSK1 |= (1<<TOIE1); // Enable Timer1 overflow interrupt at 16MHz = 16 000 000 / 2^16 = 244Hz
}
void EncoderInit()
{
  Direction0 = true;
  Direction1 = true;  
  pinMode(encoder0pinB,INPUT);
  attachInterrupt(0, wheelSpeed0, CHANGE);
  pinMode(encoder1pinB,INPUT);  
  attachInterrupt(1, wheelSpeed1, CHANGE);
}


void wheelSpeed0()
{
  //Encoder 0 Code
  int Lstate = digitalRead(encoder0pinA);
  if((encoder0PinALast == LOW) && Lstate==HIGH)
  {
    int val0 = digitalRead(encoder0pinB);
    if(val0 == LOW && Direction0)
    {
      Direction0 = false; //Reverse
    }
    else if(val0 == HIGH && !Direction0)
    {
      Direction0 = true;  //Forward
    }
  }
  encoder0PinALast = Lstate;

  if(!Direction0)  duration0++;
  else  duration0++;
}

void wheelSpeed1()
{
  //Encoder 1 Code
  int Lstate1 = digitalRead(encoder1pinA);
  if((encoder1PinALast == LOW) && Lstate1==HIGH)
  {
    int val1 = digitalRead(encoder1pinB);
    if(val1 == LOW && Direction1)
    {
      Direction1 = false; //Reverse
    }
    else if(val1 == HIGH && !Direction1)
    {
      Direction1 = true;  //Forward
    }
  }
  encoder1PinALast = Lstate1;

  if(!Direction1)  duration1++;
  else  duration1++;
}
