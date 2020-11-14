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

// Singleton instance of the radio driver
RH_RF95 rf95 (5, 2);  // Adafruit RFM9x am ESP32 ohne Display

int led = 9;

void setup()
{
  pinMode(led, OUTPUT);
  Serial.begin(115200);
  //while (!Serial) ; // Wait for serial port to be available
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
      Serial.print("got request: ");
      Serial.println((char*)buf);
//      Serial.print("RSSI: ");
//      Serial.println(rf95.lastRssi(), DEC);

      // Send a reply
      //uint8_t data[] = "Danke!";
      //rf95.send(data, sizeof(data));
      //rf95.waitPacketSent();
      //Serial.println("Sent a reply");
      // digitalWrite(led, LOW);
    }
    else
    {
      Serial.println("recv failed");
    }
  }
}
