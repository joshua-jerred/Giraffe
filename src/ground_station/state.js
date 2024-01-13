const GgsDataBase = require("./db/ggs_db.js");
const GfsConnection = require("./gfs_connection/gfs_connection.js");

// Update interval for the global state
const kGlobalStateUpdateInterval = 1000;

class GlobalState {
  constructor() {
    this.ggs_db = new GgsDataBase();
    this.gfs_connection = new GfsConnection(this);

    this.ggs_status = {
      // disconnected, connected
      influxdb: "disconnected",
      gfs: "disconnected",
      gdl: "disconnected",
      telemetry: "disconnected",
      aprsfi: "disconnected",
      total_http_requests: 0,
    };

    setInterval(this.cycle.bind(this), kGlobalStateUpdateInterval);
  }

  saveData() {
    this.ggs_db.setKey("data", "streams", "clients", this.all_clients);
  }

  getStatus() {
    return this.ggs_status;
  }

  getStreamData(stream) {
    // const gfs_data_streams = [
    //   "system_info",
    //   "data_log_stats",
    //   "modules_statuses",
    //   "stream_stats",
    //   "server_module_stats",
    //   "extension_module_stats",
    //   "environmental",
    //   "location_data",
    //   "calculated_data",
    // ];

    if (stream === "status") {
      return this.getStatus();
    }
    // } else if (gfs_data_streams.includes(stream)) {
    return this.gfs_connection.getData(stream);
    // }
    // return { error: "stream not found" };
  }

  getSettings(category) {
    return this.gfs_connection.getSettings(category);
  }

  get status() {
    return this.ggs_status;
  }

  // Called every kGlobalStateUpdateInterval milliseconds
  cycle() {
    this.gfs_connection.update();
    this.ggs_status.gfs = this.gfs_connection.status;
  }
}

module.exports = GlobalState;
