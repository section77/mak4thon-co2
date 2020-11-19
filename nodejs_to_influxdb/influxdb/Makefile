.PHONY: run build clean realclean

# Siehe hier https://hub.docker.com/_/influxdb

run:
	sudo docker run --name=andy1  --rm -p 8086:8086 -v $PWD:/var/lib/influxdb influxdb

build:
	sudo docker build -t influxdb .

clean:
	sudo rm -rf data/ meta/ wal/ influxdb.conf

realclean:
	sudo docker image rm influxdb
  
#~ Snippets
#~   curl -XPOST http://localhost:8086/query --data-urlencode 'q=CREATE DATABASE "test"'
#~ da kommt zwar eine warning, hat aber angelegt
#~   curl -G http://localhost:8086/query --data-urlencode "q=CREATE DATABASE fooba"
#~ client laufen lassen:
#~   sudo docker exec -it andy1 influx
