/* run serial_test on raspberry pi */

char dataString[50] = {0};
int a =0; 

void setup() {
Serial.begin(9600);              //Starting serial communication
}
  
void loop() {
  a++;                          // a value increase every loop
  if(a == 10)
    a = 0;
  sprintf(dataString,"%1d",a); // convert a value to hexa 
  Serial.println(dataString);   // send the data
  delay(500);                  // give the loop some break
}
