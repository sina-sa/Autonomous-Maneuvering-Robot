#include <dht.h>

dht DHT;

#define DHT11_PIN 7

void setup(){
  Serial.begin(9600);
}

void loop()
{
  int chk = DHT.read11(DHT11_PIN);
  Serial.print(chk);       // print as an ASCII-encoded decimal - same as "DEC"
  Serial.print("\n");
  delay(500);
}

