/*
 * Für Hiltec WiFi LoRa 32 V2
 * Zeigt SCD30 CO2, Temperatur, rH auf dem Display an
 */

#include <Arduino.h>
#include <U8x8lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif


// Please UNCOMMENT one of the contructor lines below
// U8x8 Contructor List
// The complete list is available here: https://github.com/olikraus/u8g2/wiki/u8x8setupcpp
// Please update the pin numbers according to your setup. Use U8X8_PIN_NONE if the reset pin is not connected
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16); 	      // FIXME Hiltec...

#include "SparkFun_SCD30_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_SCD30
SCD30 airSensor;

void setup(void)
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

  u8x8.begin();
  //u8x8.setFlipMode(1);

  //u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.setFont(u8x8_font_courB18_2x3_f);

}

void loop(void)
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

    u8x8.clear();
    u8x8.setCursor(0, 5);
    u8x8.printf("%ippm", airSensor.getCO2());
    delay (1200);

    u8x8.clear();
    u8x8.setCursor(0, 5);
    u8x8.printf("%.1f", airSensor.getTemperature());
    u8x8.print("\xb0");
    u8x8.print("C");
    delay (1200);

    u8x8.clear();
    u8x8.setCursor(0, 5);
    u8x8.printf("%.1f%%rH", airSensor.getHumidity());
    delay (1200);
  }
  else
  {
     Serial.println("Waiting for new data");
     delay(3000);
  }


}
