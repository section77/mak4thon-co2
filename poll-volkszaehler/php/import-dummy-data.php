<?php
require 'vendor/autoload.php';

use InfluxDB\Point;
use InfluxDB\Database;

$host = 'localhost';
$port = '8086';

$client = new InfluxDB\Client($host, $port);


    $database = $client->selectDB('co2');

try {




/*
 * Writing to DB
 */

/*
co2measurement co2_ppm=500,room="Raum 1" 1605299357000000000
co2measurement co2_ppm=510,room="Raum 1" 1605300124000000000
co2measurement co2_ppm=520,room="Raum 1" 1605300305000000000
co2measurement co2_ppm=1100,room="Raum 1" 1605302367000000000
co2measurement co2_ppm=1200,room="Raum 1" 1605302417000000000
co2measurement co2_ppm=1250,room="Raum 1" 1605302491000000000
co2measurement co2_ppm=2010,room="Raum 1" 1605302567000000000
co2measurement co2_ppm=2050,room="Raum 1" 1605302667000000000
co2measurement co2_ppm=400,room="Raum 2" 1605299357000000001
co2measurement co2_ppm=450,room="Raum 2" 1605300124000000001
co2measurement co2_ppm=520,room="Raum 2" 1605300305000000001
co2measurement co2_ppm=1010,room="Raum 2" 1605302367000000001
co2measurement co2_ppm=1050,room="Raum 2" 1605302417000000001
co2measurement co2_ppm=1200,room="Raum 2" 1605302491000000001
co2measurement co2_ppm=3010,room="Raum 2" 1605302567000000001
co2measurement co2_ppm=3020,room="Raum 2" 1605302667000000001
co2measurement co2_ppm=400,room="Raum 3" 1605299357000000002
co2measurement co2_ppm=450,room="Raum 3" 1605300124000000002
co2measurement co2_ppm=520,room="Raum 3" 1605300305000000002
co2measurement co2_ppm=600,room="Raum 3" 1605302367000000002
co2measurement co2_ppm=500,room="Raum 3" 1605302417000000002
co2measurement co2_ppm=450,room="Raum 3" 1605302491000000002
co2measurement co2_ppm=500,room="Raum 3" 1605302567000000002
co2measurement co2_ppm=550,room="Raum 3" 1605302667000000002
*/

    // create an array of points
    $points = array(
        new Point(
            'co2measurement', // the name of the measurement
            null, // measurement value
            ['room' => 'Raum 1'],
            ['co2_ppm' => 500,], // measurement fields
            1605299357000000000
        ),
        new Point(
            'co2measurement', // the name of the measurement
            null, // measurement value
            ['room' => 'Raum 1'],
            ['co2_ppm' => 510,], // measurement fields
            1605300124000000000
        ),
    );

    // we are writing unix timestamps, which have a second precision
    $result = $database->writePoints($points, Database::PRECISION_NANOSECONDS);

    list($usec, $sec) = explode(' ', microtime());
    $timestamp = sprintf('%d%06d', $sec, $usec*1000000);

    $points = array(
        new Point(
            'co2measurement', // the name of the measurement
            null, // measurement value
            ['room' => 'Raum 1'],
            ['co2_ppm' => 468,], // measurement fields
            $timestamp,
        ),
        new Point(
            'co2measurement', // the name of the measurement
            null, // measurement value
            ['room' => 'Raum 2'],
            ['co2_ppm' => 468,], // measurement fields
            $timestamp,
        ),
        new Point(
            'co2measurement', // the name of the measurement
            null, // measurement value
            ['room' => 'Raum 3'],
            ['co2_ppm' => 468,], // measurement fields
            $timestamp,
        ),
    );

    // we are writing unix timestamps, which have a second precision
    $result = $database->writePoints($points, Database::PRECISION_MICROSECONDS);

    // executing a query will yield a resultset object
    $result = $database->query('select * from co2measurement LIMIT 100');

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