const Validate = require("giraffe-protocol/message").validate;

const telemetryController = require("./controllers/telemetry");
const clientController = require("./controllers/client");

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


