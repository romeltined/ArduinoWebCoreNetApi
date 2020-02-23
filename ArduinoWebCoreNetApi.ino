/*
 WiFiEsp example: WebClientRepeating

 This sketch connects to a web server and makes an HTTP request
 using an Arduino ESP8266 module.
 It repeats the HTTP call each 10 seconds.

 For more details see: http://yaab-arduino.blogspot.com/p/wifiesp.html
*/



#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#define DHTPIN 2 //no ; here. Set equal to channel sensor is on
#define DHTTYPE DHT11

#include "WiFiEsp.h"
// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(19, 18); // RX, TX
#endif

#include <ArduinoJson.h>

char ssid[] = "ssid";            // your network SSID (name)
char pass[] = "pwd";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status
char server[] = "192.168.1.111";
unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10000L; // delay between updates, in milliseconds

DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

String message = "Arduino";
String dhts = "";
float temper = 0;
float humid = 0;
String contentlength = "";

// Initialize the Ethernet client object
WiFiEspClient client;

void setup()
{
  // initialize serial for debugging
  Serial.begin(115200);
  // initialize serial for ESP module
  Serial1.begin(115200);

  dht.begin();

  
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

  lcd.init();  
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print(message);
}

void loop()
{

  
  // if there's incoming data from the net connection send it out the serial port
  // this is for debugging purposes only
//  while (client.available()) {
//    char c = client.read();
//    Serial.write(c);
//  }


  String json;  
  int lines_received = 0;
  while(client.available()) {
    String line = client.readStringUntil('\r\n');
   //sometimes use 8th line
    if (lines_received == 9) { 
      json=line;  
    }
    lines_received++;
  }

  //Serial.println(json);
  
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, json);
  const char* desc = doc["description"];
 
  if(json.length()>0){
    Serial.println("from server: "+json);
    lcd.clear();
    lcd.backlight();
    lcd.setCursor(0,0);
  lcd.print(desc);
  }
 
  temper = dht.readTemperature(); 
  humid = dht.readHumidity();

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
    contentlength="Content-Length: " + String(dhts.length());
    client.println(contentlength);
    client.println();
    client.println(dhts);
    client.println();


    //GET Message
    client.println("GET /api/messages/0 HTTP/1.1");
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
