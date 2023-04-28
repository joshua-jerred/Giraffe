const WebSocketServer = require("ws").Server;
const WebSocket = require("ws");
const { v4: uuidv4 } = require("uuid");

const Validate = require("giraffe-protocol/message").validate;

const telemetryController = require("./controllers/telemetry");
const clientController = require("./controllers/client");

const ggs_ws = new WebSocketServer({ port: 8000 });

const Status = require("./status");
const ggs_status = new Status();

ggs_ws.on("connection", function connection(ws, req) {
  sendStatus();
  ws.on("error", console.error);
  ws.type = "unknown";
  ws.id = uuidv4();

  const ip = req.socket.remoteAddress;
  console.log("connected to client: " + ip);

  ws.on("message", function message(data) {
    var msg = JSON.parse(data);
    if (Validate(msg)) {
      switch (msg.source) {
        case "telemetry":
          ws.type = "telemetry";
          telemetryController(msg);
          break;
        case "client":
          ws.type = "client";
          if (msg.type === "path") {
            ws.path = msg.body.path;
            console.log("client path: " + ws.path);
            break;
          }
          clientController(msg, ws);
          break;
      }
    } else {
      console.log("invalid message");
    }
  });
});

console.log("GGS listening on port 8000...");

async function sendStatus() {
  const statusMessage = ggs_status.getStatusMessage();
  let num_clients = 0;
  let telemetry = false;
  ggs_ws.clients.forEach(function each(client) {
    if (client.readyState === WebSocket.OPEN) {
      if (client.type === "client") {
        num_clients++;
        client.send(statusMessage);
      } else if (client.type === "telemetry") {
        telemetry = true;
      }
    }
  });
  ggs_status.num_clients = num_clients;
  ggs_status.telemetry_connection = telemetry ? "connected" : "disconnected";
}

setInterval(sendStatus, 1500);
