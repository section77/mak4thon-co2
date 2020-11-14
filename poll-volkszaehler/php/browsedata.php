<?php
require 'vendor/autoload.php';

use InfluxDB\Point;
use InfluxDB\Database;

$host = 'localhost';
$port = '8086';

$client = new InfluxDB\Client($host, $port);


$database = $client->selectDB('co2');

try {
    // executing a query will yield a resultset object
    $result = $database->query('select * from co2measurement LIMIT 1000');

    // get the points from the resultset yields an array
    $points = $result->getPoints();

    print_r($points);

} catch ( Exception $exception ) {
    if (strpos($exception->getMessage(), 'HTTP Code 404 database not found: "co2"') !== FALSE || $exception->getMessage() == 'Query has failed: database not found: co2') {
        exec('curl -G http://localhost:8086/query --data-urlencode "q=CREATE DATABASE co2"');

        echo "Database was not found. Created DB. Please retry." . PHP_EOL;
    } else {
        throw $exception;
    }
}