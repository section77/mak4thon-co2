const express = require('express');
const app = express();
const port = 3000;
const os = require('os')

var bodyParser = require('body-parser');
app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());

const database_name = 'sample_school';

const Influx = require('influx');
const db = new Influx.InfluxDB({
  host: 'localhost',
  database: database_name,
  schema: [
    {
      measurement: 'indoor_air_quality',
      fields: {
        token: Influx.FieldType.STRING,
        co2: Influx.FieldType.FLOAT,
        temperature: Influx.FieldType.FLOAT,
        humidity: Influx.FieldType.FLOAT
      },
      tags: [
        'token'
      ]
    }
  ]
})

db.getDatabaseNames()
  .then(names => {
    console.log ({names});
    if (!names.includes(database_name)) {
      console.log ('creating new database', database_name);
      return db.createDatabase(database_name);
    }
  })
  .then(() => {
    app.listen(port, () => {
        console.log(`Example app listening at http://localhost:${port}`);
    })
  })
  .catch(err => {
    console.log({err});
    console.error(`Error creating Influx database!`);
  })

app.post('/addSample', (req, res) => {

    const entry = {
        measurement: 'indoor_air_quality',
        tags: { token: req.body.token },
        fields: req.body
        };
    delete entry.fields.token;

    console.log(req.path, entry);

    db.writePoints([entry]).catch(err => {
      console.error(`Error saving data to InfluxDB! ${err.stack}`)
    })

    res.send('ok');
});

app.get('/', function (req, res) {
  res.end('This ist node_influxdb/server.js. Please try /select_all');
})

app.get('/select_all', function (req, res) {
  db.query(`
    select * from indoor_air_quality
  `).then(result => {
    res.json(result)
  }).catch(err => {
    res.status(500).send(err.stack)
  })
})

//    where host = ${Influx.escape.stringLit(os.hostname())}
//    order by time desc
//    limit 10
