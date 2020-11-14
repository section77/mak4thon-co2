/*
 * Verwendete Hardware:
 *  - ESP32 WROOM32 von az-delivery
 *  - RFM9x LoRa 868/915MHz: https://www.adafruit.com/product/3072
 *
 * Funktionen:
 *  - CO2, Luftfeuchtigkeit und Temperatur über LoRa empfangen
 *  - Diese über WiFi nach volkszaehler.org schreiben
 *  - Debugging über Serial 115200
 *
 * Verwendete, zusätzliche Bibliotheken:
 *  - Radiohead für LoRa (Zip Download, installieren) http://www.airspayce.com/mikem/arduino/RadioHead/
 */

#include <SPI.h>
#include <RH_RF95.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include "wlan_settings.h"
#include "uuids.h"

WiFiClient client;

// Singleton instance of the radio driver
RH_RF95 rf95 (5, 2);  // Adafruit RFM9x am ESP32 ohne Display

int led = 9;

void setup()
{
  pinMode(led, OUTPUT);
  Serial.begin(115200);

  Serial.println("init WiFi network");

  delay (100);
  WiFi.begin(ssid, wlan_password);
  Serial.println("Connecting to ");

  Serial.println(String(ssid));
  Serial.println("MAC: " + WiFi.macAddress());
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected ");
  Serial.println(WiFi.localIP().toString());

  if (!rf95.init())
    Serial.println("init failed");
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  //  driver.setTxPower(23, false);
  // If you are using Modtronix inAir4 or inAir9,or any other module which uses the
  // transmitter RFO pins and not the PA_BOOST pins
  // then you can configure the power transmitter power for -1 to 14 dBm and with useRFO true.
  // Failure to do that will result in extremely low transmit powers.
  //  driver.setTxPower(14, true);
}

uint8_t push_http (const char* url, const char* UUID, const char* value)
{
  uint8_t ret = 0;
  HTTPClient http;

  char buf[300];
  snprintf (buf, 300, "%s%s.json?operation=add&value=%s", url, UUID, value);

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

void loop()
{
  if (rf95.available())
  {
    // Should be a message for us now
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len))
    {
      //digitalWrite(led, HIGH);
//      RH_RF95::printBuffer("request: ", buf, len);
      Serial.print("received: ");
      Serial.println((char*)buf);
//      Serial.print("RSSI: ");
//      Serial.println(rf95.lastRssi(), DEC);

      // UUID und value an ";" trennen
      for (uint8_t k = 0; k < len; ++k)
        if (buf[k] == ';')
          {
            buf[k] = 0;
            push_http ("http://demo.volkszaehler.org/middleware/data/", (char*) buf, (char*) buf + k + 1);
            break;
          }
    }
    else
    {
      Serial.println("recv failed");
    }
  }
}
