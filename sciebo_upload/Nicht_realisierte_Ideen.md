Weitere Ideen, die es nicht in den Prototypen geschafft haben

## Gehäuse
- Halterungen für die Bauteile (im Prototyp nur geklebt)
- kompakter durch passende Leitungslängen oder Platine
- Gehäuse für die LoRa-Wifi-Bridge
- für Grundschulen eine andere Gehäusevariante: Ein 3D gedrucktes Haus mit Fenster. Ein Servo öffnet die Fenster bei überschreiten des Grenzwerts (anstelle einer roten LED)

## Elektronik
- Einbau eines LiPo-Akkus → mobiler Betrieb; Controller hat schon Laderegler integriert

## Software
- ESPs in den Klassenräumen:
  * Lineare Regression über die letzten 5min C02 Werte um die voraussichtliche Zeit bis zum Überschreiten des Grenzwertes zu berechnen.
  * Abfallen der CO2 Konzentration erkennen und den asymptotischen Verlauf erkennen. Empfehlung zum Schließen der Fenster geben, wenn Änderungsrate unter Schwellwert fällt. Hintergrund Energiesparen, nur so lange Lüften wie auch Sinn macht.

## Rollout
- automatische Erzeugung von UUIDs, die das Flashen mit identischen Binaries erlaubt (vzero)

