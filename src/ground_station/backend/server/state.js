const GgsDataBase = require("./db/ggs_db.js");

class GlobalState {
  constructor() {
    this.ggs_db = new GgsDataBase();

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
  }
  getStatus() {
    return this.ggs_status;
  }
}

module.exports = GlobalState;
