const DataMeta = require("../../../project/metadata/gdl_resources.json");
const { RequestMessage, parse } = require("giraffe-protocol");
const net = require("net");
const { Point } = require("@influxdata/influxdb-client");

const TIMEOUT = 1000;

module.exports = class GdlConnection {
  constructor(global_state) {
    this.global_state = global_state;
    this.resources = {};
    this.connected = false;
    this.#updateClassSettings();
    let meta = DataMeta["data"];

    for (let category in meta) {
      this.resources[category] = {
        meta: {
          last_request: new Date(),
          timestamp: new Date(),
          requests: 0,
        },
        data: {},
      };
      for (let data_item in meta[category]) {
        this.resources[category].data[data_item] = "no data";
      }
    }
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
  async update() {
    this.#updateClassSettings();

    for (let category in this.resources) {
      if (
        this.resources[category].meta.last_request >
        new Date() - this.update_interval
      ) {
        continue;
      }
      this.#updateResource(category);
      this.resources[category].meta.last_request = new Date();
      this.resources[category].meta.requests++;

      if (
        this.connected &&
        this.resources["status"] !== undefined &&
        this.resources["status"].data["aprs_rx_queue_size"] > 0
      ) {
        this.#getAprsPositionalData();
      }

      // sleep for 50ms to prevent flooding the socket
      // await new Promise((resolve) => setTimeout(resolve, 50));
    }
  }

  getMsSinceLastUpdate(category) {
    return Math.floor(new Date() - this.resources[category].meta.timestamp);
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

  /**
   * @brief Used internally to set the local data of a given category.
   * Called by #updateResource.
   * @param {string} category The category to set the data for.
   * @param {Message} message The giraffe protocol message to get the data from.
   */
  #setLocalResource(category, message) {
    let data_section = message.bdy.dat;
    for (let data_item in data_section) {
      try {
        this.resources[category].data[data_item] = data_section[data_item];

        // collecting timestamp data
        try {
          if (this.global_state.influx_enabled) {
            const point = new Point("ggs_resource_update_delay")
              .tag("api", "gdl")
              .tag("category", category)
              .intField(
                "ms_since_last_update",
                this.getMsSinceLastUpdate(category)
              );
            this.global_state.influx_writer.write(point);
          }
        } catch (error) {
          console.log(error);
        }
        // end collecting timestamp data

        this.resources[category].meta.timestamp = new Date();
      } catch (e) {
        console.log(
          "Failed to set local resource for: " +
            data_item +
            " in " +
            data_section
        );
      }
    }
  }

  #getAprsPositionalData() {
    let request = new RequestMessage("ggs", "gdl", "aprs_location");
    let con = new net.Socket();
    let that = this;
    con.setTimeout(TIMEOUT);

    con.connect(this.port, this.address, function () {
      con.write(JSON.stringify(request));
    });

    con.on("data", function (data) {
      that.connected = true;
      try {
        let msg = parse(data.toString());
        if (msg.bdy.cde !== "ok") {
          console.log("Error: " + msg.bdy.dat);
          return;
        }
        if (msg.bdy.dat.length === 0) {
          return;
        }

        msg.bdy.dat.forEach((item) => {
          that.global_state.gdl_telemetry.addAprsPositionPacket(item);
        });
      } catch (e) {
        console.log("Error parsing data: " + e);
      }
      con.destroy();
    });

    con.on("close", function () {});

    con.on("error", function (err) {});
  }

  /**
   * Goes through the categories and requests the data from GFS.
   * @param {string} category The category to request the data for.
   */
  #updateResource(category) {
    let request = new RequestMessage("ggs", "gdl", category);
    let con = new net.Socket();
    let that = this;
    con.setTimeout(TIMEOUT);

    con.connect(this.port, this.address, function () {
      con.write(JSON.stringify(request));
    });

    con.on("data", function (data) {
      that.connected = true;
      try {
        let msg = parse(data.toString());
        if (msg.bdy.cde !== "ok") {
          console.log("Error: " + msg.bdy.dat);
          return;
        }
        that.#setLocalResource(category, msg);
      } catch (e) {
        console.log("Error parsing data for category:" + category);
      }
      con.destroy();
    });

    con.on("close", function () {});

    con.on("error", function (err) {
      that.connected = false;
      /// @todo Need to figure out why we are getting here so often. Most likely on the C++ side.
      // console.log(
      // "Socket Error during the category: " + category + " - " + err
      // );
    });
  }

  get status() {
    return this.connected ? "connected" : "disconnected";
  }
};