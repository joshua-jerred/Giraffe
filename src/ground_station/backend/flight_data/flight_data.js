const DiagnosticData = require("./diagnostic_data");
const MissionClock = require("./mission_clock");
const BitTest = require("./bit_test");
const LocationData = require("./location_data");
const Sequencer = require("./sequencer");
const FLIGHT_DATA_UPDATE_INTERVAL = 1000;

module.exports = class FlightData {
  constructor(global_state) {
    this.global_state = global_state;

    this.mission_clock = new MissionClock(global_state);
    this.diagnostics = new DiagnosticData(global_state);
    this.bit_test = new BitTest(global_state);
    this.location_data = new LocationData(global_state);
    this.sequencer = new Sequencer(global_state);

    this.general = {
      flight_phase: "n/d",
      flight_software_uptime: "n/d",
      flight_software_system_time_utc: "n/d",
      last_contact_tcp: "n/d",
      last_contact_telemetry: "n/d",
      last_updated: new Date(),
      gfs_time_synced: false,
      active_errors: "n/d",
      bit_test_status: "n/d",
      mission_clock_running: this.mission_clock.getIsRunning(),
      simulator_mode: false,
      current_position: {
        latitude: 0,
        longitude: 0,
        altitude: 0,
        heading: 0,
        horizontal_speed: 0,
        vertical_speed: 0,
      },
    };

    this.phase_prediction = {
      launch: 0,
      ascent: 0,
      descent: 0,
      recovery: 0,
      last_updated: new Date(),
    };

    this.extension_data = {
      last_updated: null,
      last_updated_source: "n/d",
      module_stats: {
        num_configured: 0,
        num_active: 0,
        num_inactive: 0,
      },
      extensions: [
        { name: "n/d", control_status: "n/d", internal_status: "n/d" },
      ],
    };

    // cache the last GFS UTC timestamp string so we don't double report location packets.
    this.gfs_tcp_last_valid_gps_frame_time = "n/d";

    setInterval(this.#cycle.bind(this), FLIGHT_DATA_UPDATE_INTERVAL);
  }

  getData(category) {
    if (category === "general") {
      return this.general;
    } else if (category === "location") {
      return this.location_data.getData();
    } else if (category === "phase_prediction") {
      return this.phase_prediction;
    } else if (category === "error_frame") {
      return this.diagnostics.getGeneralData();
    } else if (category === "extensions") {
      return this.extension_data;
    } else {
      console.log("Error: Invalid category in FlightData.getData - ", category);
    }
  }

  #cycle() {
    //   this.#updateLocation();
    this.general.active_errors = this.diagnostics.getNumActiveErrors();
    this.general.bit_test_status = this.bit_test.getBitTestStatus();
    this.general.mission_clock_running = this.mission_clock.getIsRunning();
    this.location_data.cycle();
    this.sequencer.cycle();

    const loc = this.location_data.getFlightPosition();
    if (loc) {
      this.general.current_position.valid = loc.valid;
      this.general.current_position.latitude = loc.latitude;
      this.general.current_position.longitude = loc.longitude;
      this.general.current_position.altitude = loc.altitude;
      this.general.current_position.heading = loc.heading;
      this.general.current_position.horizontal_speed = loc.horizontal_speed;
      this.general.current_position.vertical_speed = loc.vertical_speed;
    }
  }

  // ################ GENERAL DATA ################

  #newTcpContact() {
    let now = new Date();
    this.general.last_contact_tcp = `${now.getUTCHours()}:${now.getUTCMinutes()}:${now.getUTCSeconds()} UTC`;
    this.general.last_updated = new Date();
  }

  #updateFlightPhase(phase) {
    this.general.flight_phase = phase;
  }

  #updateSoftwareUptime(uptime_string) {
    this.general.flight_software_uptime = uptime_string;
  }

  #updateSoftwareSystemTimeUtc(time_string, max_skew_seconds) {
    this.general.flight_software_system_time_utc = time_string + " UTC";
    this.mission_clock.updateGfsUtcTime(time_string, max_skew_seconds);
    this.general.gfs_time_synced = this.mission_clock.isGfsTimeSynced();
  }

  #updatePhasePredictions(predictions) {
    this.phase_prediction.launch = predictions.launch;
    this.phase_prediction.ascent = predictions.ascent;
    this.phase_prediction.descent = predictions.descent;
    this.phase_prediction.recovery = predictions.recovery;
    this.phase_prediction.last_updated = new Date();
  }

  #updateInSimulatorMode(simulator_mode) {
    this.general.simulator_mode = simulator_mode;
  }

  // called by gfs_connection
  updateFlightDataFromGfsTcp(data) {
    // console.log(data);
    const MAX_CLOCK_SKEW_TCP_SECONDS = 5;
    try {
      this.#updateFlightPhase(data.flight_phase);
      this.#updateSoftwareUptime(data.uptime);
      this.#updateSoftwareSystemTimeUtc(
        data.system_time_utc,
        MAX_CLOCK_SKEW_TCP_SECONDS
      );
      this.#updatePhasePredictions(data.phase_predictions);

      // this needs to be removed. gfs should get it's launch position from the ground station
      // this.location.launch_position = data.launch_position;
      this.#updateInSimulatorMode(data.simulator_mode);
      this.#newTcpContact();
    } catch (e) {
      console.log("Error updating flight data from GFS TCP: ", e);
    }
  }

  // ################ LOCATION DATA ################

  updateLocationDataFromGfsTcp(data) {
    // this should possibly be deprecated, is there value in getting location via tcp outside of setup?

    // console.log("data\n\n", data);
    // console.log("todo, you need to get rid of this", data);
    try {
      if (data.have_gps_source === false) {
        return;
      }
      let loc = data.last_valid_gps_frame;
      // console.log(loc);

      // let last_frame = data.last_gps_frame;
      // this.location.valid = last_frame.is_valid;
      // this.location.latitude = last_frame.latitude;
      // this.location.longitude = last_frame.longitude;
      // this.location.altitude = last_frame.altitude;
      // this.location.heading = last_frame.heading_of_motion;
      // this.location.horizontal_speed = last_frame.horizontal_speed;
      // this.location.vertical_speed = last_frame.vertical_speed;
      // this.location.gps_time = last_frame.gps_utc_time;
      // this.location.last_update_source = "TCP";
      // this.location.last_updated = new Date();

      this.#newTcpContact();
      // this.mission_clock.updateGfsGpsUtcTime(last_frame.gps_utc_time);
    } catch (e) {
      console.log("Error updating location data from GFS TCP: ", e);
    }

    // this.global_state.gdl_telemetry.getMostRecentAprsPositionPacket();
    // this.last_gfs_gps_data =
    // this.global_state.gfs_connection.getRecentLocationData();
  }

  updateTimeDataFromGdlTelemetry(time_string) {
    this.general.last_contact_telemetry = time_string;
  }

  updateErrorFrameFromGfsTcp(data) {
    this.diagnostics.updateFromTcp(data);
  }

  /**
   *
   * @param {string} source - "tcp", "aprs", it's arbitrary, keep it short.
   * @param {int} num_configured - Number of configured extensions.
   * @param {int} num_active - Number of active extensions.
   * @param {int} num_inactive - Number of inactive extensions.
   * @param {{"name":{status:"",...}}} extensions - Data for each extensions.
   */
  #setExtensionData(
    source,
    num_configured,
    num_active,
    num_inactive,
    extensions
  ) {
    this.extension_data.last_updated_source = source || [];
    this.extension_data.last_updated = new Date();

    this.extension_data.module_stats = {
      num_configured: num_configured,
      num_active: num_active,
      num_inactive: num_inactive,
    };

    this.extension_data.extensions = extensions;
  }

  updateExtensionModuleStatsFromGfsTcp(data) {
    try {
      this.#setExtensionData(
        "tcp",
        data.num_configured,
        data.num_active,
        data.num_inactive,
        data.extension
      );
    } catch (e) {
      console.log("Error updating extension data from GFS TCP: ", e);
    }
  }
};
