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
#define LBUILTIN 2
boolean rele = LOW;
boolean siunciudata = LOW;
boolean siunciukey = LOW;
int x = 0; //variable for looping
char dataStr[4]; //create array to store the response. for example : SomeValue1=0 
String text;
String code;

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

//char server[] = "www.fmrc.lt";

char server[] = "192.168.0.151";
//IPAddress server(192,168,0,151);

unsigned long lastConnectionTime = 0;             // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 3L * 1000L; // delay between updates, in milliseconds
// the "L" is needed to use long type numbers


void receiveString(int bytes) {
  text = "";
  while (Wire.available()) { 
    char dd = Wire.read(); // receive a byte as character
    text += dd;
  }
  siunciukey = HIGH;
  
}

void setup() {
  pinMode(LBUILTIN, OUTPUT);
  //digitalWrite(LBUILTIN, rele);
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

  
      if (millis() - lastConnectionTime > postingInterval) {
       //siunciudata = HIGH;
       httpRequest();
      }
  
     if (client.available()) {
        char c = client.read();
        //Serial.write(c);
        if(c == '>'){
          //x=1;
          //code = "";
          for (int i = 0; i < 4; i++){
            //Serial.print(i);
            c = client.read();
            dataStr[i]=c;
            //Serial.write(c);
                //code += String(client.read());
            Serial.write(dataStr[i]);
            if (dataStr[0] == '0' && dataStr[1] == '0' && dataStr[2] == '0' && dataStr[3] == '1'){Serial.print("OK");}
            if (dataStr[0] == '0' && dataStr[1] == '0' && dataStr[2] == '0' && dataStr[3] == '2'){rele = !rele; digitalWrite(LBUILTIN, rele);}
            //if (String(dataStr) == "0001"){Serial.print("OK");}
            //if (String(dataStr) == "0002"){rele = !rele; digitalWrite(LED_BUILTIN, rele);}
             }
            
            //Serial.print(dataStr['0']);
            //Serial.print(dataStr['1']);
            //Serial.print(dataStr['2']);
            //Serial.print(dataStr['3']);
          /*if (String(dataStr) == "OpenDoor=1") {
              //Serial.print("_");
              //Serial.print(String(dataStr));            
              //Serial.print("_");             
              //Serial.println("Atidarome duris");
             }*/
          }
      }



}

// this method makes a HTTP connection to the server:
void httpRequest() {

  client.stop();

  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP GET request:
    if (siunciukey == HIGH) {
          client.println("GET /dcontrol/door.php?id=1&hash="+text+" HTTP/1.1");
          siunciukey = LOW;
      } else {
         client.println("GET /dcontrol/door.php HTTP/1.1");
      }
    //client.println("Host: www.fmrc.lt");
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

  //siunciudata = LOW;
}
