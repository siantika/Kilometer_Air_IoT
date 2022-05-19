
#include <Arduino.h>
#include "SIM800_COM.h"
#include "SoftwareSerial.h"


SIM800_COM sim800; 
String PHONE="";


void setup() {
  Serial.begin(9600);
  Serial.println("Insert your phone number (send the message) :");

  
}

void loop() {
  
  PHONE += sim800.getPhone();
  Serial.println(PHONE);
  sim800.sendSMS("Selamat Pagi, Berikut adalah tagihan minggu ini:", PHONE);
  while(1);
  
 
  
}

