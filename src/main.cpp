
#include <Arduino.h>
#include "SIM800_COM.h"
#include "SoftwareSerial.h"


SIM800_COM sim800; 



void setup() {
  Serial.begin(9600);

  
}

void loop() {
  //sim800.replySerial();
   String Phone = sim800.readSMS();
  // delay(1000);
  Serial.println(Phone);

  delay(1000);
 
  
}
