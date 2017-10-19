
#include <PinChangeInt.h>
#include <PID_v1.h>
#define encodPinA1      2                       // Quadrature encoder A pin
#define encodPinB1      12                       // Quadrature encoder B pin
#define E1              6                       // PWM outputs to L298N H-Bridge motor driver module
#define M1              A4
#define M2              A3

int turning_right = 1;
double encoderPos_old = 0;

double kp = 5 , ki = 1 , kd = 0.01;             // modify for optimal performance
double input = 0, output = 0, setpoint = 0;
long temp;
volatile long encoderPos = 0;
PID myPID(&input, &output, &setpoint, kp, ki, kd, DIRECT);  // if motor will only run at full speed try 'REVERSE' instead of 'DIRECT'

void setup() {
  pinMode(encodPinA1, INPUT_PULLUP);                  // quadrature encoder input A
  pinMode(encodPinB1, INPUT_PULLUP);                  // quadrature encoder input B
  pinMode (M1, OUTPUT);
  pinMode (M2, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(2), encoder, RISING);               // update encoder position
  //TCCR1B = TCCR1B & 0b11111000 | 1;                   // set 31KHz PWM to prevent motor noise
  myPID.SetMode(AUTOMATIC);
  myPID.SetSampleTime(1);
  myPID.SetOutputLimits(-255, 255);
  Serial.begin (9600);                              // for debugging
}

void loop() {
  for(int i=0; i<2; i++)
  {
    setpoint = i*100;                       // modify to fit motor and encoder characteristics, potmeter connected to A0
    input = encoderPos ;                                // data from encoder
    Serial.println(encoderPos);                      // monitor motor position
    while (!myPID.Compute());                                    // calculate new output
    pwmOut(output);                                     // drive L298N H-Bridge module
  }
}

void pwmOut(int out) {                                // to H-Bridge board
  if (out > 0) {
    analogWrite(E1, out);                             // drive motor CW
    // FORWARD B LEFT
      digitalWrite(M1, LOW);
      digitalWrite(M2, HIGH);
      turning_right = 1;
  }
  else {
    analogWrite(E1, abs(out));                        // drive motor CCW
    // FORWARD B LEFT
      digitalWrite(M1, HIGH);
      digitalWrite(M2, LOW);
      turning_right = 0;
  }
}

void encoder()  {                                     // pulse and direction, direct port reading to save cycles
  if (turning_right)    encoderPos++;             // if(digitalRead(encodPinB1)==HIGH)   count ++;
  else                  encoderPos--;             // if(digitalRead(encodPinB1)==LOW)   count --;
}
