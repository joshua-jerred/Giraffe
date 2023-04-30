const WebSocket = require("ws");
const parse = require("node:url").parse;
const { v4: uuidv4 } = require("uuid");
const { StatusMessage, parseMessage } = require("giraffe-protocol/socket_schema");

module.exports = async (server, global_state) => {
  const client_wss = new WebSocket.Server({
    noServer: true,
    clientTracking: true,
  });

  const telemetry_wss = new WebSocket.Server({
    noServer: true,
    clientTracking: true,
  });

  client_wss.on("connection", function connection(ws, req) {
    try {
      let query_string = req.url.split("?")[1];
      console.log(query_string);
      let query = query_string.split("=");
      console.log(JSON.stringify(query));
      if (query[0] === "client_name") {
        ws.client_name = query[1];
      } else {
        console.log("Invalid client query parameter");
        ws.close();
      }
    } catch (e) {
      console.log("Invalid query string");
      ws.close();
    }
    ws.id = uuidv4();
    ws.on("message", (msg) => {
      try {
        var message = JSON.parse(msg);
        message = parseMessage(message);
        console.log(message.toString());
      }
      catch (e) {
        console.log(e);
      }
      global_state.ggs_status.total_ws_messages++;
      //ws.send(global_state.total_ws_messages.toString()); // send ack here
    });
    ws.on("close", () => {
      console.log("closed");
    });
  });

  telemetry_wss.on("connection", function connection(ws, req) {
    console.log("Telem connection");
    ws.on("message", (message) => {
      console.log("TELEMETRY" + message);
    });
  });

  server.on("upgrade", (request, socket, head) => {
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
    const status_contents = global_state.getStatus();
    let status_message = new StatusMessage("ggs", status_contents);
    let num_clients = 0;
    let current_clients = [];
    client_wss.clients.forEach(function each(client) {
      if (client.readyState === WebSocket.OPEN) {
        current_clients.push(client.client_name);
        num_clients++;
        client.send(status_message.string());
      }
    });
    global_state.ggs_status.current_clients = current_clients;
    global_state.ggs_status.num_ws_clients = num_clients;
  }, 1000);

  return client_wss;
};
