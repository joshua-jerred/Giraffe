const WebSocket = require("ws");
const parse = require("node:url").parse;
const { v4: uuidv4 } = require("uuid");
const {
  parse: parseMessage,
  StreamResponse,
  ErrorMessage,
} = require("giraffe-protocol");
const ClientDataStreams = require("./streams.js");

/**
 * @brief For sending an error message to a client.
 * @param {ws} ws The websocket to send the error to.
 * @param {string} error_message The error message to send.
 */
function sendError(ws, error_message) {
  let error = new ErrorMessage("ggs", "gwc", error_message);
  ws.send(JSON.stringify(error));
}

module.exports = async (server, global_state) => {
  const client_wss = new WebSocket.Server({
    noServer: true,
    clientTracking: true,
  });
  const streams = new ClientDataStreams(global_state, client_wss);

  const telemetry_wss = new WebSocket.Server({
    noServer: true,
    clientTracking: true,
  });

  /**
   * @brief Called when a client first connects to the websocket server.
   *
   * @note user_name
   */
  client_wss.on("connection", function connection(ws, req) {
    // query_string should be ['/api/ws', 'user_name=<name>']
    let query_parts = req.url.split("?");
    const expected_path = "/api/ws";
    if (query_parts.length !== 2 || query_parts[0] !== expected_path) {
      console.log("Invalid query string.");
      ws.close();
      return;
    }

    // user_name_part should be ['user_name', '<name>']
    let user_name_part = query_parts[1].split("=");
    if (user_name_part.length !== 2 || user_name_part[0] !== "user_name") {
      console.log("Invalid query string. Missing user_name");
      ws.close();
      return;
    }

    let user_name = user_name_part[1];
    // Client name limits: 3-20 characters, alphanumeric, no spaces
    if (
      user_name.length < 3 ||
      user_name.length > 20 ||
      !user_name.match(/^[a-z0-9]+$/i)
    ) {
      console.log("Invalid user name.");
      ws.close();
      return;
    }

    // Generate a unique id and add the client to the streams
    ws.id = uuidv4();
    streams.addClient(ws, user_name);

    /**
     * @brief Called every time a message is received from a client.
     */
    ws.on("message", (msg) => {
      if (ws.id === undefined) {
        sendError(ws, "Client not registered.");
        ws.close();
        return;
      }

      // Try to parse the message, then send it to the 'streams' object
      try {
        var message = JSON.parse(msg);
        message = parseMessage(message);
        if (
          message.src !== "gwc" ||
          message.dst !== "ggs" ||
          message.typ !== "req"
        ) {
          console.log(message.src + " " + message.dst + " " + message.typ);
          throw "Invalid message src, dst, or typ.";
        }

        streams.handleMessage(ws, message); // This takes care of the rest
      } catch (e) {
        sendError(ws, "Invalid JSON message." + e);
        console.log(e);
      }
    });
    ws.on("close", () => {
      streams.removeClient(ws.id);
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
    // const status_contents = global_state.status;
    //   let status_message = new StatusMessage("ggs", status_contents);
    // let num_clients = 0;
    // let current_clients = [];
    // client_wss.clients.forEach(function each(client) {
    //   if (client.readyState === WebSocket.OPEN) {
    //     try {
    //       current_clients.push(global_state.clients[client.id].client_name);
    //       num_clients++;
    //     } catch (e) {
    //       console.log("err");
    //     }
    //     //       //client.send(status_message.string());
    //   }
    // });
    // global_state.ggs_status.current_client_names = current_clients;
    // global_state.ggs_status.num_ws_clients = num_clients;
  }, 1000);

  // Temporary stream implementation
  setInterval(() => {
    // client_wss.clients.forEach(function each(client) {
    //   try {
    //     if (client.readyState === WebSocket.OPEN) {
    //       let streams = global_state.clients[client.id].streams;
    //       for (let i = 0; i < streams.length; i++) {
    //         let stream = streams[i];
    //         let data = global_state.getStreamData(stream);
    //         let message = new StreamResponse("ggs", "gwc", stream, data);
    //         if (data !== undefined) {
    //           client.send(JSON.stringify(message));
    //         }
    //       }
    //     }
    //   } catch (e) {}
    // });
  }, 1000);

  return client_wss;
};
