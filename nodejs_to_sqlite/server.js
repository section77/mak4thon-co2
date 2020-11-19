const express = require('express');
const app = express();
const port = 3000;
const fs = require('fs');

var bodyParser = require('body-parser');
//app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());

try {
    fs.accessSync('data.db');
} catch (err) {
    console.log('Bitte vorher $node create_db.js ausführen');
    process.exit(1);
}

var sqlite3 = require('sqlite3').verbose();
var db = new sqlite3.Database('./data.db');

app.get('/', (req, res) => {
    console.log('GET');
    var html = '<h1>CO2-Ampel Usage</h1>';
    html += '<p><a href="/getTokens">/getTokens</a></p>';
    html += '<p>/getToken/:token/:from/:to , z.B. <a href="/getToken/abc/null/null">/getToken/abc/null/null</a> oder <a href="/getToken/abc/1605708483579/1605708483579">/getToken/abc/1605708483579/1605708483579</a></p>';
    html += '<p>/addToken POST:</p>';
    html += '<p><form action="/addSample" method="post">';
    html += 'Token: <input value="abc" type="text" name="token" /><br>';
    html += 'CO2:<input value="1" type="number" name="co2" /><br>';
    html += 'Temperature:<input value="2" type="number" name="temperature" /><br>';
    html += 'Humidity:<input value="3" type="number" name="humidity" /><br>';
    html += '<button type="submit">Add</button></form></p>';
    res.send(html);
});

// http://localhost:3000/getTokens
app.get('/getTokens', (req, res) => {
    db.all('SELECT id FROM token', function(err, tokens) {
        res.json(tokens);
    });
});

// http://localhost:3000/getToken/abc/null/null
app.get('/getToken/:token/:from/:to', (req, res) => {
    var query = 'SELECT * FROM sample WHERE tokenId = ?';
    var args = [req.params.token];

    if (req.params.from !== 'null') {
        query += ' AND timestamp >= ?';
        args.push(parseInt(req.params.from));
    }

    if (req.params.to !== 'null') {
        query += ' AND timestamp <= ?';
        args.push(parseInt(req.params.to));
    }

    db.all(query, ...args, function(err, db_result) {
        res.json(db_result);
    });
});

app.post('/addSample', (req, res) => {
    console.log('New data arrived', req.body);

    // 1. Token anlegen, wenn nicht vorhanden
    db.get('SELECT id FROM token WHERE id = ?', req.body.token, function(err, exist_res) {
        if (exist_res === undefined) {
            db.run('INSERT INTO token VALUES("' + req.body.token + '")');
        }

        // 2. Sample einfügen
        db.run('INSERT INTO sample VALUES('
            + '"' + req.body.token + '",'
            + '"' + Date.now() + '",'
            + '"' + req.body.co2 + '",'
            + '"' + req.body.temperature + '",'
            + '"' + req.body.humidity + '")'
        );
    });

    res.send('ok');
});

app.listen(port, () => {
    console.log(`Example app listening at http://localhost:${port}`);
});
