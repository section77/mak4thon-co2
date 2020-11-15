# Grafana Dashboard with influxDB

# default login

User: admin  
Password: admin

for some obscure reason you might have to refresh after entering credentials once / enter them twice.

## export datasources yaml

https://github.com/trivago/hamara

requires golang to be installed

```
$ go get -u github.com/trivago/hamara
$ hamara
```

Create API Key in Grafana Dashboard

Now export datasources using hamara with api key.

```
export GRAFANA_API_KEY=<your API key here>
hamara export --host=localhost:3003 --key=$GRAFANA_API_KEY > provisioning/datasources/all.yml
cat provisioning/datasources/all.yml
```

# influxdb

time series is measured in ns by default. Normal unixtimestamp * 1000000000

Initial setup of db:

```
$ curl -G http://localhost:8086/query --data-urlencode "q=CREATE DATABASE co2"
```

# updating dashboards

requires jq to be installed.

```
$ ./update-dashboards.sh
``