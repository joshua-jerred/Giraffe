const Database = require("../db");
const Timer = require("../helpers/timer");

const LocationStructure = {
  timestamp: "n/d",
  valid: false,
  latitude: 0,
  longitude: 0,
  altitude: 0,
  heading: "n/d",
  horizontal_speed: "n/d",
  vertical_speed: "n/d",
  additional_data: "n/d",
};

const LOCATION_AGE_TO_MARK_AS_STALE_SECONDS = 60 * 5; // 5 minutes

module.exports = class LocationData {
  constructor(global_state) {
    this.global_state = global_state;

    // launch and last position of the flight system
    this.last_updated = new Date();
    this.launch_position = JSON.parse(JSON.stringify(LocationStructure));
    this.ground_station_position = JSON.parse(
      JSON.stringify(LocationStructure)
    );

    this.flight_location_status = "unknown";
    this.flight_location_age_seconds = "n/d";
    this.flight_position = JSON.parse(JSON.stringify(LocationStructure));

    this.setting_update_timer = new Timer(2500);
    this.#loadLaunchPosition();
    this.#loadLatestFlightLocationData();
  }

  cycle() {
    this.#loadLatestFlightLocationData();

    if (!this.setting_update_timer.isExpired()) {
      return;
    }
    this.setting_update_timer.restart();

    // Using the validity flag to indicate if the launch position is set
    this.#loadLaunchPosition();
  }

  #loadLatestFlightLocationData() {
    return Database.models.LocationData.findOne({
      order: [["timestamp", "DESC"]],
    })
      .then((location_data) => {
        if (location_data) {
          this.flight_position = {
            timestamp: location_data.timestamp,
            valid: location_data.valid,
            latitude: location_data.latitude,
            longitude: location_data.longitude,
            altitude: location_data.altitude,
            heading: location_data.heading,
            horizontal_speed: location_data.horizontal_speed,
            vertical_speed: location_data.vertical_speed,
            additional_data: location_data.additional_data,
            source: location_data.source,
          };
        }

        // calculate the age of the last location data
        const now = new Date();
        const last_updated = new Date(location_data.timestamp);
        const age_in_seconds = (now.getTime() - last_updated.getTime()) / 1000;
        this.flight_location_age_seconds = age_in_seconds.toFixed(0);
        if (age_in_seconds > LOCATION_AGE_TO_MARK_AS_STALE_SECONDS) {
          this.flight_location_status = "stale";
        } else {
          this.flight_location_status = "valid";
        }
      })
      .catch((error) => {
        console.error("Error loading latest location data:", error);
      });
  }

  #loadLaunchPosition() {
    this.launch_position.timestamp = this.global_state.ggs_db.get(
      "settings",
      "ggs_settings",
      "launch_position_set_time"
    );
    this.launch_position.valid = this.global_state.ggs_db.get(
      "settings",
      "ggs_settings",
      "launch_position_set"
    );
    this.launch_position.latitude = this.global_state.ggs_db.get(
      "settings",
      "ggs_settings",
      "launch_position_latitude"
    );
    this.launch_position.longitude = this.global_state.ggs_db.get(
      "settings",
      "ggs_settings",
      "launch_position_longitude"
    );
    this.launch_position.altitude = this.global_state.ggs_db.get(
      "settings",
      "ggs_settings",
      "launch_position_altitude"
    );
  }

  getData() {
    let return_data = {
      flight_location_status: this.flight_location_status,
      flight_location_age_seconds: this.flight_location_age_seconds,
      flight_position: this.flight_position,
      ground_station_position: this.ground_station_position,
      launch_position: this.launch_position,
      last_updated: this.last_updated,
    };
    for (const key in return_data.ground_station_position) {
      if (
        return_data.ground_station_position[key] === null ||
        return_data.ground_station_position[key] === undefined
      ) {
        return_data.ground_station_position[key] = "n/d";
      }
    }
    for (const key in return_data.launch_position) {
      if (
        return_data.launch_position[key] === null ||
        return_data.launch_position[key] === undefined
      ) {
        return_data.launch_position[key] = "n/d";
      }
    }
    for (const key in return_data.flight_position) {
      if (
        return_data.flight_position[key] === null ||
        return_data.flight_position[key] === undefined
      ) {
        return_data.flight_position[key] = "n/d";
      }
    }

    return return_data;
  }

  addNewLocationReport(source, location) {
    console.log("Adding new location report:", source, location);

    Database.models.LocationData.create({
      valid: location.valid || true,
      source: source,
      latitude: location.latitude,
      longitude: location.longitude,
      altitude: location.altitude,
      heading: location.heading || null,
      horizontal_speed: location.horizontal_speed || null,
      vertical_speed: location.vertical_speed || null,
      additional_data: location.additional_data || null,
    });
  }

  setLaunchPosition(position_data) {
    const required_fields = ["latitude", "longitude", "altitude"];

    for (const field of required_fields) {
      if (position_data[field] === undefined) {
        console.error(
          `Invalid launch position data: ${field} is required but not provided`
        );
        return false;
      }

      if (typeof position_data[field] !== "number") {
        console.error(
          `Invalid launch position data: ${field} must be a number`
        );
        return false;
      }
    }

    if (position_data.latitude < -90 || position_data.latitude > 90) {
      return "Invalid launch position data: latitude out of range";
    }
    if (position_data.longitude < -180 || position_data.longitude > 180) {
      return "Invalid launch position data: longitude out of range";
    }
    if (position_data.altitude < -500) {
      return "Invalid launch position data: altitude cannot be unreasonably low";
    }

    const save_immediately = false;
    this.global_state.ggs_db.setKey(
      "settings",
      "ggs_settings",
      "launch_position_latitude",
      position_data.latitude,
      save_immediately
    );
    this.global_state.ggs_db.setKey(
      "settings",
      "ggs_settings",
      "launch_position_longitude",
      position_data.longitude,
      save_immediately
    );
    this.global_state.ggs_db.setKey(
      "settings",
      "ggs_settings",
      "launch_position_altitude",
      position_data.altitude,
      save_immediately
    );
    this.global_state.ggs_db.setKey(
      "settings",
      "ggs_settings",
      "launch_position_set",
      true, // set/valid flag to true
      save_immediately
    );
    this.global_state.ggs_db.setKey(
      "settings",
      "ggs_settings",
      "launch_position_set_time",
      new Date(),
      save_immediately
    );
    this.global_state.ggs_db.save();

    this.#loadLaunchPosition();

    return "success";
  }
};
