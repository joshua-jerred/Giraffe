const GgsDataBase = require("./db/ggs_db.js");
const GfsConnection = require("./gfs_connection/gfs_connection.js");
const GdlConnection = require("./gdl_connection/gdl_connection.js");
const GdlTelemetry = require("./gdl_connection/gdl_telemetry.js");
const FlightDataHandler = require("./http_api/flight_data_handler.js");

const InfluxWriter = require("./db/influx.js");
const { Point } = require("@influxdata/influxdb-client");

// Update interval for the global state
const kGlobalStateUpdateInterval = 500;

class GlobalState {
  constructor() {
    this.ggs_db = new GgsDataBase();
    this.influx_writer = new InfluxWriter(this);
    this.influx_enabled = this.ggs_db.get(
      "settings",
      "influx_db",
      "influx_enabled"
    );

    this.gfs_connection = new GfsConnection(this);
    this.gdl_connection = new GdlConnection(this);
    this.gdl_telemetry = new GdlTelemetry(this);
    this.flight_data_handler = new FlightDataHandler(this);

    this.ggs_status = {
      influxdb: "unknown",
      gfs: "unknown",
      gdl: "unknown",
      telemetry_uplink: "unknown",
      telemetry_downlink: "unknown",
      aprsfi: "unknown",
      total_http_requests: 0,
    };

    setInterval(this.cycle.bind(this), kGlobalStateUpdateInterval);
  }

  saveData() {
    this.ggs_db.setKey("data", "streams", "clients", this.all_clients);
  }

  getStatus() {
    return this.ggs_status;
  }

  getStreamData(stream) {
    if (stream === "status") {
      return this.getStatus();
    }
    return this.gfs_connection.getData(stream);
  }

  getSettings(category) {
    return this.gfs_connection.getSettings(category);
  }

  get status() {
    return this.ggs_status;
  }

  // Called every kGlobalStateUpdateInterval milliseconds
  cycle() {
    this.gfs_connection.update();
    this.gdl_connection.update();
    this.flight_data_handler.update();

    this.ggs_status.gfs = this.gfs_connection.status;
    this.ggs_status.gdl = this.gdl_connection.status;

    // update the telemetry status with GDL data
    if (this.ggs_status.gdl === "connected") {
      try {
        let gdl_status = this.gdl_connection.getData("status");
        this.ggs_status.telemetry_uplink = gdl_status.telemetry_uplink;
        this.ggs_status.telemetry_downlink = gdl_status.telemetry_downlink;
      } catch (error) {
        this.ggs_status.telemetry_uplink = "unknown";
        this.ggs_status.telemetry_downlink = "unknown";
      }
    }

    if (this.influx_enabled) {
      let point = new Point("ggs_status")
        .stringField("influxdb", this.ggs_status.influxdb)
        .stringField("gfs", this.ggs_status.gfs)
        .stringField("gdl", this.ggs_status.gdl)
        .stringField("telemetry_uplink", this.ggs_status.telemetry_uplink)
        .stringField("telemetry_downlink", this.ggs_status.telemetry_downlink)
        .stringField("aprsfi", this.ggs_status.aprsfi)
        .intField("total_http_requests", this.ggs_status.total_http_requests);

      this.influx_writer.write(point);
      this.influx_writer.flush();
    }
  }
}

module.exports = GlobalState;
