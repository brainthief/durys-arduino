/*
  Repeating Web client

 This sketch connects to a a web server and makes a request
 using a Wiznet Ethernet shield. You can use the Arduino Ethernet shield, or
 the Adafruit Ethernet shield, either one will work, as long as it's got
 a Wiznet Ethernet module on board.

 This example uses DNS, by assigning the Ethernet client with a MAC address,
 IP address, and DNS address.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13

 created 19 Apr 2012
 by Tom Igoe
 modified 21 Jan 2014
 by Federico Vanzati

 http://www.arduino.cc/en/Tutorial/WebClientRepeating
 This code is in the public domain.

 */

#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#define I2CAddress 9
boolean siunciudata = LOW;
boolean siunciukey = LOW;
int x = 0; //variable for looping
char dataStr[12]; //create array to store the response. for example : SomeValue1=0 
String text;

// assign a MAC address for the ethernet controller.
// fill in your address here:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
// fill in an available IP address on your network here,
// for manual configuration:
IPAddress ip(192, 168, 0, 177);

// fill in your Domain Name Server address here:
IPAddress myDns(192, 168, 0, 1);

// initialize the library instance:
EthernetClient client;

char server[] = "www.fmrc.lt";
//IPAddress server(64,131,82,241);

unsigned long lastConnectionTime = 0;             // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10L * 1000L; // delay between updates, in milliseconds
// the "L" is needed to use long type numbers

void receiveString(int bytes) {
  text = "";
  while (Wire.available()) { 
    char dd = Wire.read(); // receive a byte as character
    text += dd;
  }
  //Serial.print("Printing: ");
  //Serial.println(text);
  siunciukey = HIGH;
  
  //manageLed(text);
}

void setup() {
  // start serial port:
  Serial.begin(9600);
  Serial.println("Slave here with LAN");

  // Start the I2C Bus as Slave on address 9
  Wire.begin(I2CAddress); 

   // Attach a function to trigger when something is received.
  Wire.onReceive(receiveString);

  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // give the ethernet module time to boot up:
  delay(1000);
  // start the Ethernet connection using a fixed IP address and DNS server:
  Ethernet.begin(mac, ip, myDns);
  // print the Ethernet board/shield's IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
}

void loop() {

  if (siunciukey == LOW) {
      if (millis() - lastConnectionTime > postingInterval) {
        siunciudata = HIGH;
        httpRequest();
      }

     if (client.available()) {
        char c = client.read();
        Serial.write(c);
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
//              if (String(dataStr) == "OpenDoor=1") {
//                //Serial.print("_");
//                //Serial.print(String(dataStr));
//                //Serial.print("_");
//                Serial.println("Atidarome duris");
//                }
      }

    }
   if (siunciudata == LOW && siunciukey == HIGH) {
       httpRequestKey(text); 
//       if (client.available()) {
//         char c = client.read(); //read first character
//          //Serial.write(c);
//              while (c != '<'){ //while < character is not coming yet, keep reading character
//                c = client.read();
//                //Serial.write(c);
//              }
//              c = client.read(); //read the '<' character, but not storing in array
//              while (c != '>'){ //while > character is not coming yet,
//                dataStr[x] = c; //store character in array
//                c = client.read(); //read next character
//                x++; //incrementing index array
//              }
//              //printdata(); //print the character that has been captured by array at Serial Monitor
//              if (String(dataStr) == "OpenDoor=1") {
//                //Serial.print("_");
//                //Serial.print(String(dataStr));
//                //Serial.print("_");
//                Serial.println("Atidarome duris");
//                }
//         
//        }

       
       if (2*millis() - lastConnectionTime > postingInterval) {
        siunciukey = LOW;
      }
    }
    
 
  
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
//  if (client.available()) {
//    char c = client.read();
//    Serial.write(c);
//  }

  // if ten seconds have passed since your last connection,
  // then connect again and send data:
  

}


void httpRequestKey(String readedKey) {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP GET request:
    client.println("GET /dcontrol/door.php?id=1&hash="+readedKey+" HTTP/1.1");
    client.println("Host: www.fmrc.lt");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
    siunciukey = LOW;
    text = "";
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }

}

// this method makes a HTTP connection to the server:
void httpRequest() {

  //siunciudata = HIGH;
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP GET request:
    client.println("GET /dcontrol/door.php HTTP/1.1");
    client.println("Host: www.fmrc.lt");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
    siunciudata = LOW;
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }

  //siunciudata = LOW;
}

void printdata() {
  for (x=0;x<12;x++){
    Serial.print(dataStr[x]);
  }
}
