module.exports = class Telemetry {
  constructor(global_state) {
    this.global_state = global_state;
    this.gdl_connected = false;
  }

  async update() {
    this.gdl_connected = true;
  }

  get status() {
    return this.gdl_connected ? "connected" : "disconnected";
  }
};
