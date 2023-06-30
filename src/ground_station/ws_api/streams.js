/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   streams.js
 * @brief  This handles all "data streams" from the GWC clients to the GGS.
 *
 * @details A data stream is simply a set of json data that is continuously
 * sent to the client. A client can sign up for a data stream by sending a
 * message to the GGS through the websocket.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-11
 * @copyright  2023 (license to be defined)
 */

const WebSocket = require("ws");
const { ErrorMessage, StreamResponse } = require("giraffe-protocol");

const USER_STATS = {
  user_name: "",
  times_connected: 1,
};

/**
 * @brief For sending an error message to a client.
 * @param {ws} ws The websocket to send the error to.
 * @param {string} error_message The error message to send.
 */
function sendError(ws, error_message) {
  let error = new ErrorMessage("ggs", "gwc", error_message);
  ws.send(JSON.stringify(error));
}

class ClientDataStreams {
  constructor(global_state, web_socket_server) {
    this.gs = global_state;
    this.wss = web_socket_server; // only used in #cycle

    // contains all connected clients and their streams
    this.clients = {};

    // Contains users and their statistics. Users may have multiple clients.
    this.users = this.gs.ggs_db.get("data", "streams", "users");

    this.stats = {
      num_current_clients: 0,
      num_current_streams: 0,
      num_current_users: 0,
      current_users: [],
    };

    this.update_interval = this.gs.ggs_db.get(
      "settings",
      "ggs_server",
      "stream_rate"
    );

    this.update_timer = setInterval(
      this.#cycle.bind(this),
      this.update_interval
    );
  }

  /**
   * @brief Adds a client to the list of clients with an empty stream list.
   * If the user name is not in the list of users, it will be added.
   *
   * @param {WebSocket client} ws The client to add. They should already
   * be verified and have a 'id' property and a 'user_name' property.
   */
  addClient(ws, user_name) {
    console.log("Adding client: " + user_name, ws.id);
    this.clients[ws.id] = {
      client: ws,
      streams: [],
      user_name: user_name,
    };

    // Add user to the list of users if they don't exist
    if (!this.users[user_name]) {
      let stats = USER_STATS;
      stats.user_name = user_name;
      this.users[user_name] = stats;
    } else {
      this.users[user_name].times_connected++;
    }
    this.gs.ggs_db.setKey("data", "streams", "users", this.users);
  }

  /**
   * @brief Removes a client and their streams from the list of clients.
   * @param {string} client_id
   */
  removeClient(client_id) {
    console.log("Removing client: " + client_id);
    if (!this.doesClientExist(client_id)) {
      console.log("CLIENT DOES NOT EXIST - [removeClient]");
      return;
    }
    delete this.clients[client_id];
  }

  /**
   * @brief Parse a stream request message from a client and take action.
   * @param {string} user_id The user id of the client.
   * @param {StreamRequest} message The stream request message.
   */
  handleMessage(ws, message) {
    if (!this.doesClientExist(ws.id)) {
      sendError(ws, "Client does not exist in the streams component.");
    }
    let body = message.bdy;
    let action = body.action;
    let stream_name = "";
    if (action !== "remove_all") {
      stream_name = body.stream;
    }

    switch (action) {
      case "add":
        this.#addStreamRequest(ws, stream_name);
        break;
      case "remove":
        this.#removeStreamRequest(ws, stream_name);
        break;
      case "remove_all":
        this.#removeAllStreamsRequest(ws);
        break;
      case "get":
        this.#getStreamRequest(ws, stream_name);
        break;
      default:
        sendError(ws, "Invalid stream request action.");
        break;
    }
  }

  #addStreamRequest(ws, stream_name) {
    if (!this.#doesStreamExist(stream_name)) {
      sendError(ws, "Stream does not exist.");
      return;
    }

    // No duplicates
    if (this.clients[ws.id].streams.includes(stream_name)) {
      return;
    }

    this.clients[ws.id].streams.push(stream_name);

    console.log("Adding stream: " + stream_name + " to client: " + ws.id);
  }

  /**
   * @brief Removes a stream from a client's stream list by name.
   */
  #removeStreamRequest(ws, stream_name) {
    this.clients[ws.id].streams = this.clients[ws.id].streams.filter(
      (stream) => stream !== stream_name
    );
  }

  /**
   * @brief Removes all streams from a client's stream list.
   */
  #removeAllStreamsRequest(ws) {
    console.log("Removing all streams");
    this.clients[ws.id].streams = [];
  }

  /**
   * @brief Sends a stream (one time) to a client on request.
   */
  #getStreamRequest(ws, stream_name) {
    console.log("Getting stream: " + stream_name);
    if (!this.#doesStreamExist(stream_name)) {
      sendError(ws, "Stream does not exist.");
      return;
    }
    let data = this.#getStreamData(stream_name);
    this.#sendStreamData(ws, stream_name, data);
  }

  /**
   * @brief Gets the data associated with a stream.
   */
  #getStreamData(stream_name) {
    switch (stream_name) {
      case "ggs_status":
        return { status: "online" };
      case "gwc_socket_stats":
        return this.stats;
      default:
        if (this.gs.gfs_connection.doesDataExist(stream_name)) {
          return this.gs.gfs_connection.getData(stream_name);
        }
        return {};
    }
  }

  /**
   * @brief Actually sends the stream data to the client in a StreamResponse
   * message.
   */
  #sendStreamData(ws, stream_name, data) {
    let msg = new StreamResponse("ggs", "gwc", stream_name, data);
    ws.send(JSON.stringify(msg));
  }

  /**
   * @brief Returns true if a stream name is valid/exists.
   */
  #doesStreamExist(stream_name) {
    const static_stream_list = ["ggs_status", "gwc_socket_stats"];
    if (static_stream_list.includes(stream_name)) {
      return true;
    } else if (this.gs.gfs_connection.doesDataExist(stream_name)) {
      return true;
    }
    return false;
  }

  #doesUserExist(user_name) {
    return this.users.hasOwnProperty(user_name);
  }

  doesClientExist(client_id) {
    return this.clients.hasOwnProperty(client_id);
  }

  /**
   * @brief Sends all streams to the clients.
   */
  #cycle() {
    let num_clients_buffer = 0;
    let num_streams_buffer = 0;
    let current_users_buffer = [];

    for (let client_id in this.clients) {
      num_clients_buffer++;
      let client = this.clients[client_id];
      let ws = client.client; // client websocket

      // for stats only
      if (!current_users_buffer.includes(client.user_name)) {
        current_users_buffer.push(client.user_name);
      }

      // Check if client is open (shouldn't get here?)
      if (ws.readyState !== WebSocket.OPEN) {
        console.log("Client not open - [cycle]");
        return;
      }

      // Send all streams to the client
      let streams = client.streams;
      for (let stream_name of streams) {
        num_streams_buffer++;
        let data = this.#getStreamData(stream_name);
        this.#sendStreamData(ws, stream_name, data);
      }
    }

    // Update stats
    this.stats.num_current_clients = num_clients_buffer;
    this.stats.num_current_streams = num_streams_buffer;
    this.stats.current_users = current_users_buffer;
    this.stats.num_current_users = current_users_buffer.length;
  }
}

module.exports = ClientDataStreams;
