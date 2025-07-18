const global_state = new (require("./state"))();
const file_paths = require("./file_paths");

file_paths.prepareFileSystem();

const PORT = 7892;

// Setup the HTTP express server
const server = require("./http_api/express_server")(global_state);

server.listen(PORT);
module.exports = server;

console.log(
  `Ground Station Server listening on port ${PORT} - http://localhost:${PORT}`
);
