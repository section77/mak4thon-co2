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

WiFiMulti wifiMulti;

// Hardwaresettings
const int ledPinRed = 17;
const int ledPinGreen = 12;
const int ledPinBlue = 13;

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

RH_RF95 rf95(18, 26); // WiFi Lora 32 V2 (integriert)

void setup()
{
  Serial.begin(115200);
  Serial.println("SCD30 Example");
  Wire.begin();

  if (airSensor.begin() == false)
  {
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    while (1)
      ;
  }

  if (wifiMulti.addAP(ssid, wlan_password) == false) {
    Serial.println("WiFi ssid or passwort not correct. Please check wlan_settings.h. Freezing ...");
    //while(true){};
  }

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

void push_volkszaehler (const char* UUID, float value)
{
  HTTPClient http;

  //Serial.print("[HTTP] begin...\n");
  char buf[300];
  snprintf (buf, 300, "http://demo.volkszaehler.org/middleware/data/%s.json?operation=add&value=%.2f", UUID, value);

  // debugging
  //Serial.print (buf);
  http.begin(buf);

  int httpCode = http.GET();

  // httpCode will be negative on error
  if(httpCode > 0)
    {
        // HTTP header has been send and Server response header has been handled
        //Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if(httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            //Serial.println(payload);
        }
    }
  else
    {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

  http.end();

}

void loop()
{
  if (airSensor.dataAvailable())
  {
    if(airSensor.getCO2() > 1000) { // red
        digitalWrite(ledPinRed, HIGH);
        digitalWrite(ledPinGreen, LOW);
        digitalWrite(ledPinBlue, LOW);
    } else if(airSensor.getCO2() > 800) {  // yellow
        digitalWrite(ledPinRed, HIGH);
        digitalWrite(ledPinGreen, HIGH);
        digitalWrite(ledPinBlue, LOW);
    } else {  // green
        digitalWrite(ledPinRed, LOW);
        digitalWrite(ledPinGreen, HIGH);
        digitalWrite(ledPinBlue, LOW);
    }

    Serial.print("co2(ppm):");
    Serial.print(airSensor.getCO2());

    Serial.print(" temp(C):");
    Serial.print(airSensor.getTemperature(), 1);

    Serial.print(" humidity(%):");
    Serial.print(airSensor.getHumidity(), 1);

    Serial.println();

    u8x8.clear();
    u8x8.setCursor(0, 5);
    u8x8.printf("%ippm", airSensor.getCO2());
    delay (1500);

    u8x8.clear();
    u8x8.setCursor(0, 5);
    u8x8.printf("%.1f", airSensor.getTemperature());
    u8x8.print("\xb0");
    u8x8.print("C");
    delay (1500);

    u8x8.clear();
    u8x8.setCursor(0, 5);
    u8x8.printf("%.1f%%rH", airSensor.getHumidity());
    delay (1500);

    // wait for WiFi connection
    if((wifiMulti.run() == WL_CONNECTED))
    {
      push_volkszaehler (uuid_co2, airSensor.getCO2());
      push_volkszaehler (uuid_temp, airSensor.getTemperature());
      push_volkszaehler (uuid_humidity, airSensor.getHumidity());
    }
  }
  else
    {
      Serial.println("Waiting for new data");
      delay(1000);
    }

  // Send a message to rf95_server
  char value_buf[100];
  snprintf (value_buf, 100, "%u;%.1f;%.1f", airSensor.getCO2(), airSensor.getTemperature(), airSensor.getHumidity());
  rf95.send((uint8_t *)value_buf, sizeof(value_buf));

  rf95.waitPacketSent();

  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (rf95.waitAvailableTimeout(3000))
  {
    // Should be a reply message for us now
    if (rf95.recv(buf, &len))
      {
        Serial.print("got reply: ");
        Serial.println((char*)buf);
        // Serial.print("RSSI: ");
        // Serial.println(rf95.lastRssi(), DEC);
      }
    else
      {
        Serial.println("recv failed");
      }

  }
  else
  {
    Serial.println("No reply, is rf95_server running?");
  }
}
