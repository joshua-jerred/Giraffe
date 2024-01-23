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

  /**
   * @brief This function returns the data for a given category.
   * @param {string} category The category to get the data for.
   * @returns {object} The data for the given category, or null if the
   * category does not exist.
   */
  getData(category) {
    if (!this.resources[category]) {
      return null;
    }
    let data_and_meta = this.resources[category].data;
    // data_and_meta.meta.MS_SINCE_LAST_UPDATE =
    // this.getMsSinceLastUpdate(category);
    return data_and_meta;
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
      console.log("new socket");
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
        console.log("GDL Connection Data");

        let message = parse(data.toString());
        if (!message.bdy.rsc || !message.bdy.dat) {
          throw new Error("Invalid response received: " + message);
        }

        self.#handleSocketResponse(message);

        if (message.bdy.rsc === "status") {
          console.log("got status data");
          self.status_data = message.bdy.data;
        }

        if (self.socket_interval) {
          clearInterval(self.socket_interval);
        }
        self.socket_interval = setInterval(() => {
          // send new requests here!
          let new_request = new RequestMessage("ggs", "gdl", "status");
          self.socket.write(JSON.stringify(new_request));
        }, self.update_interval);
      });
      this.socket.on("close", function () {
        self.connected = false;
        clearInterval(self.socket_interval);
        self.socket_interval = null;
      });
      this.socket.on("error", function (err) {
        console.log("GDL Connection Error: " + err);
        self.connected = false;
        clearInterval(self.socket_interval);
        self.socket_interval = null;
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
    let received_resource = data.bdy.rsc;
    let received_data = data.bdy.dat;

    if (received_resource === "status") {
      this.status_data = received_data;
      this.status_last_updated = new Date();
    }

    return "status";
  }

  getMsSinceLastUpdate(category) {
    return Math.floor(new Date() - this.resources[category].meta.timestamp);
  }

  sendBroadcast(data) {
    let body = {
      data: data,
    };
    let request = new SetMessage("ggs", "gdl", "new_broadcast", body);
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
