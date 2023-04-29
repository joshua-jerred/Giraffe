const GgsDataBase = require("./db/ggs_db.js");

class GlobalState {
  constructor() {
    this.ggs_db = new GgsDataBase();

    this.statuses = {
      // disconnected, connected
      ggs: "connected",
      influx_db: "disconnected",
      gfs: "disconnected",
      telemetry: "disconnected",
    };

    this.num_ws_clients = 0;
    this.total_http_requests = 0;
    this.total_ws_messages = 0;
  }
  getStatuses() {
    return this.statuses;
  }
  getStatusMessage() {
    return JSON.stringify({
      status: this.statuses,
      data: {
        num_ws_clients: this.num_ws_clients,
        total_http_requests: this.total_http_requests,
        total_ws_messages: this.total_ws_messages,
      },
    });
  }
}

module.exports = GlobalState;
