.PHONY: realclean

run: node_modules data.db
	node server.js

node_modules:
	npm install

data.db:
	node create_db.js

realclean:
	rm -rf node_modules
	rm -f data.db
