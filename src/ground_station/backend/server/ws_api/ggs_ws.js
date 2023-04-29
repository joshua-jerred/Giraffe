const WebSocket = require("ws");
const querystring = require("node:querystring");
const parse = require("node:url").parse;
const { v4: uuidv4 } = require("uuid");

module.exports = async (server, global_state) => {
  const client_wss = new WebSocket.Server({
    noServer: true,
    clientTracking: true,
  });

  const telemetry_wss = new WebSocket.Server({
    noServer: true,
    clientTracking: true,
  });

  client_wss.on("connection", function connection(ws, connectionRequest) {
    console.log("connection");
    ws.id = uuidv4();
    ws.on("message", (message) => {
      const msg = message.toString();
      console.log(msg);
      global_state.total_ws_messages++;
      ws.send(global_state.total_ws_messages.toString());
    });
    ws.on("close", () => {
      console.log("closed");
    });
  });

  telemetry_wss.on("connection", function connection(ws, connectionRequest) {
    console.log("connection");
    ws.on("message", (message) => {
      console.log(message);
      ws.send(JSON.stringify({ message: "There be gold in them thar hills." }));
    });
  });

  server.on("upgrade", (request, socket, head) => {
    console.log("upgrade");
    const { pathname } = parse(request.url);

    if (pathname === "/api/ws") {
      client_wss.handleUpgrade(request, socket, head, function done(ws) {
        client_wss.emit("connection", ws, request);
      });
    } else if (pathname === "/api/telemetry/ws") {
      telemetry_wss.handleUpgrade(request, socket, head, function done(ws) {
        telemetry_wss.emit("connection", ws, request);
      });
    } else {
      socket.destroy();
    }
  });

  setInterval(() => {
    const statusMessage = global_state.getStatusMessage();
    let num_clients = 0;
    client_wss.clients.forEach(function each(client) {
      if (client.readyState === WebSocket.OPEN) {
        num_clients++;
        client.send(statusMessage);
      }
    });
    global_state.num_clients = num_clients;
  }, 1000);

  return client_wss;
};
