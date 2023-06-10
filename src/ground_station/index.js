const PORT = 3020;

const global_state = new (require("./state"));

// Setup the HTTP and WS servers
const server = require("./http_api/express_server")(global_state);
const socket = require("./ws_api/ggs_ws")(server, global_state);

server.listen(PORT);
module.exports = server;