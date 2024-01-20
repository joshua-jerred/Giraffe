const PORT = 7892;

const global_state = new (require("./state"))();

// Setup the HTTP express server
const server = require("./http_api/express_server")(global_state);

server.listen(PORT);
module.exports = server;

console.log(
  `Ground Station Server listening on port ${PORT} - http://localhost:${PORT}`
);
