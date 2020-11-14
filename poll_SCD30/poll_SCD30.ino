#include <WiFi.h>

const char* ssid     = "no_connection";
//const char* password = "PWD_FILL_ME";
#include "wlan_password.h"

const char* host = "192.168.10.116";
const int   port = 2121;

// Use WiFiClient class to create TCP connections
WiFiClient client;

#include "SparkFun_SCD30_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_SCD30
SCD30 airSensor;

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

  // We start by connecting to a WiFi network
  Serial.println("init network");
  WiFi.begin(ssid, password);
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

  Serial.print("connecting to ");
  Serial.println(host);
}

int value = 0;

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
  }
  else
    Serial.println("Waiting for new data");

  if(!client.connected()){
    Serial.println("Open connection to:");
    Serial.println(String(host)+":"+String(port));
    if (!client.connect(host, port))
    {
      Serial.println("connection failed");
      delay(1000);
    }
    Serial.println("connection succided");
    delay(1000);
  }

  //Serial.println("sending Data");
  client.print("CO2 (ppm): ");
  client.print(airSensor.getCO2());
  client.print(", temp(C): ");
  client.print(airSensor.getTemperature(), 1);
  client.print(", humidity(%):");
  client.println(airSensor.getHumidity(), 1);  

  delay(500);
}
