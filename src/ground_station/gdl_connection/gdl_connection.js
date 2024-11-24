const DataMeta = require("../../../project/metadata/gdl_resources.json");
const { RequestMessage, SetMessage, parse } = require("giraffe-protocol");
const net = require("net");
const { Point } = require("@influxdata/influxdb-client");

const SocketExchangeQueue = require("../socket_exchange_queue");

const TIMEOUT = 3000;

module.exports = class GdlConnection {
  constructor(global_state) {
    this.global_state = global_state;
    this.connected = false;
    this.gdl_enabled = false;
    this.socket = null;
    this.socket_interval = null; // setInterval() object

    this.update_interval = 1000;
    this.#updateClassSettings();

    this.status_data = {};
    this.previous_last_message_received = "";
    this.status_last_updated = new Date();

    this.new_broadcast_message = null;
    this.new_exchange_message = null;

    this.config_data = {};
    this.config_up_to_date = false;
    this.new_config = null;

    this.telemetry_data = {
      position_packets: [],
      received_messages: [],
      sent_messages: [],
    };

    this.update_interval_id = setInterval(
      this.#cycle.bind(this),
      this.update_interval
    );
  }

  getConnectionStatus() {
    return this.connected;
  }

  getStatus() {
    return {
      data: this.status_data,
      meta: {
        MS_SINCE_LAST_UPDATE: new Date() - this.status_last_updated,
      },
    };
  }

  /**
   * @brief This function returns true if the given category exists.
   * @param {string} category The category to check.
   * @returns boolean True if the category exists, false otherwise.
   */
  doesDataExist(category) {
    return this.resources[category] ? true : false;
  }

  /**
   * @brief Called at a set interval at a higher level. This runs everything.
   */
  #cycle() {
    this.#updateClassSettings();
    if (!this.enabled) {
      this.connected = false;
      this.return;
    }

    if (!this.socket || !this.connected) {
      let self = this;
      this.socket = new net.Socket();
      this.socket.setTimeout(TIMEOUT);
      this.socket.connect(this.port, this.address, function () {
        self.connected = true;
        // send the first request
        let request = new RequestMessage("ggs", "gdl", "status");
        self.socket.write(JSON.stringify(request));
        // console.log("GDL Connection Cycle 1");
      });
      this.socket.on("data", function (data) {
        self.connected = true;

        let next_request_rsc = "status";
        try {
          let message = parse(data.toString());
          next_request_rsc = self.#handleSocketResponse(message);
          console.log("GDL Connection Cycle: " + message);
        } catch {}

        if (self.socket_interval) {
          clearInterval(self.socket_interval);
        }
        self.socket_interval = setInterval(() => {
          // send new requests here!
          let new_request = null;
          if (self.new_exchange_message) {
            // if a exchange is pending
            let body = {
              data: self.new_exchange_message,
            };
            new_request = new SetMessage("ggs", "gdl", "new_exchange", body);
            self.new_exchange_message = null;
          } else if (self.new_broadcast_message) {
            // if a broadcast is pending
            let body = {
              data: self.new_broadcast_message,
            };
            new_request = new SetMessage("ggs", "gdl", "new_broadcast", body);
            self.new_broadcast_message = null;
          } else if (self.new_config) {
            // if a config change is pending
            new_request = new SetMessage(
              "ggs",
              "gdl",
              "config",
              self.new_config
            );
            self.new_config = null;
            self.config_up_to_date = false;
          } else {
            new_request = new RequestMessage("ggs", "gdl", next_request_rsc);
          }
          self.socket.write(JSON.stringify(new_request));
        }, self.update_interval);
      });
      this.socket.on("close", function () {
        self.connected = false;
        clearInterval(self.socket_interval);
        self.socket_interval = null;
        self.new_broadcast_message = null;
        console.log("GDL Connection Closed");
      });
      this.socket.on("timeout", function () {
        self.connected = false;
        clearInterval(self.socket_interval);
        self.socket_interval = null;
        self.new_broadcast_message = null;
        console.log("GDL Connection Timeout");
      });
      this.socket.on("error", function (err) {
        console.log("GDL Connection Error: " + err);
        self.connected = false;
        clearInterval(self.socket_interval);
        self.socket_interval = null;
        self.new_broadcast_message = null;
        console.log("GDL Connection Error");
      });
    }

    return;
  }

  /**
   * Receives the data, handles it, and returns what should be requested next.
   * @param {*} data
   * @returns {string} The resource to request next.
   */
  #handleSocketResponse(data) {
    let received_data = data.bdy.dat;
    if (!data.bdy.rsc) {
      return "status";
    }

    let received_resource = data.bdy.rsc;

    if (received_resource === "status") {
      this.status_data = received_data;
      this.status_last_updated = new Date();

      if (
        this.status_data.last_message_received !==
          this.previous_last_message_received &&
        this.status_data.last_message_received.length > 5 // arbitrary length, n/d
      ) {
        this.previous_last_message_received =
          this.status_data.last_message_received;
        try {
          let date_time = new Date(this.status_data.last_message_received);
          let formatted = `${date_time.getHours()}:${date_time.getMinutes()}:${date_time.getSeconds()} UTC`;
          this.global_state.flight_data.updateTimeDataFromGdlTelemetry(
            formatted
          );
        } catch (e) {
          console.log("Error parsing last message received: ", e);
        }
      }

      // console.log(this.status_data);

      // update the local config first
      if (!this.config_up_to_date) {
        return "config";
      }

      // get received messages if there are any
      if (this.status_data.svl_received_queue_size > 0) {
        return "received_messages";
      }

      // get sent messages if there are any
      if (this.status_data.svl_sent_messages_queue > 0) {
        return "sent_messages";
      }

      // lastly, update the logs
      if (this.status_data.svl_log_queue_size > 0) {
        return "log";
      }
    } else if (received_resource === "config") {
      this.config_data = received_data;
      this.config_up_to_date = true;
    } else if (received_resource === "log") {
      for (let obj of received_data) {
        let timestamp = new Date(obj.time + "Z");
        let unix_time = Math.floor(timestamp.getTime() / 1000);
        this.global_state.database.addGdlServerLogItem({
          level: obj.level.toLowerCase(),
          message: obj.msg,
          timestamp: unix_time,
        });
      }
    } else if (received_resource === "received_messages") {
      for (let obj of received_data) {
        // console.log("packet type: " + obj.type);
        // console.log("packet identifier: " + obj.identifier);
        // console.log(obj.data);
        if (obj.type === "BROADCAST" || obj.type === "EXCHANGE") {
          this.global_state.database.addReceivedMessage(
            obj.type,
            obj.data,
            obj.identifier,
            obj.timestamp
          );
        } else if (obj.type === "TELEMETRY_DATA_REPORT") {
          let data = obj.data;
          this.global_state.database.addReceivedTelemetryDataReport(
            data,
            obj.timestamp
          );
        } else if (obj.type === "LOCATION") {
          let loc = obj.data;
          this.global_state.database.addReceivedLocation(
            loc.latitude,
            loc.longitude,
            loc.altitude,
            loc.speed,
            loc.heading,
            loc.time_code
          );
        } else {
          console.log("Unknown packet type: " + obj.type);
        }
      }
    } else if (received_resource === "sent_messages") {
      for (let obj of received_data) {
        if (obj.type !== "LOCATION") {
          this.global_state.database.addSentMessage(
            obj.type,
            obj.data,
            obj.identifier
          );
        } else {
          console.log("packet sent, not implemented yet");
        }
      }
    } else {
      console.log("Received unknown resource: " + received_resource);
    }
    return "status";
  }

  sendBroadcast(data) {
    if (!this.connected) {
      return false;
    }
    if (this.new_broadcast_message) {
      return false;
    }
    this.new_broadcast_message = data;
    return true;
  }

  sendExchange(data) {
    if (!this.connected) {
      return false;
    }
    if (this.new_exchange_message) {
      return false;
    }
    this.new_exchange_message = data;
    return true;
  }

  setConfig(new_config) {
    if (!this.connected) {
      return "not_connected";
    }

    if (this.new_config) {
      return "config change already pending";
    }

    const required_keys = [
      "logging_level",
      "logging_print_to_stdout",
      "proactive_keep_alive",
      "remote_callsign",
      "remote_ssid",
      "server_port",
      "source_callsign",
      "source_ssid",
    ];

    for (let key of required_keys) {
      if (new_config[key] === undefined) {
        return "Missing required key: " + key;
      }
    }

    this.new_config = new_config;

    return "success";
  }

  getConfig() {
    return {
      values: this.config_data,
      metadata: {
        logging_level: {
          name: "Logging Level",
          description: "The level of logging to use.",
          type: "enum",
          options: ["INFO", "WARN"],
        },
        logging_print_to_stdout: {
          name: "Print to Stdout",
          description: "Whether to print logs to stdout.",
          type: "bool",
        },
        proactive_keep_alive: {
          name: "Proactive Keep Alive",
          description: "Whether to send keep alive messages proactively.",
          type: "bool",
        },
        remote_callsign: {
          name: "Remote Callsign",
          description: "The callsign of the remote station.",
          type: "string",
        },
        remote_ssid: {
          name: "Remote SSID",
          description: "The SSID of the remote station.",
          type: "int",
        },
        source_callsign: {
          name: "Source Callsign",
          description: "The callsign of the source station.",
          type: "string",
        },
        source_ssid: {
          name: "Source SSID",
          description: "The SSID of the source station.",
          type: "int",
        },
      },
    };
  }

  /**
   * @brief Reads settings relevant to this class from the database and
   * stores them in the class.
   */
  #updateClassSettings() {
    const config = this.global_state.ggs_db.get("settings", "gdl");
    this.gdl_enabled = config.gdl_enabled;
    this.update_interval = config.data_update_interval;
    this.address = config.socket_address;
    this.port = config.socket_port;
  }

  get status() {
    // edge cases here, not too worried.
    if (!this.gdl_enabled) {
      return "disabled";
    }

    if (this.connected) {
      return "connected";
    }

    return "disconnected";
  }
};
