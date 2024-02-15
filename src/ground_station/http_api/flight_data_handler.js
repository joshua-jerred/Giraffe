const express = require("express");
const router = express.Router();
const parseGetQuery = require("./query_parse");

module.exports = class FlightDataHandler {
  constructor(global_state) {
    this.global_state = global_state;

    this.general = {
      values: {
        last_aprs_contact: "no-data",
        flight_time: "no-data",
        flight_mode: "no-data",
      },
      metadata: {
        last_aprs_contact: {
          name: "Time of Last Contact",
          units: "ms",
          description: "The time since the last contact with the aircraft.",
        },
        flight_time: {
          name: "Flight Time",
          units: "ms",
          description: "The total time since the aircraft was armed.",
        },
        flight_mode: {
          name: "Flight Mode",
          units: "string",
          description: "The current flight mode of the aircraft.",
        },
      },
      timestamp: new Date(),
    };

    this.location_data = {
      values: {
        source: "none",
        valid: false,
        latitude: 0,
        longitude: 0,
        altitude: 0,
        heading: 0,
        speed: 0,
        last_update: 0,
      },
      metadata: {
        source: {
          name: "Location Source",
          description: "The source of the current location data.",
        },
        valid: {
          name: "Location Validity",
          description: "Whether the current location data is valid.",
        },
        latitude: {
          name: "Latitude",
          units: "degrees",
          description: "The latitude of the aircraft.",
        },
        longitude: {
          name: "Longitude",
          units: "degrees",
          description: "The longitude of the aircraft.",
        },
        altitude: {
          name: "Altitude",
          units: "meters",
          description: "The altitude of the aircraft.",
        },
        heading: {
          name: "Heading",
          units: "degrees",
          description: "The heading of the aircraft.",
        },
        speed: {
          name: "Speed",
          units: "m/s",
          description: "The speed of the aircraft.",
        },
        last_update: {
          name: "Last Update",
          units: "ms",
          description: "The time since the last update of the location data.",
        },
      },
      timestamp: new Date(),
    };

    this.last_aprs_contact = null;
    this.last_gfs_gps_data = null; // via tcp socket
  }

  #getResponseBody(item) {
    let response_body = {};
    response_body.values = item.values;
    response_body.metadata = item.metadata;
    response_body.metadata.MS_SINCE_LAST_UPDATE = Math.floor(
      new Date() - item.timestamp
    );

    return response_body;
  }

  handleRequest(req, res) {
    if (!req.query.hasOwnProperty("category")) {
      genericResponse(res, 400, "category is required.");
      return;
    }
    let category = req.query.category;

    let response_body = {};
    if (category === "location") {
      response_body = this.#getResponseBody(this.location_data);
    } else if (category === "general") {
      response_body = this.#getResponseBody(this.general);
    } else {
      genericResponse(res, 400, "category not found.");
      return;
    }

    res.json(response_body);
  }

  update() {
    // this.last_aprs_contact = "todo";
    // this.global_state.gdl_telemetry.getMostRecentAprsPositionPacket();
    this.last_gfs_gps_data =
      this.global_state.gfs_connection.getRecentLocationData();

    this.#updateLocation();
    this.#updateGeneral();
  }

  #updateGeneral() {
    if (this.last_aprs_contact !== null) {
      this.general.values.last_aprs_contact =
        this.last_aprs_contact.utc_timestamp;
    } else {
      this.general.values.last_aprs_contact = "no-contact";
    }

    this.general.timestamp = new Date();
  }

  #updateLocation() {
    if (this.last_aprs_contact === null && this.last_gfs_gps_data === null) {
      this.location_data.values.source = "none";
      return;
    } else if (this.last_aprs_contact === null) {
      this.location_data.values.source = "GFS";
      this.location_data.values.fix = this.last_gfs_gps_data.fix;
      this.location_data.values.valid = true;
      this.location_data.values.latitude = this.last_gfs_gps_data.latitude;
      this.location_data.values.longitude = this.last_gfs_gps_data.longitude;
      this.location_data.values.altitude = this.last_gfs_gps_data.altitude;
      this.location_data.values.heading =
        this.last_gfs_gps_data.heading_of_motion;
      this.location_data.values.speed = this.last_gfs_gps_data.horizontal_speed;
      this.location_data.values.last_update =
        this.last_gfs_gps_data.gps_utc_time;
      this.location_data.values.vertical_speed =
        this.last_gfs_gps_data.vertical_speed;
    } else {
      this.location_data.values.source = "APRS";
      this.location_data.values.valid = true;
      this.location_data.values.latitude = this.last_aprs_contact.latitude;
      this.location_data.values.longitude = this.last_aprs_contact.longitude;
      this.location_data.values.altitude = this.last_aprs_contact.altitude;
      this.location_data.values.heading = this.last_aprs_contact.course;
      this.location_data.values.speed = this.last_aprs_contact.speed;
      this.location_data.values.last_update =
        this.last_aprs_contact.utc_timestamp;
    }

    this.location_data.timestamp = new Date();
  }
};
