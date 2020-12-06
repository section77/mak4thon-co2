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
 *  - Werte über LoRa senden
 *  - Debugging über Serial 115200
 *
 * Verwendete, zusätzliche Bibliotheken:
 *  - U8g2_u8x8 für OLED (über Bibliotheksverwaltung)
 *  - SparkFun_SCD30_Arduino_Library (über Bibliotheksverwaltung) https://github.com/sparkfun/SparkFun_SCD30_Arduino_Library
 *  - MH-Z19 by Jonathan Dempsey (über Bibliotheksverwaltung) https://github.com/WifWaf/MH-Z19
 *  - Radiohead für LoRa (Zip Download, installieren) http://www.airspayce.com/mikem/arduino/RadioHead/
 */

//#define USE_VOLKSZAEHLER
#define USE_NEW_POST_JSON
//#define USE_LORA

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

#ifdef USE_LORA
#include <RH_RF95.h>
RH_RF95 rf95(18, 26); // WiFi Lora 32 V2 (integriert)
#endif

#include <HTTPClient.h>
#include "wlan_settings.h"
//#include "uuids.h"
#include "module_token.h"

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
const int ledPinRed = 17;     // Nico: 32
const int ledPinGreen = 12;   // Nico: 33
const int ledPinBlue = 13;    // Nico: 12

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

HTTPClient http;

void setup()
{
  Serial.begin(115200);
  Serial.println("CO2-api: Luftqualität messen, anzeigen und über Wifi oder LoRa weitergeben");

  // FIXME: Aus Sicherheitsgründen würde ich später nur die module_id auf Display und über Serial ausgeben
  Serial.printf("module_token = '%s'\n", module_token);
  Serial.printf("module_id = '%s'\n", module_id);

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

  Serial.println("init WiFi network");
  WiFi.begin(ssid, wlan_password);
  Serial.println("Connecting to ");

  Serial.println(String(ssid));
  Serial.println("MAC: " + WiFi.macAddress());
  uint8_t max_wifi_retry = 10;  // entspricht 5s
  while (WiFi.status() != WL_CONNECTED && --max_wifi_retry)
    {
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

  // Maximal 8 Zeichen module_id ausgeben
  {
    u8x8.setCursor(0, 2);
    // nach 8 Zeichen abschneiden
#define MAX_ID_LEN 8
    char tmp_mod_id[MAX_ID_LEN + 1];
    strncpy (tmp_mod_id, module_id, MAX_ID_LEN);
    tmp_mod_id[MAX_ID_LEN] = '\0';
    u8x8.print(tmp_mod_id);
  }

#ifdef USE_LORA
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
#endif

  // allow reuse (if server supports it)
  http.setReuse(true);
}

void build_JSON (char *buf, size_t buflen, uint16_t co2, float temperature, float humidity)
{
  snprintf (buf, buflen, "{\"token\":\"%s\",\"id\":\"%s\",\"co2\":%u,\"temperature\":%.2f,\"humidity\":%.2f}", module_token, module_id, co2, temperature, humidity);
  buf[buflen - 1] = '\0';
}

uint8_t http_POST (const char *host, uint16_t port, const char *uri, const char *json)
{
  uint8_t ret = 0;
  http.begin(host, port, uri);

  // über www-form-urlencoded wäre auch möglich
  //http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  //int httpCode = http.POST("token=555&co2=1234&temperature=24.5&humidity=44.5");

  http.addHeader("Content-Type", "application/json");


  int httpCode = http.POST(json);
  if(httpCode > 0)
    {
      // HTTP header has been send and Server response header has been handled
      // Serial.printf("[HTTP] POST... code: %d\n", httpCode);
    }
  else
    {
      Serial.printf("[HTTP] POST failed, error: %s\n", http.errorToString(httpCode).c_str());
      ret = -1;
    }
  http.end();
  return ret;
}

#ifdef USE_VOLKSZAEHLER
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
#endif

#ifdef USE_LORA
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
#endif

void push_value (const char* UUID, float value)
{
  // Debugging: Send to host in LAN
  // push_http ("http://192.168.10.116:8888", UUID, value);

#ifdef USE_VOLKSZAEHLER
  if(WiFi.status() == WL_CONNECTED)
    {
      if (push_http ("http://demo.volkszaehler.org/middleware/data/", UUID, value))
        {
          Serial.println("http failed, fallback to LoRa");
#ifdef USE_LORA
          push_LoRa (UUID, value);
#endif
        }
    }
  else
#endif
#ifdef USE_LORA
    push_LoRa (UUID, value);
#else
    ;
#endif
}

void pre ()
{
//  u8x8.clear();
//  u8x8.setCursor(0, 2);
//  if (WiFi.status() == WL_CONNECTED)
//    u8x8.print("WiFi");
//  else
//    u8x8.print("LoRa");

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

      if(co2 > 1550)   // red Nico hat 2000 daraus gemacht
        {
          digitalWrite(ledPinRed, HIGH);
          digitalWrite(ledPinGreen, LOW);
          digitalWrite(ledPinBlue, LOW);
        }
      else if(co2 > 550)      // yellow Nico hat 1500 daraus gemacht
        {
          digitalWrite(ledPinRed, HIGH);
          digitalWrite(ledPinGreen, HIGH);
          digitalWrite(ledPinBlue, LOW);
        }
      else      // green
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
      //push_value (uuid_co2, co2);
      delay (1000);

      pre ();
      u8x8.printf("%4.1f", temperature);
      u8x8.print("\xb0");
      u8x8.print("C ");
      //push_value (uuid_temp, temperature);
      delay (1000);

      pre ();
      u8x8.printf("%4.1f%%rH", humidity);
      //push_value (uuid_humidity, humidity);
      delay (1000);

#ifdef USE_NEW_POST_JSON
#define BUF_SIZE 200
      char buf[BUF_SIZE];
      build_JSON (buf, BUF_SIZE, co2, temperature, humidity);
      http_POST ("192.168.10.116", 4000, "/addSample", buf);
#endif

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
