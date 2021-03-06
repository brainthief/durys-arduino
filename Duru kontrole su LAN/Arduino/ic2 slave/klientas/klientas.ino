#include <Wire.h>

#define I2CAddress 9
#define ledPin 13

void receiveString(int bytes) {
  String text = "";
  while (Wire.available()) { 
    char c = Wire.read(); // receive a byte as character
    text += c;
  }
  Serial.print("Printing: ");
  Serial.println(text);

  manageLed(text);
}

void manageLed(String cmd) {
  if(cmd == "ON")
    digitalWrite(ledPin,HIGH);
  else if(cmd == "OFF")
    digitalWrite(ledPin,LOW);
  else
    Serial.println("Unknown command: " + cmd);
}

void setup() {
  Serial.begin(9600);
  Serial.println("Slave here");
  
  // Start the I2C Bus as Slave on address 9
  Wire.begin(I2CAddress); 

   // Attach a function to trigger when something is received.
  Wire.onReceive(receiveString);

  // LED
  pinMode(ledPin,OUTPUT);
  digitalWrite(ledPin,LOW);
}

void loop() {
  delay(10);
}

