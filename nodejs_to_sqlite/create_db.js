// Initial Datenbank erstellen
// Voher in bash mit "$ sqlite3 data.db" und Strg-D eine leere Datenbank-Datei erstellen
// Dann $ node create_db.js

const fs = require('fs');
var sqlite3 = require('sqlite3').verbose();

var db = new sqlite3.Database('./data.db');
db.run('CREATE TABLE token ('
    + 'id VARCHAR(255) NOT NULL, '
    + 'PRIMARY KEY (id)'
+ ')');

db.run('CREATE TABLE sample ('
    + 'tokenId VARCHAR(255) NOT NULL,'
    + 'timestamp INT NOT NULL,'
    + 'co2 INT NOT NULL,'
    + 'temperature INT NOT NULL,'
    + 'humidity INT NOT NULL,'
    + 'PRIMARY KEY (tokenId, timestamp),'
    + 'FOREIGN KEY (tokenId) REFERENCES token(id)'
+ ')');
