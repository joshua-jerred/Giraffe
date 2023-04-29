class GlobalState {
  constructor() {
    this.statuses = { // disconnected, connected
      ggs: "connected",
      influx_db: "disconnected", 
      gfs: "disconnected",
      telemetry: "disconnected",
    }

    this.num_ws_clients = 0;
    this.total_http_requests = 0;
  }
  getStatuses() {
    return this.statuses;
  }
}

module.exports = GlobalState;
