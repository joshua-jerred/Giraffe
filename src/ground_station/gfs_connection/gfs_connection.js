const net = require("net");
const gfsResources = require("../../../project/metadata/gfs_resources.json");
const { DataRequest, parse, SetMessage } = require("giraffe-protocol");
const GfsDataSync = require("./data_sync.js");
const GfsSettingSync = require("./setting_sync.js");

module.exports = class GfsConnection {
  constructor(global_state) {
    this.settings_resources = gfsResources.settings;
    this.data_resources = gfsResources.data;
    this.global_state = global_state;

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

    for (let key in this.data_resources) {
      this.data_resources[key].meta = {
        last_request: new Date(),
        requests: 0,
      };
      this.data_resources[key].data = {};
    }

    for (let key in this.settings_resources) {
      this.settings_resources[key].meta = {
        last_request: new Date(),
        requests: 0,
      };
      this.settings_resources[key].data = {};
    }

    this.global_state = global_state;
    this.connected = false;

    this.data_sync = new GfsDataSync(global_state);
    this.setting_sync = new GfsSettingSync(global_state);

    this.recent_location_data = {};
  }

  getData(category) {
    return this.data_sync.getData(category);
  }

  doesDataExist(category) {
    return this.data_sync.doesDataExist(category);
  }

  getSettings(category) {
    return this.setting_sync.getData(category);
  }

  getMsSinceLastUpdate(category) {
    let val = this.data_sync.getMsSinceLastUpdate(category);
    return val;
  }

  doSettingsExist(category) {
    return this.setting_sync.doesDataExist(category);
  }

  getRecentLocationData() {
    return this.recent_location_data;
  }

  updateSettings(category, data, res) {
    if (!this.connected) {
      res.status(500).json({ message: "Not connected to GFS" });
    }

    let cat_with_prefix = "setting/" + category;
    let request = new SetMessage("ggs", "gfs", cat_with_prefix, data);
    this.#sendRequest(request, (response) => {
      if (!response.bdy.cde || response.bdy.cde !== "ok") {
        res.status(500).json(response.getJson());
        return;
      }
      res.status(200).json(response.getJson());
    });
  }

  update() {
    this.data_sync.update();
    this.setting_sync.update();
    this.connected = this.data_sync.getConnectionStatus();

    if (this.doesDataExist("location_data")) {
      this.recent_location_data = this.getData("location_data");
      if (!this.recent_location_data.hasOwnProperty("last_valid_gps_frame")) {
        this.recent_location_data = null;
        return;
      }
      this.recent_location_data =
        this.recent_location_data.last_valid_gps_frame;
    }
  }

  get status() {
    return this.connected ? "connected" : "disconnected";
  }

  #sendRequest(msg, callback) {
    let con = new net.Socket();
    const TIMEOUT = 500;
    con.setTimeout(TIMEOUT);
    con.connect(this.port, this.address, function () {
      con.write(msg.getString());
    });
    con.on("data", function (data) {
      let response = null;
      try {
        response = parse(data.toString());
      } catch (err) {
        console.log(err);
        callback({ status: "error", error: "parse error" + err });
        con.destroy();
        return;
      }
      callback(response);
      con.destroy();
    });
    con.on("timeout", function () {
      callback({ status: "timeout" });
      con.destroy();
    });
    con.on("error", function (error) {
      callback({ status: "error", error: error });
      con.destroy();
    });
  }
};
