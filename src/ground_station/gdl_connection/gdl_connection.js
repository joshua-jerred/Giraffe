const DataMeta = require("../../../project/metadata/gdl_resources.json");
const { RequestMessage, SetMessage, parse } = require("giraffe-protocol");
const net = require("net");
const { Point } = require("@influxdata/influxdb-client");

const TIMEOUT = 1000;

module.exports = class GdlConnection {
  constructor(global_state) {
    this.global_state = global_state;
    this.connected = false;
    this.socket = null;
    this.socket_interval = null; // setInterval() object

    this.update_interval = 1000;
    this.#updateClassSettings();

    this.status_data = {};
    this.status_last_updated = new Date();

    this.new_broadcast_message = null;

    this.config_data = {};
    this.config_up_to_date = false;
    this.new_config = null;

    this.telemetry_data = {
      position_packets: [],
      received_messages: [],
      sent_messages: [],
    };

    setInterval(this.#cycle.bind(this), this.update_interval);
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

    if (!this.socket || !this.connected) {
      let self = this;
      this.socket = new net.Socket();
      this.socket.setTimeout(TIMEOUT);
      this.socket.connect(this.port, this.address, function () {
        self.connected = true;
        // send the first request
        let request = new RequestMessage("ggs", "gdl", "status");
        self.socket.write(JSON.stringify(request));
      });
      this.socket.on("data", function (data) {
        self.connected = true;
        // console.log("GDL Connection Data");

        let next_request_rsc = "status";
        try {
          let message = parse(data.toString());
          next_request_rsc = self.#handleSocketResponse(message);
        } catch {}

        if (self.socket_interval) {
          clearInterval(self.socket_interval);
        }
        self.socket_interval = setInterval(() => {
          // send new requests here!
          let new_request = null;
          if (self.new_broadcast_message) {
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
      });
      this.socket.on("error", function (err) {
        // console.log("GDL Connection Error: " + err);
        self.connected = false;
        clearInterval(self.socket_interval);
        self.socket_interval = null;
        self.new_broadcast_message = null;
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
        if (obj.type !== "LOCATION") {
          this.global_state.database.addReceivedMessage(
            obj.type,
            obj.data,
            obj.identifier
          );
        } else {
          let loc = obj.location;
          this.global_state.database.addReceivedLocation(
            loc.latitude,
            loc.longitude,
            loc.altitude,
            loc.speed,
            loc.heading,
            loc.time_code
          );
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
          console.log("location packet sent, not implemented yet");
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

  /**
   * @brief Reads settings relevant to this class from the database and
   * stores them in the class.
   */
  #updateClassSettings() {
    this.update_interval = this.global_state.ggs_db.get(
      "settings",
      "gdl",
      "data_update_interval"
    );

    this.address = this.global_state.ggs_db.get(
      "settings",
      "gdl",
      "socket_address"
    );

    this.port = this.global_state.ggs_db.get("settings", "gdl", "socket_port");
  }

  get status() {
    return this.connected ? "connected" : "disconnected";
  }
};
