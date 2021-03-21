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
 *  - Debugging über Serial 115200
 *
 * Verwendete, zusätzliche Bibliotheken:
 *  - U8g2_u8x8 für OLED (über Bibliotheksverwaltung)
 *  - SparkFun_SCD30_Arduino_Library (über Bibliotheksverwaltung) https://github.com/sparkfun/SparkFun_SCD30_Arduino_Library
 *  - MH-Z19 by Jonathan Dempsey (über Bibliotheksverwaltung) https://github.com/WifWaf/MH-Z19
 */

#define USE_SCD30
//#define USE_MHZ19

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

// Hardwaresettings
const int ledPinRed = 17;     // Nico: 32
const int ledPinGreen = 12;   // Nico: 33
const int ledPinBlue = 13;    // Nico: 12

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

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

  // OLED
  u8x8.begin();
  u8x8.setFont(u8x8_font_courB18_2x3_f);
  u8x8.setCursor(0, 0);

  // RGB LED Ports konfigurieren
  pinMode(ledPinRed, OUTPUT);
  pinMode(ledPinGreen, OUTPUT);
  pinMode(ledPinBlue, OUTPUT);

}

int disp = 0;

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

      u8x8.setCursor(0, 1);
      if (disp)
        {
          u8x8.printf("%4.1f", temperature);
          u8x8.print("\xb0");
          u8x8.print("C ");
        }
      else
        {
          u8x8.printf("%4.1f%%rH", humidity);
        }
      disp = ! disp;

      u8x8.setCursor(0, 5);
      u8x8.printf("%4ippm ", co2);
      delay (3000);

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
