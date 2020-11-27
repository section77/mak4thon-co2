const express = require('express');
const app = express();
const port = 4000;
const os = require('os')
const fs = require('fs');

var bodyParser = require('body-parser');
app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());

const database_name = 'sample_school';

const Influx = require('influx');
const db = new Influx.InfluxDB({
  host: 'influxdb',
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
        'token', 'id'
      ]
    }
  ]
})

var connected = false;
setInterval (function (){
  if (! connected) {
    db.ping(5000).then(hosts => {
      hosts.forEach(host => {
        if (host.online) {
          console.log(`${host.url.host} responded in ${host.rtt}ms running ${host.version})`)
          db.getDatabaseNames()
            .then(names => {
              console.log ({names});
              connected = true;
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
        } else {
          console.log(`${host.url.host} is offline :(`)
        }
      })
    })
  }
}, 1000)

const token_mapping_raw = fs.readFileSync('mapping.json');
var token_mapping = JSON.parse(token_mapping_raw);
console.log(token_mapping_raw);

app.post('/addSample', (req, res) => {
  const t = req.body.token;
  if (token_mapping.map((ele) => ele.token).includes(t))
    {
      const entry = {
          measurement: 'indoor_air_quality',
          tags: { token: t,
                  id: req.body.id },
          fields: req.body
          };
      delete entry.fields.token;
      delete entry.fields.id;

      console.log(req.path, entry);

      db.writePoints([entry]).catch(err => {
        console.error(`Error saving data to InfluxDB! ${err.stack}`)
      })
      res.send('ok');
    }
  else
    {
      console.log ('Reject token', t); 
      res.sendStatus(403);
    }
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
