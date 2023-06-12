/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   data_sync.js
 * @brief  This file contains an interface to sync data between the GGS and GFS.
 *
 * =*=======================*=
 * @author     Joshua Jerred https://joshuajer.red
 * @date       2023-06-10
 * @copyright  2023 (license to be defined)
 */

const DataMeta = require("../../common/metadata/gfs_resources.json");
const { RequestMessage, parse } = require("giraffe-protocol");
const net = require("net");

module.exports = class GfsDataSync {
  constructor(global_state) {
    this.global_state = global_state;
    this.resources = {};
    this.connected = false;
    this.updateClassSettings();

    for (let category in DataMeta) {
      this.resources[category] = {
        meta: {
          last_request: new Date(),
          requests: 0,
        },
        data: {},
      };
      for (let data_item in DataMeta[category]) {
        this.resources[category].data[data_item] = "no data";
      }
    }
  }

  getConnectionStatus() {
    return this.connected;
  }

  updateClassSettings() {
    this.update_interval = this.global_state.ggs_db.get(
      "settings",
      "gfs_state_intervals",
      "data"
    );

    this.address = this.global_state.ggs_db.get(
      "settings",
      "gfs_connection",
      "gfs_socket_address"
    );

    this.port = this.global_state.ggs_db.get(
      "settings",
      "gfs_connection",
      "gfs_socket_port"
    );
  }

  getData(category) {
    if (!this.resources[category]) {
      return null;
    }
    return this.resources[category].data;
  }

  setLocalResource(category, message) {
    let data_section = message.body.dat;
    for (let data_item in data_section) {
      try {
        this.resources[category].data[data_item] = data_section[data_item];
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

  updateResource(category) {
    let request = new RequestMessage("ggs", "gfs", "data/" + category);
    let con = new net.Socket();
    let that = this;

    con.connect(this.port, this.address, function () {
      con.write(JSON.stringify(request));
    });

    con.on("data", function (data) {
      that.connected = true;
      try {
        let msg = parse(data.toString());
        if (msg.body.cde !== "ok") {
          console.log("Error: " + msg.body.dat);
          return;
        }
        that.setLocalResource(category, msg);
      } catch (e) {
        console.log("Error parsing data " + e);
      }
      con.destroy();
    });

    con.on("close", function () {});

    con.on("error", function (err) {
      that.connected = false;
    });
  }

  update() {
    this.updateClassSettings();

    for (let category in this.resources) {
      if (
        this.resources[category].meta.last_request >
        new Date() - this.update_interval
      ) {
        continue;
      }
      this.updateResource(category);
      this.resources[category].meta.last_request = new Date();
      this.resources[category].meta.requests++;
    }
  }
};
