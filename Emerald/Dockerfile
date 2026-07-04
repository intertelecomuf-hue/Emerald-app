FROM node:18-alpine

# Create app directory
WORKDIR /usr/src/app

# Install app dependencies
COPY package.json package-lock.json* ./
RUN npm install --production --silent || npm install --silent

# Bundle app source
COPY . .

EXPOSE 3000

CMD ["node", "server.js"]
