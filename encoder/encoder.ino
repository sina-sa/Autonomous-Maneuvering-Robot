
#define LEFT_ENCODER_CHA 2
#define LEFT_ENCODER_CHB 4

#define RIGHT_ENCODER_CHA 3
#define RIGHT_ENCODER_CHB 7



const int QEM [16] = {0,-1,1,2,1,0,2,-1,-1,2,0,1,2,1,-1,0}; // Quadrature Encoder Matrix


// left encoder variables
volatile unsigned char left_old, left_new = 0;
volatile long left_position = 0; 

volatile float time1, time2;

volatile float time3, time4;

//right encoder variables
volatile unsigned char right_old, right_new = 0; 
volatile long right_position = 0; 



void setup()
{

  
  pinMode(LEFT_ENCODER_CHA, INPUT);
  pinMode(LEFT_ENCODER_CHB, INPUT);


  attachInterrupt(digitalPinToInterrupt(LEFT_ENCODER_CHA), left_encoder_interrupt, RISING);   // interrupt on pin 2;
  //attachInterrupt(digitalPinToInterrupt(RIGHT_ENCODER_CHA), right_encoder_interrupt, RISING); // interrupt on pin 2;
  

  Serial.begin(9600);

}


int dir = 0;

void loop(){

  
  
  /*Serial.print("\t");
  Serial.print("left_position");
  Serial.print("\t");
  Serial.println(left_position);
  
  
  Serial.print("\t");
  Serial.print("right_position");
  Serial.print("\t");
  Serial.print(right_position);*/

  Serial.print("\nRIGHT_ENCODER_CHA = ");
  Serial.print(digitalRead(RIGHT_ENCODER_CHA));
  Serial.print("\nRIGHT_ENCODER_CHB = ");
  Serial.print(digitalRead(RIGHT_ENCODER_CHB));
  
  
}
      

// 

void left_encoder_interrupt()
{
  while((digitalRead(LEFT_ENCODER_CHA) == 1) || (digitalRead(LEFT_ENCODER_CHB) == 1))
  {
    if(digitalRead(LEFT_ENCODER_CHA) == 1)   time1 = micros();
    if(digitalRead(LEFT_ENCODER_CHB) == 1)   time2 = micros();
  }
  if(time2 > time1)  left_position++;
  else               left_position--;
}

void right_encoder_interrupt()
{
  while((digitalRead(RIGHT_ENCODER_CHA) == 1) || (digitalRead(RIGHT_ENCODER_CHB) == 1))
  {
    if(digitalRead(RIGHT_ENCODER_CHA) == 1)   time3 = micros();
    if(digitalRead(RIGHT_ENCODER_CHB) == 1)   time4 = micros();
  }
  if(time4 > time3)  right_position++;
  else               right_position--;
}



