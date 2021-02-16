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
#include <Wire.h> //i2c bibiiotka
#define I2CAddress 9
int x = 0; //variable for looping
char dataStr[12]; //create array to store the response. for example : SomeValue1=0 
char c;
#define ledPin 13
boolean siunciudata = LOW;
boolean siunciukey = LOW;

// assign a MAC address for the ethernet controller.
// fill in your address here:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
// fill in an available IP address on your network here,
// for manual configuration:
IPAddress ip(192, 168, 0, 177);

// fill in your Domain Name Server address here:
IPAddress myDns(1, 1, 1, 1);

// initialize the library instance:
EthernetClient client;

char server[] = "www.fmrc.lt";
//IPAddress server(64,131,82,241);

unsigned long lastConnectionTime = 0;             // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10L * 1000L; // delay between updates, in milliseconds
// the "L" is needed to use long type numbers

void receiveString(int bytes) {
  String text = "";
  while (Wire.available()) { 
    char datn = Wire.read(); // receive a byte as character
    text += datn;
  }
  Serial.print("Printing: ");
  Serial.println(text);
  httpRequestKey(text);
  //manageLed(text);
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
  // start serial port:
  Serial.begin(9600);
  Serial.println("Slave here with LAN");
 
  
  // Start the I2C Bus as Slave on address 9
  Wire.begin(I2CAddress); 

   // Attach a function to trigger when something is received.
  Wire.onReceive(receiveString);
  
  // give the ethernet module time to boot up:
  delay(1000);
  // start the Ethernet connection using a fixed IP address and DNS server:
  Ethernet.begin(mac, ip, myDns);
  // print the Ethernet board/shield's IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
}

void loop() {

while (siunciudata == LOW && siunciudata == LOW) {
    if (millis() - lastConnectionTime > postingInterval) {
    httpRequest();
    }
  }
  

  
  //Wire.onReceive(receiveString);
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

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
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }

}

// this method makes a HTTP connection to the server:
void httpRequest() {
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
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }

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


}

  void printdata() {
  for (x=0;x<12;x++){
    Serial.print(dataStr[x]);
  }
  //siunciu = LOW;
}
