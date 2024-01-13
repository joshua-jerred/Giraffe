const GgsDataBase = require("./db/ggs_db.js");
const GfsConnection = require("./gfs_connection/gfs_connection.js");
const GdlConnection = require("./gdl_connection/gdl_connection.js");

// Update interval for the global state
const kGlobalStateUpdateInterval = 1000;

class GlobalState {
  constructor() {
    this.ggs_db = new GgsDataBase();
    this.gfs_connection = new GfsConnection(this);
    this.gdl_connection = new GdlConnection(this);

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
    if (stream === "status") {
      return this.getStatus();
    }
    return this.gfs_connection.getData(stream);
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
    this.gdl_connection.update();

    this.ggs_status.gfs = this.gfs_connection.status;
    this.ggs_status.gdl = this.gdl_connection.status;
  }
}

module.exports = GlobalState;
