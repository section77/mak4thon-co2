/*
  Reading CO2, humidity and temperature from the SCD30
  By: Nathan Seidle
  SparkFun Electronics
  Date: May 22nd, 2018
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  Feel like supporting open source hardware?
  Buy a board from SparkFun! https://www.sparkfun.com/products/15112

  This example prints the current CO2 level, relative humidity, and temperature in C.

  Hardware Connections:
  Attach RedBoard to computer using a USB cable.
  Connect SCD30 to RedBoard using Qwiic cable.
  Open Serial Monitor at 115200 baud.
*/

/**
 * BasicHTTPClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

/*

  GraphicsTest.ino

  Some graphics/text output for U8x8 API

  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright (c) 2016, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification,
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list
    of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice, this
    list of conditions and the following disclaimer in the documentation and/or other
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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

#include <HTTPClient.h>
#include "wlan_settings.h"

#include "uuids.h"

const int ledPinRed = 17; 
const int ledPinGreen = 12; 
const int ledPinBlue = 13; 

#include "SparkFun_SCD30_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_SCD30
SCD30 airSensor;

WiFiMulti wifiMulti;

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);         // FIXME Hiltec...

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

  wifiMulti.addAP(ssid, wlan_password);

  //The SCD30 has data ready every two seconds

  delay(10);

  // We start by connecting to a WiFi network

  pinMode(ledPinRed, OUTPUT);
  pinMode(ledPinGreen, OUTPUT);
  pinMode(ledPinBlue, OUTPUT);

  u8x8.begin();

}

void pre(void) // part of GraphicsTest
{
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_f);
  u8x8.clear();

  u8x8.inverse();
  u8x8.print(" U8x8 Library ");
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.noInverse();
  u8x8.setCursor(0,1);
}

void draw_bar(uint8_t c, uint8_t is_inverse)
{
  uint8_t r;
  u8x8.setInverseFont(is_inverse);
  for( r = 0; r < u8x8.getRows(); r++ )
  {
    u8x8.setCursor(c, r);
    u8x8.print(" ");
  }
}

void draw_ascii_row(uint8_t r, int start)
{
  int a;
  uint8_t c;
  for( c = 0; c < u8x8.getCols(); c++ )
  {
    u8x8.setCursor(c,r);
    a = start + c;
    if ( a <= 255 )
      u8x8.write(a);
  }
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
    Serial.print("co2(ppm):");
    Serial.print(airSensor.getCO2());

    Serial.print(" temp(C):");
    Serial.print(airSensor.getTemperature(), 1);

    Serial.print(" humidity(%):");
    Serial.print(airSensor.getHumidity(), 1);

    Serial.println();

    // wait for WiFi connection
    if((wifiMulti.run() == WL_CONNECTED))
    {
      push_volkszaehler (uuid_co2, airSensor.getCO2());
      push_volkszaehler (uuid_temp, airSensor.getTemperature());
      push_volkszaehler (uuid_humidity, airSensor.getHumidity());
    }
  }
  else
    Serial.println("Waiting for new data");

  pre();
  u8x8.print("CO2:");
  u8x8.setCursor(0,2);
  u8x8.print(airSensor.getCO2());

//  u8g2.firstPage();
//  u8g2.setFont(u8g2_font_ncenB24_tr);
//  u8g2.drawStr(0,24,"Hello World!");
  
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

  delay(1500);

}
