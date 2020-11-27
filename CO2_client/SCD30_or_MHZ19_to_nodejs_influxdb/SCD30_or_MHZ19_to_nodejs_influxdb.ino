/*
 * Verwendete Hardware:
 *  - Heltec WiFi Lora 32 V2
 *  - SCD30 oder MHZ19
 *  - RGB LED
 *
 * Funktionen:
 *  - CO2, Luftfeuchtigkeit und Temperatur aus dem SCD30 lesen
 *  - auf dem OLED darstellen
 *  - Grenzwertüberschreitung an der RGB LED anzeigen
 *  - Werte über WiFi/TCP über nodejs middleware in influxdb schreiben
 *  - Debugging über Serial 115200
 *
 * Verwendete, zusätzliche Bibliotheken:
 *  - U8g2_u8x8 für OLED (über Bibliotheksverwaltung)
 *  - SparkFun_SCD30_Arduino_Library (über Bibliotheksverwaltung) https://github.com/sparkfun/SparkFun_SCD30_Arduino_Library
 *  - MH-Z19 by Jonathan Dempsey (über Bibliotheksverwaltung) https://github.com/WifWaf/MH-Z19
 */

#define USE_SCD30
//#define USE_MHZ19

// Hier hostname oder statische IP des servers eintragen, auf dem die nodejs middleware läuft
const char *server = "192.168.10.116";

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

      if(co2 > 2000)          // red
        {
          digitalWrite(ledPinRed, HIGH);
          digitalWrite(ledPinGreen, LOW);
          digitalWrite(ledPinBlue, LOW);
        }
      else if(co2 > 1500)     // yellow
        {
          digitalWrite(ledPinRed, HIGH);
          digitalWrite(ledPinGreen, HIGH);
          digitalWrite(ledPinBlue, LOW);
        }
      else                   // green
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

      u8x8.setCursor(0, 5);
      u8x8.printf("%ippm ", co2);
      delay (1000);

      u8x8.setCursor(0, 5);
      u8x8.printf("%.1f", temperature);
      u8x8.print("\xb0");
      u8x8.print("C ");
      delay (1000);

      u8x8.setCursor(0, 5);
      u8x8.printf("%.1f%%rH", humidity);
      delay (1000);

      #define BUF_SIZE 200
      char buf[BUF_SIZE];
      build_JSON (buf, BUF_SIZE, co2, temperature, humidity);
      http_POST (server, 4000, "/addSample", buf);

      u8x8.setCursor(0, 5);
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
