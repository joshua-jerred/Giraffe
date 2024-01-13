const net = require("net");
const gfsResources = require("../../../project/metadata/gfs_resources.json");
const { DataRequest, parse } = require("giraffe-protocol");
const GfsDataSync = require("./data_sync.js");
const GfsSettingSync = require("./setting_sync.js");

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
    this.connected = false;

    this.data_sync = new GfsDataSync(global_state);
    this.setting_sync = new GfsSettingSync(global_state);
  }

  getData(category) {
    return this.data_sync.getData(category);
  }

  doesDataExist(category) {
    return this.data_sync.doesDataExist(category);
  }

  getSettings(category) {
    return this.setting_sync.getData(category);
  }

  getMsSinceLastUpdate(category) {
    return this.data_sync.getMsSinceLastUpdate(category);
  }

  doSettingsExist(category) {
    return this.setting_sync.doesDataExist(category);
  }

  update() {
    this.data_sync.update();
    this.setting_sync.update();
    this.connected = this.data_sync.getConnectionStatus();
  }

  get status() {
    return this.connected ? "connected" : "disconnected";
  }
};
