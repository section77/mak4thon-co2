# Docker Image for running influxdb and grafana

https://hub.docker.com/r/philhawthorne/docker-influxdb-grafana/

```
docker pull philhawthorne/docker-influxdb-grafana
```

# Quick Start

docker run -d \
  --name docker-influxdb-grafana \
  -p 3003:3003 \
  -p 3004:8083 \
  -p 8086:8086 \
  -v storage:/var/lib/influxdb \
  -v storage:/var/lib/grafana \
  philhawthorne/docker-influxdb-grafana:latest
  
# create infludb database

```
CREATE DATABASE "co2"
```

# upload data to influxdb options

https://www.influxdata.com/blog/getting-started-writing-data-to-influxdb/

## 1. Uploading via Chronograf

Access Cronograf on localhost:3004

Click Explore

Click Write top right corner

Drag n Drop co2Data.txt

SELECT * FROM "co2"."autogen"."co2measurement"

# configure grafana to connect to influxdb

1) Add datasource

URL http://localhost:8086
Database co2

Save and test