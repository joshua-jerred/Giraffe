const GgsDataBase = require("./db/ggs_db.js");
const GfsConnection = require("./gfs_connection/gfs_connection.js");
const GdlConnection = require("./gdl_connection/gdl_connection.js");
const FlightDataHandler = require("./http_api/flight_data_handler.js");

const Database = require("./db/database.js");
const InfluxWriter = require("./db/influx.js");
const { Point } = require("@influxdata/influxdb-client");

const AprsFi = require("./aprs_fi/aprs_fi.js");

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

    this.database = new Database();

    this.gfs_connection = new GfsConnection(this);
    this.gdl_connection = new GdlConnection(this);
    this.flight_data_handler = new FlightDataHandler(this);

    this.aprs_fi = new AprsFi(this);

    this.logging_level = this.ggs_db.get(
      "settings",
      "ggs_settings",
      "logging_level"
    );

    this.ggs_status = {
      status: "ok",
      influxdb: "unknown",
      gfs: "unknown",
      gdl: "unknown",
      telemetry_uplink: "unknown",
      telemetry_downlink: "unknown",
      aprsfi: "unknown",
      total_http_requests: 0,
      general: this.flight_data_handler.general.values,
      location: this.flight_data_handler.location_data.values,
    };

    setInterval(this.cycle.bind(this), kGlobalStateUpdateInterval);
    this.info("Started Ground Station");
  }

  // log an error message to the database
  error(message) {
    if (this.logging_level === "error") {
      this.database.addLog("error", message);
    }
  }

  // log a warning message to the database
  warning(message) {
    if (this.logging_level === "error" || this.logging_level === "warning") {
      this.database.addLog("warning", message);
    }
  }

  // log an info message to the database
  info(message) {
    if (this.logging_level !== "error" && this.logging_level !== "warning") {
      this.database.addLog("info", message);
    }
  }

  // log a debug message to the database
  debug(message) {
    if (this.logging_level === "debug") {
      this.database.addLog("debug", message);
    }
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
    this.ggs_status.aprsfi = this.aprs_fi.updateAndGetStatus();

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
