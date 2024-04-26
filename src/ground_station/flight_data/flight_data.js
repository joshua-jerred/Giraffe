const FLIGHT_DATA_UPDATE_INTERVAL = 1000;

class MissionClock {
  constructor(global_state) {
    let input_time = global_state.ggs_db.get(
      "data",
      "mission_clock",
      "start_time"
    );

    this.start_time = new Date(input_time);
    this.start_time_locked = false; /// @TODO : Relies on the GFS being connected via TCP

    if (isNaN(this.start_time)) {
      this.resetClock();
    }
  }

  getStartTime() {
    return this.start_time.toUTCString();
  }

  resetClock() {
    console.log("Invalid Date");
    this.start_time = new Date();
    global_state.ggs_db.setKey(
      "data",
      "mission_clock",
      "start_time",
      this.start_time.toUTCString()
    );
  }

  getJsonData() {
    return {
      start_time: this.getStartTime(),
      start_time_locked: this.start_time_locked,
    };
  }
}

module.exports = class FlightData {
  constructor(global_state) {
    this.global_state = global_state;

    this.mission_clock = new MissionClock(global_state);

    this.general = {
      last_contact: "no-data",
      flight_mode: "no-data",
      last_updated: new Date(),
    };

    this.location = {
      source: "none",
      valid: false,
      latitude: 0,
      longitude: 0,
      altitude: 0,
      heading: 0,
      speed: 0,
      last_updated: new Date(),
    };

    setInterval(this.#cycle.bind(this), FLIGHT_DATA_UPDATE_INTERVAL);
  }

  getData(category) {
    if (category === "general") {
      return this.general;
    } else if (category === "location") {
      return this.location;
    }
  }

  #cycle() {
    this.#updateGeneral();
    this.#updateLocation();
  }

  #updateGeneral() {
    this.general.last_updated = new Date();
  }

  #updateLocation() {
    // this.global_state.gdl_telemetry.getMostRecentAprsPositionPacket();
    // this.last_gfs_gps_data =
    // this.global_state.gfs_connection.getRecentLocationData();
  }
};
