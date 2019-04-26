# ArduinoWebCoreNetApi

This project sends temperature and humidity data to a Web Api .NET core.
It also displays a message from Web Api to the LCD attached to Arduino.
The connection is through ESP8266.

LCD I2C
1. Connect the Black jumper cable from the GND pin on the LCD to the GND pin on the MEGA
2.  Connect the Red jumper cable from then VCC pin on the LCD to the 5V pin on the MEGA
3. Connect the Green jumper cable from the SDA pin on the LCD to the SDA pin on the MEGA
4. Connect the Yellow jumper cable from the SCL pin on the LCD to the SCL pin on the MEGA

DHT11 Temperature and Humidity sensor
1. DHT11 Data PIN  to PIN 2 of Mega 2560
2. Vcc to 5v
3. GND to GND

ESP8266 to Arduino MEGA 2560
1. Arduino TX (18) to ESP RX. Use R1=1k ohm (to RX) and R2=2.2k ohm (to GND) voltage divider.
2. Arduino RX (19) to ESP TX.
3. Arduino 3.3v to ESP VCC, RESET, CH_EN
4. GND to GND

The Web API code
https://github.com/romeltined/WebCoreNetApi
