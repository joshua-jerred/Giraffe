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

module.exports = class LocationData {
  constructor(global_state) {
    this.global_state = global_state;

    // launch and last position of the flight system
    this.last_updated = new Date();
    this.launch_position = LocationStructure;
    this.ground_station_position = LocationStructure;
    this.last_known_flight_position = LocationStructure;

    this.setting_update_timer = new Timer(2500);
    this.#loadLatestLocationData();
  }

  cycle() {
    if (!this.setting_update_timer.isExpired()) {
      return;
    }
    this.setting_update_timer.restart();

    // Using the validity flag to indicate if the launch position is set
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

  #loadLatestLocationData() {
    return Database.models.LocationData.findOne({
      order: [["timestamp", "DESC"]],
    })
      .then((location_data) => {
        if (location_data) {
          this.last_known_flight_position = {
            timestamp: location_data.timestamp,
            valid: location_data.valid,
            latitude: location_data.latitude,
            longitude: location_data.longitude,
            altitude: location_data.altitude,
            heading: location_data.heading,
            horizontal_speed: location_data.horizontal_speed,
            vertical_speed: location_data.vertical_speed,
            additional_data: location_data.additional_data,
          };
        }
      })
      .catch((error) => {
        console.error("Error loading latest location data:", error);
      });
  }

  getData() {
    let return_data = {
      ground_station_position: this.ground_station_position,
      launch_position: this.launch_position,
      last_known_position: this.last_known_flight_position,
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
    for (const key in return_data.last_known_position) {
      if (
        return_data.last_known_position[key] === null ||
        return_data.last_known_position[key] === undefined
      ) {
        return_data.last_known_position[key] = "n/d";
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
};
