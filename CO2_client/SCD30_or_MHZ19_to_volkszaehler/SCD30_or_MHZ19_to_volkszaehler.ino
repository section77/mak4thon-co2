/*
 * Verwendete Hardware:
 *  - Heltec WiFi Lora 32 V2
 *  - SCD30 oder MHZ19
 *  - RGB LED
 *
 * Funktionen:
 *  - CO2, Luftfeuchtigkeit und Temperatur aus dem SCD30/MHZ19 lesen
 *  - auf dem OLED darstellen
 *  - Grenzwertüberschreitung an der RGB LED anzeigen
 *  - Werte über WiFi nach volkszaehler.org schreiben
 *  - Debugging über Serial 115200
 *
 * Verwendete, zusätzliche Bibliotheken:
 *  - U8g2_u8x8 für OLED (über Bibliotheksverwaltung)
 *  - SparkFun_SCD30_Arduino_Library (über Bibliotheksverwaltung) https://github.com/sparkfun/SparkFun_SCD30_Arduino_Library
 *  - MH-Z19 by Jonathan Dempsey (über Bibliotheksverwaltung) https://github.com/WifWaf/MH-Z19
 */

#define USE_SCD30
//#define USE_MHZ19

/*************************************************************************************/

#include <Arduino.h>
#include <U8x8lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#include <Wire.h>
#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>
#include "wlan_settings.h"
#include "uuids.h"

#ifdef USE_SCD30
#include "SparkFun_SCD30_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_SCD30
SCD30 airSensor;
#endif

#ifdef USE_MHZ19
#include "MHZ19.h"
MHZ19 airSensor;
HardwareSerial mySerial(1);
#define RX_PIN 36
#define TX_PIN 2
#define BAUDRATE 9600
#endif

WiFiClient client;

// Hardwaresettings
const int ledPinRed = 17;
const int ledPinGreen = 12;
const int ledPinBlue = 13;

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

HTTPClient http;

void setup()
{
  Serial.begin(115200);
  Serial.println("CO2-api: Luftqualität messen, anzeigen und über Wifi weitergeben");

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

  // OLED
  u8x8.begin();
  u8x8.setFont(u8x8_font_courB18_2x3_f);
  u8x8.setCursor(0, 0);
  u8x8.print("connect");
  u8x8.setCursor(0, 3);
  u8x8.print("WiFi");
  u8x8.setCursor(0, 5);

  Serial.println("init WiFi network");
  WiFi.begin(ssid, wlan_password);
  Serial.println("Connecting to ");

  Serial.println(String(ssid));
  Serial.println("MAC: " + WiFi.macAddress());

  uint8_t max_wifi_retry = 5;  // entspricht 10s
  while (WiFi.status() != WL_CONNECTED && --max_wifi_retry)
    {
      delay(2000);
      Serial.print(".");
      u8x8.print(".");
    }

  if (max_wifi_retry)
    {
      Serial.println("");
      Serial.println("WiFi connected ");
      Serial.println(WiFi.localIP().toString());
    }
  else
    {
      Serial.println("WiFi NOT connected");
      u8x8.setCursor(0, 5);
      u8x8.print("FAILED");
      delay (3000);
    }
    
  u8x8.clear ();

  // RGB LED Ports konfigurieren
  pinMode(ledPinRed, OUTPUT);
  pinMode(ledPinGreen, OUTPUT);
  pinMode(ledPinBlue, OUTPUT);

  // allow reuse (if server supports it)
  http.setReuse(true);
}

uint8_t push_http (const char* url, const char* UUID, float value)
{
  uint8_t ret = 0;

  char buf[300];
  snprintf (buf, 300, "%s%s.json?operation=add&value=%.2f", url, UUID, value);

  // debugging
  //Serial.println (buf);
  http.begin(buf);

  int httpCode = http.GET();

  // httpCode will be negative on error
  if(httpCode > 0)
    {
      // HTTP header has been send and Server response header has been handled
      //Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      if(httpCode == HTTP_CODE_OK)
        ;//Serial.println("sent value via http");
      else
        ret = -1;
    }
  else
    {
      Serial.printf("[HTTP] GET failed, error: %s\n", http.errorToString(httpCode).c_str());
      ret = -1;
    }
  http.end();
  return ret;
}

void push_value (const char* UUID, float value)
{
  // Debugging: Send to host in LAN
  // push_http ("http://192.168.10.116:8888", UUID, value);

  if(WiFi.status() == WL_CONNECTED)
    {
      if (push_http ("http://demo.volkszaehler.org/middleware/data/", UUID, value))
        {
          Serial.println("push_http failed");
        }
    }
}

void pre ()
{
  u8x8.setCursor(0, 2);
  if (WiFi.status() == WL_CONNECTED)
    u8x8.print("WiFi");
  else
    u8x8.print("    ");

  u8x8.setCursor(0, 5);
}

void loop()
{
#ifdef USE_SCD30
  if (airSensor.dataAvailable())
#endif
    {
      uint16_t co2 = airSensor.getCO2();
      float temperature = airSensor.getTemperature();
      float humidity = 0;
#ifdef USE_SCD30
      humidity = airSensor.getHumidity();
#endif

      if(co2 > 2000)           // red
        {
          digitalWrite(ledPinRed, HIGH);
          digitalWrite(ledPinGreen, LOW);
          digitalWrite(ledPinBlue, LOW);
        }
      else if(co2 > 1500)      // yellow
        {
          digitalWrite(ledPinRed, HIGH);
          digitalWrite(ledPinGreen, HIGH);
          digitalWrite(ledPinBlue, LOW);
        }
      else                    // green
        {
          digitalWrite(ledPinRed, LOW);
          digitalWrite(ledPinGreen, HIGH);
          digitalWrite(ledPinBlue, LOW);
        }

      Serial.print("co2(ppm):");
      Serial.print(co2);

      Serial.print(" temp(C):");
      Serial.print(temperature, 1);

      Serial.print(" humidity(%):");
      Serial.print(humidity, 1);

      Serial.println();

      pre ();
      u8x8.printf("%4ippm ", co2);
      push_value (uuid_co2, co2);
      delay (1000);

      pre ();
      u8x8.printf("%4.1f ", temperature);
      u8x8.print("\xb0");
      u8x8.print("C ");
      push_value (uuid_temp, temperature);
      delay (1000);

      pre ();
      u8x8.printf("%4.1f%%rH", humidity);
      push_value (uuid_humidity, humidity);
      delay (1000);

      pre ();
      u8x8.printf("RSSI%4i", WiFi.RSSI());
      delay (1000);

    }
#ifdef USE_SCD30
  else
    {
      Serial.println("Waiting for new sensor data");
      delay(1000);
    }
#else
  delay (5000);
#endif

}
