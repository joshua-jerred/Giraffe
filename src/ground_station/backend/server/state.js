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

    this.clients = {};
    this.client_names = {};
    this.all_client_names = this.ggs_db.get("data", "clients", "client_names");
  }

  saveData() {
    this.ggs_db.setKey("data", "clients", "client_names", this.all_client_names);
  }

  getStatus() {
    return this.ggs_status;
  }
}

module.exports = GlobalState;
