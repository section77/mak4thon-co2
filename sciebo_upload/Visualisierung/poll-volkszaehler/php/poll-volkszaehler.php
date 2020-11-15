<?php
require 'vendor/autoload.php';
require 'config/room_config.php';
require 'config/poll_config.php';

use InfluxDB\Point;
use InfluxDB\Database;

// on initial Startup influxdb might not be available lets wait a bit
if ($poll_config['delaying_start_seconds']) {
    echo "delaying start" . PHP_EOL;
    sleep($poll_config['delaying_start_seconds']);
}

// get data from volkszahler using REST
$influxdb_client = new InfluxDB\Client($poll_config['influxdb_host'], $poll_config['influxdb_port']);

while (true) {
    try {
        $database = $influxdb_client->selectDB('co2');
        $client = new GuzzleHttp\Client();

        foreach ($room_config as $room_key => $uuids) {
            print date('d.m.Y H:i:s') . " Gathering data for " . $room_key . PHP_EOL;
            /**
             * polling full data for the past 1 day
             */
            $res = $client->request('GET', 'https://demo.volkszaehler.org/middleware/data/'.$uuids['co2'].'.json?from=1+days+ago', [
            ]);

            if ($poll_config['verbose']) {
                echo "http status code" . PHP_EOL;
                echo $res->getStatusCode();
                // "200"
                echo "http header content-type" . PHP_EOL;
                echo $res->getHeader('content-type')[0];
            }
            // 'application/json; charset=utf8'
            $raw_response = $res->getBody();

            $json = json_decode($raw_response, true);

            $points = [];

            /**
             * @var $tuple 0 = microtime, 1 = co2 in ppm, 2?
             */
            print "found " . count ($json['data']['tuples']) . " tuples" . PHP_EOL;
            foreach ($json['data']['tuples'] as $tuple) {

                $data = [
                    'room' => $room_key,
                    // force integer
                    'co2_ppm' => (int)$tuple[1],
                    // data is in ms -> *1000 for ns
                    'timestamp' => (int)$tuple[0] * 1000
                ];

                if ($poll_config['verbose']) {
                    print "point data:" . PHP_EOL;
                    print_r($data);
                }

                $points[] = new Point(
                    'co2measurement', // the name of the measurement
                    null, // measurement value
                    ['room' => $data['room']],
                    ['co2_ppm' => $data['co2_ppm'],], // measurement fields
                    $data['timestamp']
                );
            }

            print "starting write." . PHP_EOL;
            $result = $database->writePoints($points, Database::PRECISION_MICROSECONDS);
            print "done." . PHP_EOL;
            sleep($poll_config['poll_sleep_between_requests']);
        }

        if ($poll_config['verbose']) {
            print "selecting 100 measurements from database". PHP_EOL;

            // executing a query will yield a resultset object
            $result = $database->query('select * from co2measurement LIMIT 100');

            // get the points from the resultset yields an array
            $points = $result->getPoints();
            print_r($points);
        }

    } catch ( Exception $exception ) {
        if (strpos($exception->getMessage(), 'HTTP Code 404 database not found: "co2"') !== FALSE || $exception->getMessage() == 'Query has failed: database not found: co2') {
            print "Database not found." . PHP_EOL;
            print "Creating database." . PHP_EOL;
            $command = 'curl -G http://'.$poll_config['influxdb_host'].':8086/query --data-urlencode "q=CREATE DATABASE co2"';
            print "Executing: $command" . PHP_EOL;
            exec($command);
            echo "Executed. Please retry." . PHP_EOL;
        } else {
            throw $exception;
        }
    }

    print "wait ".$poll_config['poll_interval']." for next poll interval." . PHP_EOL;
    sleep ($poll_config['poll_interval']);
}