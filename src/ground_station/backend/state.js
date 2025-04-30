const GgsDataBase = require("./db/ggs_db.js");
const GfsConnection = require("./gfs_connection/gfs_connection.js");
const GdlConnection = require("./gdl_connection/gdl_connection.js");
const FsaConnection = require("./fsa_connection/fsa_connection.js");
const FsaControl = require("./fsa_connection/fsa_control.js");
const FlightData = require("./flight_data/flight_data.js");
const FlightDataHandler = require("./http_api/flight_data_handler.js");
const GfsSocketRequester = require("./gfs_connection/gfs_socket_request.js");

const Database = require("./db/database.js");
const InfluxWriter = require("./db/influx.js");
const { Point } = require("@influxdata/influxdb-client");

const AprsFi = require("./aprs/aprs_fi.js");
const AprsIs = require("./aprs/aprs_is.js");

// Update interval for the global state
const kGlobalStateUpdateInterval = 500;

class GlobalState {
  constructor() {
    // ggs_db is a deprecated name, this is configuration data now
    this.ggs_db = new GgsDataBase();
    this.influx_writer = new InfluxWriter(this);
    this.influx_enabled = this.ggs_db.get(
      "settings",
      "influx_db",
      "influx_enabled"
    );

    this.database = new Database(this);
    this.flight_data = new FlightData(this);

    this.fsa_connection = new FsaConnection(this);
    this.fsa_control = new FsaControl(this);
    this.gfs_connection = new GfsConnection(this);
    this.gdl_connection = new GdlConnection(this);
    this.flight_data_handler = new FlightDataHandler(this);
    this.gfs_socket_requester = new GfsSocketRequester(this);

    this.aprs_fi = new AprsFi(this);
    this.aprs_is = new AprsIs(this);

    this.logging_level = this.ggs_db.get(
      "settings",
      "ggs_settings",
      "logging_level"
    );

    this.ggs_status = {
      status: "ok",
      influxdb: "n/d",
      gfs: "n/d",
      gdl: "n/d",
      fsa: "n/d",
      fsa_gfs_status: "n/d",
      telemetry_uplink: "n/d",
      telemetry_downlink: "n/d",
      aprs_is: "n/d",
      aprs_fi: "n/d",
      total_http_requests: 0,
      // general: this.flight_data_handler.general.values,
      // location: this.flight_data_handler.location_data.values,
    };

    setInterval(this.cycle.bind(this), kGlobalStateUpdateInterval);
    this.info("Started Ground Station");
  }

  // log an error message to the database
  error(message) {
    this.database.addLog("error", message);
  }

  // log a warning message to the database
  warning(message) {
    if (this.logging_level !== "error") {
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
    /// @todo these should NOT be updated here. They should try to update themselves
    this.gfs_connection.update();
    // this.flight_data_handler.update();

    this.ggs_status.gfs = this.gfs_connection.status;
    this.ggs_status.gdl = this.gdl_connection.status;
    this.ggs_status.aprs_fi = this.aprs_fi.updateAndGetStatus();
    this.ggs_status.aprs_is = this.aprs_is.updateAndGetStatus();

    // update the telemetry status with GDL data
    if (this.ggs_status.gdl === "connected") {
      try {
        let gdl_status = this.gdl_connection.getStatus();
        this.ggs_status.telemetry_uplink = gdl_status.data.tpl_uplink;
        this.ggs_status.telemetry_downlink = gdl_status.data.tpl_downlink;
      } catch (error) {
        this.ggs_status.telemetry_uplink = "N/D";
        this.ggs_status.telemetry_downlink = "N/D";
      }
    } else if (this.ggs_status.gdl === "disconnected") {
      this.ggs_status.telemetry_uplink = "down";
      this.ggs_status.telemetry_downlink = "down";
    } else if (this.ggs_status.gdl === "disabled") {
      this.ggs_status.telemetry_uplink = "disabled";
      this.ggs_status.telemetry_downlink = "disabled";
    }

    // update the FSA status
    this.ggs_status.fsa = this.fsa_connection.getFsaConnectionStatus();
    this.ggs_status.fsa_gfs_status = this.fsa_connection.getFsaGfsStatus();

    // update inluxdb status
    if (this.influx_enabled) {
      let point = new Point("ggs_status")
        .stringField("influxdb", this.ggs_status.influxdb)
        .stringField("gfs", this.ggs_status.gfs)
        .stringField("gdl", this.ggs_status.gdl)
        .stringField("fsa", this.ggs_status.fsa)
        .stringField("fsa_gfs_status", this.ggs_status.fsa_gfs_status)
        .stringField("telemetry_uplink", this.ggs_status.telemetry_uplink)
        .stringField("telemetry_downlink", this.ggs_status.telemetry_downlink)
        .stringField("aprs_fi", this.ggs_status.aprs_fi)
        .intField("total_http_requests", this.ggs_status.total_http_requests);

      this.influx_writer.write(point);
      this.influx_writer.flush();
    }
  }
}

module.exports = GlobalState;
