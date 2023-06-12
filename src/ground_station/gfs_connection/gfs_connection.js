const net = require("net");
const gfsResources = require("../../common/metadata/gfs_resources.json");
const { DataRequest, parse } = require("giraffe-protocol");
const GfsDataSync = require("./data_sync.js");

module.exports = class GfsConnection {
  constructor(global_state) {
    this.settings_resources = gfsResources.settings;
    this.data_resources = gfsResources.data;

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
    this.address = "localhost";
    this.port = 8321;
    this.connected = false;

    this.data_sync = new GfsDataSync(global_state);
  }

  get settings_res() {
    return this.settings_resources;
  }

  getData(category) {
    return this.data_sync.getData(category);
  }

  getSettings(category) {
    return null;
  }

  update() {
    this.data_sync.update();
    this.connected = this.data_sync.getConnectionStatus();
  }

  get status() {
    return this.connected ? "connected" : "disconnected";
  }
};
