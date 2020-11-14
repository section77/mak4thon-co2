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


#include <Wire.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>

#include "wlan_settings.h"
#include "uuids.h"
#include "SparkFun_SCD30_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_SCD30


const int ledPinRed = 17; 
const int ledPinGreen = 12; 
const int ledPinBlue = 13; 


SCD30 airSensor;
WiFiMulti wifiMulti;


void setup() {
  Serial.begin(115200);
  Serial.println("SCD30 Example");
  Wire.begin();

  if (airSensor.begin() == false) {
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    while (1){};
  }

  if (wifiMulti.addAP(ssid, wlan_password) == false) {
    Serial.println("WiFi ssid or passwort not correct. Please check wlan_settings.h. Freezing ...");
    while(true){};
  }

  //The SCD30 has data ready every two seconds

  delay(10);

  // We start by connecting to a WiFi network

  pinMode(ledPinRed, OUTPUT);
  pinMode(ledPinGreen, OUTPUT);
  pinMode(ledPinBlue, OUTPUT);
}

void push_volkszaehler(const char* UUID, float value) {
  HTTPClient http;

  //Serial.print("[HTTP] begin...\n");
  char buf[300];
  snprintf (buf, 300, "http://demo.volkszaehler.org/middleware/data/%s.json?operation=add&value=%.2f", UUID, value);
  
  // debugging
  //Serial.print (buf);
  http.begin(buf);

  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      //Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        //Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

  http.end();
}

void loop() {
  if (airSensor.dataAvailable()) {
    Serial.print("co2(ppm):");
    Serial.print(airSensor.getCO2());

    Serial.print(" temp(C):");
    Serial.print(airSensor.getTemperature(), 1);

    Serial.print(" humidity(%):");
    Serial.print(airSensor.getHumidity(), 1);

    Serial.println();

    // wait for WiFi connection
    if ((wifiMulti.run() == WL_CONNECTED)) {
      push_volkszaehler (uuid_co2, airSensor.getCO2());
      push_volkszaehler (uuid_temp, airSensor.getTemperature());
      push_volkszaehler (uuid_humidity, airSensor.getHumidity());
    }
  } else {
    Serial.println("Waiting for new data");
  }

  if (airSensor.getCO2() > 1000) { // red
      digitalWrite(ledPinRed, HIGH);    
      digitalWrite(ledPinGreen, LOW);
      digitalWrite(ledPinBlue, LOW);
  } else if (airSensor.getCO2() > 800) {  // yellow
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
