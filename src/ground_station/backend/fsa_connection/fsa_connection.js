const net = require("net");
const { RequestMessage, SetMessage, parse } = require("giraffe-protocol");
const loadMetaData = require("../metadata/metaLoader");

const FSA_PORT = 20392;
const FSA_TIMEOUT = 2000; // ms
const FSA_STATUS_UPDATE_RATE = 1000; // ms
const FSA_SETTINGS_UPDATE_RATE = 5000; // ms

const MESSAGE_DELAY = 200; // time to wait before sending the next message
class SocketExchangeQueue {
  constructor(global_state) {
    this.global_state = global_state;
    this.queue = [];
    this.sending = false;

    this.address = null;
    this.#updateClassSettings();

    this.error_count = 0;
    this.timeout_count = 0;
    this.parse_error_count = 0;
    this.success_count = 0;

    setInterval(this.#sendNext.bind(this), MESSAGE_DELAY);
  }

  addMessage(message, callback = (error, response) => {}) {
    this.queue.push({ message, callback });

    if (!this.sending) {
      this.#sendNext();
    }
  }

  getStats() {
    return {
      queue_length: this.queue.length,
      error_count: this.error_count,
      timeout_count: this.timeout_count,
      parse_error_count: this.parse_error_count,
      success_count: this.success_count,
    };
  }

  #updateClassSettings() {
    this.address = this.global_state.ggs_db.get(
      "settings",
      "gfs_connection",
      "gfs_socket_address"
    );
  }

  #sendMessage(message, callback) {
    // console.log("Sending message to FSA: " + JSON.stringify(message));
    let socket = new net.Socket();
    socket.setTimeout(FSA_TIMEOUT);
    socket.connect(FSA_PORT, this.address, () => {
      socket.write(JSON.stringify(message));
    });
    socket.on("data", (data) => {
      // console.log("FSA response: " + data);
      // this.#kickConnectionTimeout();
      try {
        let response_message = parse(data.toString());
        callback(null, response_message);
        this.success_count++;
      } catch (e) {
        callback("Failed to parse FSA response: " + e, null);
        this.parse_error_count++;
        return;
      }
    });
    socket.on("timeout", () => {
      callback("FSA connection timed out for resource", null);
      socket.destroy();
      this.timeout_count++;
    });
    socket.on("error", (error) => {
      callback("FSA connection error: " + error, null);
      socket.destroy();
      this.error_count++;
    });
    socket.on("close", () => {
      // console.log("FSA connection closed.");
    });
  }

  #sendNext() {
    if (this.queue.length === 0) {
      this.sending = false;
      return;
    }
    // console.log("Sending next message.");
    let { message, callback } = this.queue.shift();
    this.sending = true;
    this.#sendMessage(message, callback);
  }
}

module.exports = class FsaConnection {
  constructor(global_state) {
    this.socket_exchange = new SocketExchangeQueue(global_state);
    this.global_state = global_state;
    this.connected_to_fsa = false;
    this.connection_timeout = null;

    this.status_data = {
      agent_uptime_s: 0,
      flight_software_status: "n/d",
    };

    this.have_settings = false;
    this.settings_data = {};

    this.status_update_rate = null;
    this.status_update_interval = null;
    this.settings_update_rate = null;
    this.settings_update_interval = null;
    this.#setUpdateRate(FSA_STATUS_UPDATE_RATE, FSA_SETTINGS_UPDATE_RATE);

    this.#sendRequestMessage("status");
    this.#sendRequestMessage("settings");
  }

  #kickConnectionTimeout() {
    this.connected_to_fsa = true;
    clearTimeout(this.connection_timeout);
    this.connection_timeout = setTimeout(() => {
      this.connected_to_fsa = false;
      this.have_settings = false;
    }, FSA_TIMEOUT);
  }

  #setUpdateRate(status_rate, settings_rate) {
    clearInterval(this.status_update_interval);
    clearInterval(this.settings_update_interval);
    this.status_update_rate = status_rate;
    this.settings_update_rate = settings_rate;

    this.status_update_interval = setInterval(
      this.#sendRequestMessage.bind(this, "status"),
      this.status_update_rate
    );
    this.settings_update_interval = setInterval(
      this.#sendRequestMessage.bind(this, "settings"),
      this.settings_update_rate
    );
  }

  #sendRequestMessage(resource) {
    // console.log("Requesting resource: " + resource);
    let request_message = new RequestMessage("ggs", "fsa", resource);

    this.socket_exchange.addMessage(
      request_message,
      (error, response_message) => {
        if (error) {
          // console.log("FSA request error: " + error);
          return;
        }
        this.#kickConnectionTimeout();
        this.#handleFsaRequestResponse(resource, response_message);
      }
    );
  }

  #handleFsaRequestResponse(resource, response_message) {
    const body = response_message.bdy;

    if (!body.hasOwnProperty("cde")) {
      console.log("FSA response missing code.");
      return;
    }
    if (!body.hasOwnProperty("dat")) {
      console.log("FSA response missing data.");
      return;
    }
    const response_code = body.cde;

    if (response_code !== "ok") {
      console.log("FSA response error: " + response_code);
      return;
    }
    const response_data = body.dat;

    if (resource === "status") {
      this.status_data = response_data;
    } else if (resource === "settings") {
      this.have_settings = true;
      this.settings_data = response_data;
    }
  }

  #sendSetMessage(resource, data, callback = (error, response_message) => {}) {
    console.log("Setting resource: " + resource);
    let set_message = new SetMessage("ggs", "fsa", resource, data);

    this.socket_exchange.addMessage(set_message, (error, response_message) => {
      if (error) {
        callback("FSA set error for resource: " + resource, response_message);
        return;
      }
      this.#kickConnectionTimeout();
      callback(null, response_message);
    });
  }

  getDataForResource(
    resource,
    response,
    values_wanted = true,
    metadata_wanted = true
  ) {
    let metadata = loadMetaData("fsa", resource);
    let values = {};

    if (resource === "status") {
      values = this.status_data;
      values["connected_to_fsa"] = this.connected_to_fsa;
      values["gdl_fda_link_stats"] = this.socket_exchange.getStats();
    } else if (resource === "settings") {
      values = this.settings_data;
    } else {
      response.status(404);
      return;
    }

    let response_body = {};
    if (values_wanted) {
      response_body["values"] = values;
    }

    if (metadata_wanted) {
      response_body["metadata"] = metadata;
    }

    response.send(response_body);
  }

  updateConfig(new_config_data, response) {
    this.#sendSetMessage(
      "settings",
      new_config_data,
      (error, response_message) => {
        if (error) {
          response
            .status(500)
            .send({ error: error, response: response_message });
          return;
        }

        if (response_message.bdy.cde === "ok") {
          response.send(response_message);
        } else {
          response.status(500).send({
            error: "Failed to update FSA settings.",
            response: response_message,
          });
        }
      }
    );
  }

  /// @brief Get the current status of the agent
  getFsaConnectionStatus() {
    return this.connected_to_fsa ? "connected" : "disconnected";
  }

  getFsaGfsStatus() {
    return this.status_data.flight_software_status;
  }
};
