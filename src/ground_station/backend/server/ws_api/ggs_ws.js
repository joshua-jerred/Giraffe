const WebSocket = require("ws");
const parse = require("node:url").parse;
const { v4: uuidv4 } = require("uuid");
const {
  StatusMessage,
  parseMessage,
} = require("giraffe-protocol/socket_schema");

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
      // Get the client name from the query string 'client_name=...'
      let query_string = req.url.split("?")[1];
      let query = query_string.split("=");
      if (query[0] === "client_name") {
        let name = query[1];

        // Ensure that the client name is not already in use.
        if (name in global_state.client_names) {
          console.log("Client name already in use " + name);
          ws.close();
          // Check if the client name is in the database
        } else if (name in global_state.all_client_names) {
          ws.id = global_state.all_client_names[name];
        } else {
          console.log("New client name " + name);
          ws.id = uuidv4();
          global_state.all_client_names[name] = ws.id;
          global_state.saveData();
        }
        console.log("Client connected " + ws.id);
        global_state.clients[ws.id] = {
          client_name: name,
          current_path: "",
          queries: 0,
        };

      } else {
        console.log("Invalid client query parameter");
        ws.close();
      }
    } catch (e) {
      console.log("Invalid query string" + e);
      ws.close();
    }

    ws.on("message", (msg) => {
      if (ws.id === undefined || !(ws.id in global_state.clients)) {
        console.log("Client not registered");
        // wait
        return;
      }

      try {
        var message = JSON.parse(msg);
        message = parseMessage(message);
        global_state.clients[ws.id].queries++;
        if (message.type == "path") {
          global_state.clients[ws.id].current_path = message.body;
        }
      } catch (e) {
        console.log(e);
      }
      global_state.ggs_status.total_ws_messages++;
      //ws.send(global_state.total_ws_messages.toString()); // send ack here
    });
    ws.on("close", () => {
      console.log("closed");
      console.log(global_state.clients);
      delete global_state.clients[ws.id];
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
