const create = require("giraffe-protocol/message").create;

class Status {
  constructor() {
    this.num_clients = 0;
    this.telemetry_connection = "disconnected";
    this.gfs_connection = "disconnected";
  }
  getStatusMessage() {
    let body = {
      num_clients: this.num_clients,
      telemetry_connection: this.telemetry_connection,
      gfs_connection: this.gfs_connection,
    };
    return JSON.stringify(create("ggs", "status", body));
  }
}

module.exports = Status;
