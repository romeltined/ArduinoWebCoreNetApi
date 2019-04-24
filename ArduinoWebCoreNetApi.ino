/*
 WiFiEsp example: WebClientRepeating

 This sketch connects to a web server and makes an HTTP request
 using an Arduino ESP8266 module.
 It repeats the HTTP call each 10 seconds.

 For more details see: http://yaab-arduino.blogspot.com/p/wifiesp.html
*/



#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <dht.h>
#define dht_dpin A0 //no ; here. Set equal to channel sensor is on

#include "WiFiEsp.h"
// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(19, 18); // RX, TX
#endif

char ssid[] = "ssid";            // your network SSID (name)
char pass[] = "password";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status
char server[] = "192.168.1.111";
unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10000L; // delay between updates, in milliseconds

dht DHT;

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

String message = "Welcome";
String dhts = "";
float temper = 0;
float humid = 0;


// Initialize the Ethernet client object
WiFiEspClient client;

void setup()
{
  // initialize serial for debugging
  Serial.begin(115200);
  // initialize serial for ESP module
  Serial1.begin(115200);
  // initialize ESP module
  WiFi.init(&Serial1);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  Serial.println("You're connected to the network");
  
  printWifiStatus();
}

void loop()
{
  // if there's incoming data from the net connection send it out the serial port
  // this is for debugging purposes only
  lcd.print(message);
  message = "";
  while (client.available()) {
    //char c = client.read();
    //Serial.write(c);
    message += (char)client.read();
  }

  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print(message);
  
  DHT.read11(dht_dpin);
  temper = DHT.temperature;
  humid = DHT.humidity;

  // if 10 seconds have passed since your last connection,
  // then connect again and send data
  if (millis() - lastConnectionTime > postingInterval) {
    httpRequest();
  }

  
}

// this method makes a HTTP connection to the server
void httpRequest()
{
  Serial.println();
    
  // close any connection before send a new request
  // this will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection
  if (client.connect(server, 80)) {
    Serial.println("Connecting...");

    //POST DHTS
    dhts = "{\"temperature\":" + String(temper, 2) + ",\"humidity\":" + String(humid, 2) + "}"; 
    client.println("POST /api/dhts HTTP/1.1");
    client.println("Host: 192.168.1.111");
    client.println("Content-Type: application/json");
    client.println("Content-Length: " + String(dhts.length);
    client.println();
    client.println(dhts);
    client.println();


    //GET Message
    client.println("GET api/messages/0 HTTP/1.1");
    client.println("Host: 192.168.1.111");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made
    lastConnectionTime = millis();
  }
  else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
  }
}


void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}