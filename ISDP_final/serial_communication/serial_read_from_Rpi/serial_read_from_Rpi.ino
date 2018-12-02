/* run serial_test on raspberry pi */

const int ledPin = A1;
int temp = 0;

void setup()
{
  Serial.begin(9600);              //Starting serial communication
  pinMode(ledPin, OUTPUT);
}
  
void light(int n)
{
  for (int i = 0; i < n; i++) 
  {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
  }
}
  
void loop()
{
  if (Serial.available())
  {
    temp = Serial.read() ;  //– '0';
    temp -= '0';
    light(temp);
  }
  //delay(500);
}
