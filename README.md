# mak-thon

# Grafana Docker Setup

Current implementation uses volkszaehler for gathering sensor data.

Configure Polling settings in poll-volkszaehler/php/config. See [README](poll-volkszaehler/README.md).

```
docker-compose up
``

# on co2

co2 is measured in ppm (parts per million)


data ranges from 250 ppm (normal ambient air) - > 40.000 ppm (toxic) 
https://www.kane.co.uk/knowledge-centre/what-are-safe-levels-of-co-and-co2-in-rooms


sanitary safe levels of co2 in rooms:

| co2 concentration (ppm) | sanitary rating | suggestion                       |
|:-------------:|:-------------------:|:--------------------------------------:|
| < 1000        | uncritical          | no actions needed                      |
| 1000–2000     | noticeable          | ventilation needed                     |
| >2000         | unacceptable        | additional ventilation measures needed |

source: https://www.umweltbundesamt.de/sites/default/files/medien/pdfs/kohlendioxid_2008.pdf

in regards to covid:

| #     | co2 concentration (ppm) | color       |
|:-----:|:-----------------------:|:-----------:|
| IDA 1 |  350 - 549              | green       |
| IDA 2 |  550 – 1049             | light green |
| IDA 3 | 1050 - 1549             | yellow      |
| IDA 4 | > 1550                  | red         |

source: ["Risikobewertung von virenbeladenen Aerosolen anhand der CO2-Konzentration"](co2-colors.png)
Autoren: Anne Hartmann, Martin Kriegel
Technische Universität Berlin, Hermann-Rietschel-Institut
DOI: http://dx.doi.org/10.14279/depositonce-10361

