const WebSocket = require("ws");
const parse = require("node:url").parse;
const { v4: uuidv4 } = require("uuid");
const {
  parse: parseMessage,
  StreamResponse,
  ErrorMessage,
} = require("giraffe-protocol");

function SendError(ws, message, error_message) {
  let error = new ErrorMessage("ggs", message.src, error_message, message.id);
  ws.send(JSON.stringify(error));
}

function GetDataItem(ws, message) {
  const valid_systems = ["gfs", "ggs"];

  let params = message.body.params;
  let system = params.system;
  if (!valid_systems.includes(system)) {
    SendError(ws, message, "Invalid System - ClientRequest: " + system);
    return;
  }

  if (system === "gfs") {
  } else if (system === "ggs") {
    console.log("GGS");
  }
}

// Data Stream Request
function ClientStream(ws, message, global_state) {
  let action = message.body.action;
  let stream = message.body.stream;
  console.log("Client Stream Request: " + action + " " + stream);
  if (action === "add" || action === "remove") {
    let streams = global_state.clients[ws.id].streams;
    if (action === "add") {
      if (streams.includes(stream)) {
        return;
      } else {
        streams.push(stream);
      }
    } else if (action === "remove") {
      if (streams.includes(stream)) {
        streams.splice(streams.indexOf(stream), 1);
      } else {
        return;
      }
    }
  } else if (action === "clear_all") {
    global_state.clients[ws.id].streams = [];
  } else {
    SendError(ws, message, "Invalid Action - ClientRequest: " + action);
  }
  //   console.log("Data stream request ");
  //   console.log(message.body);
  //   let sr = message.body;
  //   let streams = global_state.clients[ws.id].streams;

  //   let num_streams = streams.length;
  //   for (let i = 0; i < streams.length; i++) {
  //     if (streams[i] == sr) {
  //       console.log("Stream already exists " + sr);
  //       return;
  //     }
  //   }
  //   streams.push(sr);
  // }
}

function ClientRequest(ws, message, global_state) {
  let cat = message.cat;

  if (cat === "data") {
    return GetDataItem(ws, message);
  } else if (cat === "stream") {
    return ClientStream(ws, message, global_state);
  } else {
    SendError(ws, message, "Invalid Message Category - ClientRequest: " + cat);
  }
}

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
        if (name in global_state.current_client_names) {
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
          streams: ["status"],
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
      global_state.ggs_status.total_ws_messages++;
      if (ws.id === undefined || !(ws.id in global_state.clients)) {
        console.log("Client not registered");
        return;
      }

      try {
        var message = JSON.parse(msg);
        global_state.clients[ws.id].queries++;

        message = parseMessage(message);
        if (message.src === "client") {
          if (message.typ === "info") {
            if (message.cat === "path") {
              global_state.clients[ws.id].current_path = message.body.path;
              global_state.clients[ws.id].streams = ["status"];
            }
            return;
          } else if (message.typ === "req") {
            ClientRequest(ws, message, global_state);
            return;
          }
        } else if (message.src === "telemetry") {
          // Todo
        } else {
          console.log("Invalid message source");
          return;
        }
        //console.log(message);

        // Path message
        // if (message.typ == "path") {
        //   global_state.clients[ws.id].current_path = message.body;
        //   console.log(global_state.clients[ws.id]);
      } catch (e) {
        let err = new ErrorMessage("ggs", "client", e.toString(), message.id);
        ws.send(JSON.stringify(err));
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

  // Update the status of GGS
  setInterval(() => {
    const status_contents = global_state.status;
    //   let status_message = new StatusMessage("ggs", status_contents);
    let num_clients = 0;
    let current_clients = [];
    client_wss.clients.forEach(function each(client) {
      if (client.readyState === WebSocket.OPEN) {
        current_clients.push(global_state.clients[client.id].client_name);
        num_clients++;
        //       //client.send(status_message.string());
      }
    });
    global_state.ggs_status.current_client_names = current_clients;
    global_state.ggs_status.num_ws_clients = num_clients;
  }, 1000);

  // Temporary stream implementation
  setInterval(() => {
    client_wss.clients.forEach(function each(client) {
      if (client.readyState === WebSocket.OPEN) {
        let streams = global_state.clients[client.id].streams;
        for (let i = 0; i < streams.length; i++) {
          let stream = streams[i];
          try {
            let data = global_state.getStreamData(stream);
            let message = new StreamResponse("ggs", "client", stream, data);
            if (data !== undefined) {
              client.send(JSON.stringify(message));
            }
          } catch (e) {
            console.log(e);
          }
        }
      }
    });
  }, 1000);

  return client_wss;
};
