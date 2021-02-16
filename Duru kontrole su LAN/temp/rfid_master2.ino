/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read new NUID from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 * 
 * Example sketch/program showing how to the read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino SPI interface.
 * 
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the type, and the NUID if a new card has been detected. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 * 
 * @license Released into the public domain.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h> //i2c bibiiotka
#define I2CAddress 9 // massterio adresas
String text; //zinute kuria siusime per serial

#define SS_PIN 10
#define RST_PIN 9
 
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key; 

// Init array that will store new NUID 
byte nuidPICC[4];
boolean rele = LOW;

//knopke
const int buttonPin = 2;    // the number of the pushbutton pin
//kintamieji
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers


void getText(){
  if(Serial.available()) // check if we send a message
  {
    while(Serial.available())
    {
      char c = Serial.read(); // read the next character.
      text += c;
      delay(10);
    }  
  }
}

void sendText() {
  Wire.beginTransmission(I2CAddress); // transmit to device #9
  for(int i=0; i<(text.length()); i++){
    Wire.write(text[i]);
    Serial.println(text[i]);
  }              
  Wire.endTransmission();    // stop transmitting
  delay(1000);
}





void setup() { 
  Serial.begin(9600);
  Serial.println("Master here");
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 
  
  // Start the I2C Bus as Master
  Wire.begin(); 

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(buttonPin, INPUT);
}
 
void loop() {
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  } 

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        //Serial.println(buttonState);
        //buttonState == HIGH;
        text = "000000";
        //Serial.println(reading);
        sendText();
        delay(100);
        text = "";
      }
    }
  } 

  lastButtonState = reading;
  // Look for new cards
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;



 for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
   
  printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
   rfid.PICC_HaltA();

  
  rfid.PCD_StopCrypto1();
         String readedKey = String(rfid.uid.uidByte[0], HEX) + String(rfid.uid.uidByte[1], HEX) + String(rfid.uid.uidByte[2], HEX) + String(rfid.uid.uidByte[3], HEX);
         Serial.println(readedKey);
         text = readedKey;
         sendText();
         delay(100);
         text = "";
  //if ( readedKey == "46218fab" || readedKey == "c6c279e" || readedKey == "ecef5d83") {
         //Serial.println("Open");
         //rele=!rele;
         //digitalWrite(7, rele);       
         //digitalWrite(6,rele);
         //delay(1000);                  // waits for a second
         //digitalWrite(7, LOW);        // sets the digital pin 13 off
         //delay(1000);
         //Serial.println(rfid.uid.uidByte[0], HEX);
         //Serial.println(rfid.uid.uidByte[1], HEX);
         //Serial.println(rfid.uid.uidByte[2], HEX);
         //Serial.println(rfid.uid.uidByte[3], HEX);
    //}
}



void printHex(byte *buffer, byte bufferSize) {
//  for (byte i = 0; i < bufferSize; i++) {
//    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
//    Serial.print(buffer[i], HEX);
//  }
}

