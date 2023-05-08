const net = require("net");
const gfsResources = require("../metadata/gfs_resources.json");
const { DataRequest, parse } = require("giraffe-protocol");

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
  }

  get settings_res() {
    return this.settings_resources;
  }

  getData(category) {
    let data = this.data_resources[category].data;
    return data;
  }

  getSettings(category) {
    if (!this.settings_resources[category]) {
      return null;
    }
    let data = this.settings_resources[category].data;
    return data;
  }

  update() {
    this.updateDataResources();
    this.updateSettingsResources();
    this.global_state.ggs_status.gfs = this.connected
      ? "connected"
      : "disconnected";
  }

  updateDataResources() {
    const now = new Date();
    const update_rate = 3000; // ms
    for (let key in this.data_resources) {
      if (now - this.data_resources[key].meta.last_request > update_rate) {
        this.data_resources[key].meta.last_request = now;
        this.requestData(this.data_resources[key]);
      }
    }
  }

  updateSettingsResources() {
    const now = new Date();
    const update_rate = 3000; // ms
    for (let key in this.settings_resources) {
      if (now - this.settings_resources[key].meta.last_request > update_rate) {
        this.settings_resources[key].meta.last_request = now;
        this.requestData(this.settings_resources[key]);
      }
    }
  }

  requestData(item) {
    item.meta.requests += 1;
    var that = this;

    let params = {
      resource: item.resource,
      category: item.category,
      subcategory: item.subcategory,
    };

    let request = new DataRequest("ggs", "gfs", params);

    let con = new net.Socket();
    con.connect(8321, "127.0.0.1", function () {
      this.connected = true;
      con.write(JSON.stringify(request));
    });

    con.on("data", function (data) {
      try {
        let msg = parse(data.toString());

        if (msg.cat === "error") {
          console.log(msg);
          return;
        }

        if (msg.id === request.id) {
          item.data = msg.body.data;
        }

        that.connected = true;
      } catch (e) {
        console.log("Error parsing data");

        that.connected = false;
      }
      con.destroy();
      
    });

    con.on("close", function () {});

    con.on("error", function (err) {
      console.log("Error connecting to GFS" + err);
      that.connected = false;
    });
  }

  get status() {
    return this.connected ? "connected" : "disconnected";
  }
};
