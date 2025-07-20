const { where } = require("sequelize");
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

// Distance between two coordinates in meters
function distanceBetweenCoordinates(lat1, lon1, lat2, lon2) {
  const DEG2RAD = Math.PI / 180;
  const distance_lat = (lat2 - lat1) * DEG2RAD;
  const distance_lon = (lon2 - lon1) * DEG2RAD;
  const a =
    Math.sin(distance_lat / 2) * Math.sin(distance_lat / 2) +
    Math.cos(lat1 * DEG2RAD) *
      Math.cos(lat2 * DEG2RAD) *
      Math.sin(distance_lon / 2) *
      Math.sin(distance_lon / 2);
  const c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));
  const R = 6371; // Radius of the Earth in meters
  const distance = R * c; // Distance in meters
  return distance;
}

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

    this.distance_from_launch_position_km = "n/d";
    this.flight_path_distance_km = "n/d";

    // This one stands alone from the rest of the data, request wise.
    this.flight_path_data = [];

    this.setting_update_timer = new Timer(2500);
    this.#loadLaunchPosition();
    this.#loadLatestFlightLocationData();
    this.#loadFlightPathData();
  }

  cycle() {
    this.#loadLatestFlightLocationData();

    if (!this.setting_update_timer.isExpired()) {
      return;
    }
    this.setting_update_timer.restart();

    // Using the validity flag to indicate if the launch position is set
    this.#loadLaunchPosition();

    if (
      this.launch_position.valid === true &&
      this.flight_position.valid === true
    ) {
      const dist = distanceBetweenCoordinates(
        this.launch_position.latitude,
        this.launch_position.longitude,
        this.flight_position.latitude,
        this.flight_position.longitude
      );
      this.distance_from_launch_position_km = dist.toFixed(2);
    }

    this.#loadFlightPathData();
  }

  getFlightPosition() {
    return this.flight_position;
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

  #loadFlightPathData() {
    // inefficient, but who cares
    return Database.models.LocationData.findAll({
      order: [["timestamp", "DESC"]],
      where: {
        valid: true,
        archived: false,
      },
      limit: 400,
    })
      .then((location_data) => {
        this.flight_path_data = location_data.map((data) => ({
          timestamp: data.timestamp,
          latitude: data.latitude,
          longitude: data.longitude,
          altitude: data.altitude,
          heading: data.heading,
          horizontal_speed: data.horizontal_speed,
          vertical_speed: data.vertical_speed,
          additional_data: data.additional_data,
          valid: data.valid,
        }));

        let path_distance = 0;
        for (const position of this.flight_path_data) {
          if (position.valid === true) {
            if (this.flight_path_data.indexOf(position) > 0) {
              const prev_position =
                this.flight_path_data[
                  this.flight_path_data.indexOf(position) - 1
                ];
              path_distance += distanceBetweenCoordinates(
                prev_position.latitude,
                prev_position.longitude,
                position.latitude,
                position.longitude
              );
            }
          }
        }
        this.flight_path_distance_km = path_distance.toFixed(2);
      })
      .catch((error) => {
        console.error("Error loading flight path data:", error);
      });
  }

  getData() {
    let return_data = {
      flight_location_status: this.flight_location_status,
      flight_location_age_seconds: this.flight_location_age_seconds,
      distance_from_launch_position_km: this.distance_from_launch_position_km,
      flight_path_distance_km: this.flight_path_distance_km,
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
    // console.log("Adding new location report:", source, location);

    Database.models.LocationData.create({
      valid: location.valid || true,
      source: source,
      latitude: location.latitude,
      longitude: location.longitude,
      altitude: location.altitude,
      heading: typeof location.heading == "number" ? location.heading : null,
      horizontal_speed:
        typeof location.horizontal_speed == "number"
          ? location.horizontal_speed
          : null,
      vertical_speed:
        typeof location.vertical_speed == "number"
          ? location.vertical_speed
          : null,
      additional_data: location.additional_data || null,
    });
  }

  invalidateLaunchPosition() {
    this.launch_position.valid = false;
    this.global_state.ggs_db.setKey(
      "settings",
      "ggs_settings",
      "launch_position_set",
      false, // set/valid flag to false
      true // save immediately
    );
  }

  setLaunchPosition(position_data) {
    const required_fields = ["latitude", "longitude", "altitude"];

    console.warn(
      "Setting launch position happens on both the ground station and the flight system. Fix this"
    );

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

  getFlightPathData() {
    return this.flight_path_data.map((data) => ({
      // timestamp: data.timestamp,
      latitude: data.latitude,
      longitude: data.longitude,
      // altitude: data.altitude,
      // heading: data.heading,
      // horizontal_speed: data.horizontal_speed,
      // vertical_speed: data.vertical_speed,
      // additional_data: data.additional_data,
    }));
  }
};
