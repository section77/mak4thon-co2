# Setup your rooms

Config file: `php/config/poll_config.php`

```
    'Raum 1' => [
        // Andre
        'co2' => '6f11eaa0-260d-11eb-a5eb-73e0dd6f6423',
    ],
    'Raum 2' => [
        // Justin
        'co2' => '84c96190-25f8-11eb-8265-d7d703fabc64',
    ],
```

Array key is used as label in Grafana / tag for measurements

# Polling configuration

Config file: `php/config/poll_config.php`

```
    'influxdb_host' => 'localhost',
    'influxdb_port' => '8086',
    'poll_interval' => 30,
    'poll_sleep_between_requests' => 2,
    'verbose' => FALSE,
    'delaying_start_seconds' => 10,
```

`delaying_start_seconds` can be helpful to not have supervisor terminate on first startup. use 0 or false to not delay start.

# Debugging / Logs

```
docker ps
docker exec -it <container-id> bash

```