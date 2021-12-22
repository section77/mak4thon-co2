/*
 * Verwendete Hardware:
 *  - Heltec WiFi Lora 32 V2
 *  - SCD30 oder MHZ19
 *  - RGB LED
 *
 * Funktionen:
 *  - CO2, Luftfeuchtigkeit und Temperatur aus dem SCD30/MHZ19 lesen
 *  - Grenzwertüberschreitung an der RGB LED anzeigen
 *  - Debugging über Serial 115200
 *  - Anzeige der Messwerte via WLAN (AP mode; SSID: s. Config-Block unten; erreichbar unter http://192.168.4.1/)
 *  - Messintervall kann per measurement_interval eingestellt werden
 *
 * Verwendete, zusätzliche Bibliotheken:
 *  - SparkFun_SCD30_Arduino_Library (über Bibliotheksverwaltung) https://github.com/sparkfun/SparkFun_SCD30_Arduino_Library
 *  - MH-Z19 by Jonathan Dempsey (über Bibliotheksverwaltung) https://github.com/WifWaf/MH-Z19
 */

#define USE_SCD30
//#define USE_MHZ19


// Replace with your network credentials
const char* ssid     = "CO2-Ampel";
const char* password = "123456789";

// Hardwaresettings
const int ledPinRed = 17;     // Nico: 32
const int ledPinGreen = 12;   // Nico: 33
const int ledPinBlue = 13;    // Nico: 12

const int measurement_interval = 30000; // delay between measurements in ms

const int level_red = 2000;
const int level_yellow = 1500;

// Load Wi-Fi library
#include <WiFi.h>

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

#include <Wire.h>
#include <Arduino.h>

#ifdef USE_SCD30
  #include "SparkFun_SCD30_Arduino_Library.h" //Click here to get the library: http://librarymanafger/All#SparkFun_SCD30
  SCD30 airSensor;
#endif


uint16_t co2;
float temperature;
float humidity;

unsigned long time_now;
String color_start;
String color_end;

void setup()
{
  Serial.begin(115200);
  Serial.println("CO2-api: Luftqualität messen, anzeigen und über Wifi oder LoRa weitergeben");

  Wire.begin();

#ifdef USE_SCD30
  if (airSensor.begin() == false)
    {
      Serial.println("Air sensor not detected. Please check wiring. Freezing...");
      while (1)
        ;
    }
#endif
#ifdef USE_MHZ19
  mySerial.begin(BAUDRATE, SERIAL_8N1, RX_PIN, TX_PIN);
  airSensor.begin(mySerial);
  airSensor.autoCalibration(); //auto calibration ON
#endif


  // RGB LED Ports konfigurieren
  pinMode(ledPinRed, OUTPUT);
  pinMode(ledPinGreen, OUTPUT);
  pinMode(ledPinBlue, OUTPUT);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)...");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  server.begin();
  time_now = millis()-measurement_interval;
}

void loop()
{
  if (millis() > time_now + measurement_interval)
    {
      time_now=millis();
#ifdef USE_SCD30
      if (airSensor.dataAvailable())
#endif
        {
          co2 = airSensor.getCO2();
          temperature = airSensor.getTemperature();
          humidity = 0;
#ifdef USE_SCD30
          humidity = airSensor.getHumidity();
#endif
        }

      if(co2 > level_red)          // red
        {
          digitalWrite(ledPinRed, HIGH);
          digitalWrite(ledPinGreen, LOW);
          digitalWrite(ledPinBlue, LOW);
          color_start="<red>";
          color_end="</red>";
        }
      else if(co2 > level_yellow)     // yellow
        {
          digitalWrite(ledPinRed, HIGH);
          digitalWrite(ledPinGreen, HIGH);
          digitalWrite(ledPinBlue, LOW);
          color_start="<yellow>";
          color_end="</yellow>";
        }
      else                   // green
        {
          digitalWrite(ledPinRed, LOW);
          digitalWrite(ledPinGreen, HIGH);
          digitalWrite(ledPinBlue, LOW);
          color_start="<green>";
          color_end="</green>";
        }

      Serial.print("co2: ");
      Serial.print(co2);
      Serial.println(" ppm");

      Serial.print("temp: ");
      Serial.print(temperature, 1);
      Serial.println(" °C");

      Serial.print("humidity: ");
      Serial.print(humidity, 1);
      Serial.println(" %");

      Serial.println();
    }
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) 
    {                             // If a new client connects,
      Serial.println("New Client.");          // print a message out in the serial port
      String currentLine = "";                // make a String to hold incoming data from the client
      while (client.connected()) {            // loop while the client's connected
        if (client.available()) {             // if there's bytes to read from the client,
          char c = client.read();             // read a byte, then
          Serial.write(c);                    // print it out the serial monitor
          header += c;
          if (c == '\n') {                    // if the byte is a newline character
            // if the current line is blank, you got two newline characters in a row.
            // that's the end of the client HTTP request, so send a response:
            if (currentLine.length() == 0) {
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();
          
              // Display the HTML web page
              client.println("<!DOCTYPE html><html>");
              client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
              client.println("<meta http-equiv=\"refresh\" content=\"30\">");
              client.println("<link rel=\"icon\" href=\"data:,\">");
              client.println("<style>");
              client.println("  body { background-color: #202020; color: #FFFFFF;}");
              client.println("  html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
              client.println("  red {font-size: 64px; color: #ff0000;}");
              client.println("  yellow {font-size: 64px; color: #ffff00;}");
              client.println("  green {font-size: 64px; color: #00ff00;}");
              client.println("  text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
              client.println("</style></head>");
          
              // Web Page Heading
              client.println("<body><h1>CO2-Ampel</h1>");
          
              // Display current readings
              client.print("<h2><p>CO2-Konzentration: [ppm]<b></h2>");
              client.print(color_start);
              client.printf("<h1>%4i </h1></p>", co2);
              client.print(color_end);
              client.printf("<p>Temperatur: %.1f &deg;C</p>", temperature);
              client.printf("<p>Luftfeuchtigkeit: %.1f %</p>", humidity);
             
              // The HTTP response ends with another blank line
              client.println();
              // Break out of the while loop
              break;
            } else { // if you got a newline, then clear currentLine
              currentLine = "";
            }
          } else if (c != '\r') {  // if you got anything else but a carriage return character,
            currentLine += c;      // add it to the end of the currentLine
          }
        }
      }
      // Clear the header variable
      header = "";
      // Close the connection
      client.stop();
      Serial.println("Client disconnected.");
      Serial.println("");
    } 
      
}
