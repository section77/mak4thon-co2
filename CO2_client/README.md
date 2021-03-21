# Software für die Module in den Klassenräumen

Siehe auch den Header von CO2-api.ino

## Konfiguration:

Die zwei .h.template kopieren, in .h umbenennen und die Zugangsdaten eintragen oder so lassen, falls kein WLAN vorhanden ist.
Dann wird automatisch LoRa verwendet.

## Verdrahtung:
  ../docs/CO2\_Sender\_ESP32_LoRa.pdf

## Links:
  Heltec repo mit Schaltplänen PinoutDiagram usw. -> Sehr zu empfehlen
  https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series.git

## Versorgungsspannung oder Batteriespannung lesen?

In WiFi_LoRa_32_V2(433,470-510).PDF sieht man, dass an Pin13 ein Spannungsteiler
von VBat hängt, wahrscheinlich gedacht um die Batteriespannung zu überwachen.
  http://community.heltec.cn/t/heltec-esp32-wifi-kit-example-to-read-a-voltage/115/2

Aber Pin 13 hängt auf ADC2 und den kann man nicht zusammen mit WiFi verwenden:
  "ADC2 is used by the Wi-Fi driver. Therefore the application can only use ADC2 when the Wi-Fi driver has not started."
  https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc.html

Deshalb gibt es von dem heltec Modul eine Version V2.1 wo der Pin von 13 auf 37 (ADC1) geändert wurde:
  http://community.heltec.cn/t/heltec-wifi-lora-v2-battery-management/147/18
