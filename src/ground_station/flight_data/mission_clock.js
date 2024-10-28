module.exports = class MissionClock {
  constructor(global_state) {
    this.global_state = global_state;
    let input_is_running = global_state.ggs_db.get(
      "data",
      "mission_clock",
      "is_running"
    );

    console.log(
      "input_is_running: ",
      input_is_running,
      typeof input_is_running
    );

    let input_time = global_state.ggs_db.get(
      "data",
      "mission_clock",
      "start_time"
    );

    this.is_running =
      typeof input_is_running === "boolean" ? input_is_running : false;
    this.start_time = new Date(input_time);

    if (isNaN(this.start_time)) {
      console.log("Invalid Mission Start Date - Resetting");
      console.log(this.resetClock());
    }

    this.gfs_utc_time = "n/d";
    this.gfs_time_skew = "n/d";
    this.gfs_gps_utc_time = "n/d";
    this.gfs_time_synced = false;
    this.gfs_time_last_updated = new Date();
  }

  getIsRunning() {
    return this.is_running;
  }

  getStartTime() {
    return this.start_time.toUTCString();
  }

  getElapsedTime() {
    let current_time = new Date();
    let elapsed_time = current_time - this.start_time;
    return Math.floor(elapsed_time / 1000);
  }

  getUtcTime() {
    let current_time = new Date();
    let hours = current_time.getUTCHours();
    if (hours < 10) {
      hours = "0" + hours;
    }
    let minutes = current_time.getUTCMinutes();
    if (minutes < 10) {
      minutes = "0" + minutes;
    }
    let seconds = current_time.getUTCSeconds();
    if (seconds < 10) {
      seconds = "0" + seconds;
    }
    return `${hours}:${minutes}:${seconds}`;
  }

  getGfsGpsUtcTime() {
    if (this.gfs_gps_utc_time.length > 8) {
      return this.gfs_gps_utc_time.split(" ")[1];
    }
  }

  isGfsTimeSynced() {
    const TIME_EXPIRATION = 60; // seconds

    let seconds_since_updated =
      (new Date() - this.gfs_time_last_updated) / 1000;

    if (seconds_since_updated > TIME_EXPIRATION) {
      return false;
    }

    return this.gfs_time_synced;
  }

  /**
   * Start the mission clock
   * @param {number} skew_seconds - The number of seconds to skew the start time
   * into the future by.
   */
  resetClock(skew_seconds = null) {
    if (isNaN(skew_seconds)) {
      return "skew_seconds error";
    } else if (skew_seconds < 0) {
      return "skew_seconds must be positive";
    }

    if (skew_seconds === null) {
      skew_seconds = 0;
    }

    this.is_running = true;
    this.start_time = new Date();
    this.start_time.setSeconds(this.start_time.getSeconds() + skew_seconds);

    console.log(
      "Resetting Clock: ",
      this.start_time,
      this.start_time.getSeconds()
    );

    this.global_state.ggs_db.setKey(
      "data",
      "mission_clock",
      "is_running",
      this.is_running
    );
    this.global_state.ggs_db.setKey(
      "data",
      "mission_clock",
      "start_time",
      this.start_time.toUTCString()
    );

    return "success";
  }

  stopClock() {
    this.is_running = false;
    this.global_state.ggs_db.setKey(
      "data",
      "mission_clock",
      "is_running",
      this.is_running.toString()
    );

    return "success";
  }

  getJsonData() {
    return {
      ggs_utc_time: this.getUtcTime(),
      gfs_utc_time: this.gfs_utc_time,
      gfs_time_skew: this.gfs_time_skew,
      gfs_gps_utc_time: this.getGfsGpsUtcTime(),
      mission_elapsed_time: this.getElapsedTime(),
      is_running: this.getIsRunning(),
      start_time: this.getStartTime(),
    };
  }

  /**
   * The UTC time from the flight software, used to verify that the GFS computer
   * is in sync with the flight software.
   * @param {string} time hh:mm:ss
   * @param {number} max_skew The maximum number of seconds the GFS computer's
   * clock can be off by. When this is updated via TCP the skew is expected to
   * be less. If updated via telemetry the skew is expected to be greater.
   */
  updateGfsUtcTime(time, max_skew) {
    if (isNaN(max_skew)) {
      console.log("Invalid max_skew");
      return;
    }

    let current_time = new Date();
    let time_parts = time.split(":");
    current_time.setUTCHours(time_parts[0]);
    current_time.setUTCMinutes(time_parts[1]);
    current_time.setUTCSeconds(time_parts[2]);

    let time_skew = Math.abs(current_time - new Date());

    if (time_skew > max_skew * 1000) {
      console.log("GFS Clock Skew Exceeded: ", time_skew);
      this.gfs_time_synced = false;
    } else {
      this.gfs_time_synced = true;
    }

    this.gfs_utc_time = time;
    this.gfs_time_skew = time_skew;
    this.gfs_time_last_updated = new Date();
  }

  // Same as gfsUtcTime
  updateGfsGpsUtcTime(time) {
    this.gfs_gps_utc_time = time;
  }
};
