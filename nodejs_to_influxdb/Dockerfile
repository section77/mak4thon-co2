FROM node:15-buster-slim

WORKDIR /home/mode

COPY package.json .
RUN npm install

COPY server.js .

#CMD ["/bin/bash"]
CMD ["nodejs", "server.js"]
