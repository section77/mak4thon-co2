/*
 * Verwendete Hardware:
 *  - Heltec WiFi Lora 32 V2
 *  - SCD30
 *  - RGB LED
 *
 * Funktionen:
 *  - CO2, Luftfeuchtigkeit und Temperatur aus dem SCD30 lesen
 *  - auf dem OLED darstellen
 *  - Grenzwertüberschreitung an der RGB LED anzeigen
 *  - Werte über WiFi nach volkszaehler.org schreiben
 *  - Werte über LoRa senden
 *  - Debugging über Serial 115200
 *
 * Verwendete, zusätzliche Bibliotheken:
 *  - U8g2_u8x8 für OLED (über Bibliotheksverwaltung)
 *  - SparkFun_SCD30_Arduino_Library (über Bibliotheksverwaltung) https://github.com/sparkfun/SparkFun_SCD30_Arduino_Library
 *  - Radiohead für LoRa (Zip Download, installieren) http://www.airspayce.com/mikem/arduino/RadioHead/
 */

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

#include <RH_RF95.h>

#include <HTTPClient.h>
#include "wlan_settings.h"
#include "uuids.h"

#include "SparkFun_SCD30_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_SCD30
SCD30 airSensor;

WiFiClient client;

// Hardwaresettings
const int ledPinRed = 17;
const int ledPinGreen = 12;
const int ledPinBlue = 13;

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

RH_RF95 rf95(18, 26); // WiFi Lora 32 V2 (integriert)

void setup()
{
  Serial.begin(115200);
  Serial.println("CO2-api: Luftqualität messen, anzeigen und über Wifi oder LoRa weitergeben");
  Wire.begin();

  if (airSensor.begin() == false)
  {
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    while (1)
      ;
  }

  Serial.println("init WiFi network");
  WiFi.begin(ssid, wlan_password);
  Serial.println("Connecting to ");

  Serial.println(String(ssid));
  Serial.println("MAC: " + WiFi.macAddress());
  uint8_t max_wifi_retry = 10;  // entspricht 5s
  while (WiFi.status() != WL_CONNECTED && --max_wifi_retry) {
    delay(500);
    Serial.print(".");
  }
  if (max_wifi_retry)
    {
      Serial.println("");
      Serial.println("WiFi connected ");
      Serial.println(WiFi.localIP().toString());
    }
  else
    Serial.println("WiFi NOT connected (using LoRa fallback)");

  // RGB LED Ports konfigurieren
  pinMode(ledPinRed, OUTPUT);
  pinMode(ledPinGreen, OUTPUT);
  pinMode(ledPinBlue, OUTPUT);

  // OLED
  u8x8.begin();
  u8x8.setFont(u8x8_font_courB18_2x3_f);

  // LoRa
  if (!rf95.init())
    Serial.println("init failed");
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // You can change the modulation parameters with eg
  // rf95.setModemConfig(RH_RF95::Bw500Cr45Sf128);

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 2 to 20 dBm:
  //  rf95.setTxPower(20, false);
  // If you are using Modtronix inAir4 or inAir9, or any other module which uses the
  // transmitter RFO pins and not the PA_BOOST pins
  // then you can configure the power transmitter power for 0 to 15 dBm and with useRFO true.
  // Failure to do that will result in extremely low transmit powers.
  //  rf95.setTxPower(14, true);
}

uint8_t push_http (const char* url, const char* UUID, float value)
{
  uint8_t ret = 0;
  HTTPClient http;

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
          Serial.println("sent value via http");
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

void push_LoRa (const char* UUID, float value)
{
  // Messwert an die LoRa-WiFi-Bridge senden
  char value_buf[100];
  snprintf (value_buf, 100, "%s;%.2f", UUID, value);
  rf95.send((uint8_t *)value_buf, sizeof(value_buf));
  rf95.waitPacketSent();
  // Wir erwarten keine Antwort von der LoRa_WiFi_Bridge
  Serial.println("sent value via LoRa");
}

void push_value (const char* UUID, float value)
{
  // Debugging: Send to host in LAN
  // push_http ("http://192.168.10.116:8888", UUID, value);

  if(WiFi.status() == WL_CONNECTED)
    {
      if (push_http ("http://demo.volkszaehler.org/middleware/data/", UUID, value))
        {
          Serial.println("http failed, fallback to LoRa");
          push_LoRa (UUID, value);
        }
    }
  else
    push_LoRa (UUID, value);
}

void pre ()
{
  u8x8.clear();
  u8x8.setCursor(0, 2);
  if (WiFi.status() == WL_CONNECTED)
    u8x8.print("WiFi");
  else
    u8x8.print("LoRa");

  u8x8.setCursor(0, 5);
}

void loop()
{
  if (airSensor.dataAvailable())
    {
      uint16_t co2 = airSensor.getCO2();
      float humidity = airSensor.getHumidity();
      float temperature = airSensor.getTemperature();

      if(co2 > 1000) { // red
          digitalWrite(ledPinRed, HIGH);
          digitalWrite(ledPinGreen, LOW);
          digitalWrite(ledPinBlue, LOW);
      } else if(co2 > 800) {  // yellow
          digitalWrite(ledPinRed, HIGH);
          digitalWrite(ledPinGreen, HIGH);
          digitalWrite(ledPinBlue, LOW);
      } else {  // green
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
      u8x8.printf("%ippm", co2);
      push_value (uuid_co2, co2);
      delay (1000);

      pre ();
      u8x8.printf("%.1f", temperature);
      u8x8.print("\xb0");
      u8x8.print("C");
      push_value (uuid_temp, temperature);
      delay (1000);

      pre ();
      u8x8.printf("%.1f%%rH", humidity);
      push_value (uuid_humidity, humidity);
      delay (1000);
    }
  else
    {
      Serial.println("Waiting for new SCD30 data");
      delay(1000);
    }


}
