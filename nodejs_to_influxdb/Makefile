.PHONY: run realclean

run: node_modules
	node server.js

node_modules:
	npm install

realclean:
	rm -rf node_modules package-lock.json
	$(MAKE) -C influxdb $@
