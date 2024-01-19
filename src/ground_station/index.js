// const aprs_fi = require("./aprs_fi/aprs_fi");
const fs = require("node:fs");

const VERSION_FILE_PATH = "../version";
const re = /\bVERSION_NUMBER<([0-9\.]+)>[ \n\r]+STAGE<([A-Za-z ]+)>/g;

const match = re.exec(fs.readFileSync(VERSION_FILE_PATH, "utf8"));
const version_number = match[1];
const stage = match[2];

console.log("version_number: ", version_number);
console.log("stage: ", stage);

// aprs_fi();

// const PORT = 7892;

// const global_state = new (require("./state"))();

// Setup the HTTP and WS servers
// const server = require("./http_api/express_server")(global_state);
// const socket = require("./ws_api/ggs_ws")(server, global_state);
//
// server.listen(PORT);
// module.exports = server;
//
// console.log(
// `Ground Station Server listening on port ${PORT} - http://localhost:${PORT}`
// );
