const MissionClock = require("./mission_clock");
const FLIGHT_DATA_UPDATE_INTERVAL = 1000;

module.exports = class FlightData {
  constructor(global_state) {
    this.global_state = global_state;

    this.mission_clock = new MissionClock(global_state);

    this.general = {
      flight_phase: "n/d",
      flight_software_uptime: "n/d",
      flight_software_system_time_utc: "n/d",
      last_contact_tcp: "n/d",
      last_contact_telemetry: "n/d",
      last_updated: new Date(),
      gfs_time_synced: false,
    };

    this.phase_prediction = {
      launch: 0,
      ascent: 0,
      descent: 0,
      recovery: 0,
      last_updated: new Date(),
    };

    this.location = {
      launch_position: {
        valid: false,
        latitude: 0,
        longitude: 0,
        altitude: 0,
      },
      have_gps: "n/d",
      valid: "n/d",
      latitude: 0,
      longitude: 0,
      altitude: 0,
      heading: 0,
      horizontal_speed: 0,
      vertical_speed: 0,
      gps_time: "n/d",
      last_updated: "n/d",
    };

    setInterval(this.#cycle.bind(this), FLIGHT_DATA_UPDATE_INTERVAL);
  }

  getData(category) {
    if (category === "general") {
      return this.general;
    } else if (category === "location") {
      return this.location;
    } else if (category === "phase_prediction") {
      return this.phase_prediction;
    } else {
      console.log("Error: Invalid category in FlightData.getData()");
    }
  }

  #cycle() {
    //   this.#updateLocation();
  }

  // ################ GENERAL DATA ################

  #newTcpContact() {
    let now = new Date();
    this.general.last_contact_tcp = `${now.getUTCHours()}:${now.getUTCMinutes()}:${now.getUTCSeconds()} UTC`;
    this.general.last_updated = new Date();
  }

  #newTelemetryContact() {
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
    this.general.flight_software_system_time_utc = time_string;
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
      this.location.launch_position = data.launch_position;
      this.#newTcpContact();
    } catch (e) {
      console.log("Error updating flight data from GFS TCP: ", e);
    }
  }

  // ################ LOCATION DATA ################

  updateLocationDataFromGfsTcp(data) {
    // console.log("data\n\n", data);
    try {
      if (data.have_gps_source) {
        this.location.have_gps = true;
      } else {
        this.location.have_gps = false;
        return;
      }

      let last_frame = data.last_gps_frame;
      this.location.valid = last_frame.is_valid;
      this.location.latitude = last_frame.latitude;
      this.location.longitude = last_frame.longitude;
      this.location.altitude = last_frame.altitude;
      this.location.heading = last_frame.heading_of_motion;
      this.location.horizontal_speed = last_frame.horizontal_speed;
      this.location.vertical_speed = last_frame.vertical_speed;
      this.location.gps_time = last_frame.gps_utc_time;
      this.location.last_update_source = "TCP";
      this.location.last_updated = new Date();

      this.#newTcpContact();
      this.mission_clock.updateGfsGpsUtcTime(last_frame.gps_utc_time);
    } catch (e) {
      console.log("Error updating location data from GFS TCP: ", e);
    }

    // this.global_state.gdl_telemetry.getMostRecentAprsPositionPacket();
    // this.last_gfs_gps_data =
    // this.global_state.gfs_connection.getRecentLocationData();
  }
};
