const express = require("express");
const router = express.Router();
const parseGetQuery = require("./query_parse");

module.exports = class FlightDataHandler {
  constructor(global_state) {
    this.global_state = global_state;

    this.general = {
      values: {
        time_of_last_contact: "no-data",
        flight_time: "no-data",
        flight_mode: "no-data",
      },
      metadata: {
        time_of_last_contact: {
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
        time_of_last_update: 0,
      },
      metadata: {},
      timestamp: new Date(),
    };
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

  update() {}
};
