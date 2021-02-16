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
 * RST/Reset   RST          6             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      5            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */

#include <SPI.h> //interfeisui palaikyti
#include <Ethernet.h> //LAN biblioteka
#include <MFRC522.h> //RFID chipas

#define SS_PIN 5
#define RST_PIN 6

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key; 

// Init array that will store new NUID 
byte nuidPICC[4];
boolean rele = LOW;

boolean siunciu = LOW; //duomenų siuntimo busena

int x = 0; //variable for looping
char dataStr[12]; //create array to store the response. for example : SomeValue1=0 
char c;

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
//statinis IP
IPAddress ip(192, 168, 0, 177);
EthernetClient client;
//serveris
char server[] = "192.168.0.151";

unsigned long lastConnectionTime = 0;             // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 1L * 1000L; // delay between updates, in milliseconds
// the "L" is needed to use long type numbers

void setup() {
  // start serial port:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  Ethernet.begin(mac, ip);
  delay(1000);
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
}

void loop() {

  //Serial.print(String(dataStr));

  while (siunciu == LOW) {
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
             delay(100);
      if ( readedKey == "46218fab" || readedKey == "c6c279e" || readedKey == "ecef5d83" || readedKey == "15bae72b") {
             //Serial.println("Open");
             rele=!rele;
             digitalWrite(7, rele);       
             //digitalWrite(6,rele);
             
             siunciu = HIGH;
             httpRequest(readedKey);
             //delay(1000);                  // waits for a second
             //digitalWrite(7, LOW);        // sets the digital pin 13 off
             //delay(1000);
             //Serial.println(rfid.uid.uidByte[0], HEX);
             //Serial.println(rfid.uid.uidByte[1], HEX);
             //Serial.println(rfid.uid.uidByte[2], HEX);
             //Serial.println(rfid.uid.uidByte[3], HEX);
        }
  }
  

  while (siunciu == HIGH) {
  
    
       
     
    
    if (client.available()) {
    c = client.read(); //read first character
    //Serial.write(c);
        while (c != '<'){ //while < character is not coming yet, keep reading character
          c = client.read();
          //Serial.write(c);
        }
        c = client.read(); //read the '<' character, but not storing in array
        while (c != '>'){ //while > character is not coming yet,
          dataStr[x] = c; //store character in array
          c = client.read(); //read next character
          x++; //incrementing index array
        }
        printdata(); //print the character that has been captured by array at Serial Monitor
        if (String(dataStr) == "OpenDoor=1") {
          //Serial.print("_");
          //Serial.print(String(dataStr));
          //Serial.print("_");
          Serial.println("Atidarome duris");
          }
         
  }

  // if ten seconds have passed since your last connection,
  // then connect again and send data:
  if (millis() - lastConnectionTime > postingInterval) {
   siunciu = LOW;
  }

}
 
  
}

void httpRequest(String readedKey) {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP GET request:
    client.println("GET /dcontrol/door.php?id=1&hash="+readedKey+" HTTP/1.1");
    client.println("Host: 192.168.0.151");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }

}

void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void printdata() {
  for (x=0;x<12;x++){
    Serial.print(dataStr[x]);
  }
  siunciu = LOW;
}
