const GgsDataBase = require("./db/ggs_db.js");
const GfsConnection = require("./gfs_connection/gfs_connection.js");

class GlobalState {
  constructor() {
    this.ggs_db = new GgsDataBase();
    this.gfs_connection = new GfsConnection(this);

    this.ggs_status = {
      // disconnected, connected
      influx_db: "disconnected",
      gfs: "disconnected",
      telemetry: "disconnected",
      num_ws_clients: 0,
      current_clients: [],
      total_http_requests: 0,
      total_ws_messages: 0,
    };

    this.clients = {};
    this.current_client_names = {};
    this.all_client_names = this.ggs_db.get("data", "clients", "client_names");

    setInterval(this.cycle.bind(this), 1000);
  }

  saveData() {
    this.ggs_db.setKey(
      "data",
      "clients",
      "client_names",
      this.all_client_names
    );
  }

  getStatus() {
    return this.ggs_status;
  }

  getStreamData(stream) {
    if (stream === "status") {
      return this.getStatus();
    } else if (stream === "critical") {
      return this.gfs_connection.getData("critical");
    } else if (stream === "position_gps") {
      return this.gfs_connection.getData("position_gps");
    } else if (stream === "position_imu") {
      return this.gfs_connection.getData("position_imu");
    } else if (stream === "gfs_status") {
      return this.gfs_connection.getData("gfs_status");
    }

  }

  get status() {
    return this.ggs_status;
  }

  cycle() {
    this.gfs_connection.update();
    this.ggs_status.gfs = this.gfs_connection.status;
  }
}

module.exports = GlobalState;
