const express = require("express");
const router = express.Router();
const parseGetQuery = require("./query_parse");

const responseMetadata = {
  general: {
    last_contact: {
      name: "Time of Last Contact",
      units: "ms",
      description: "The time since the last contact with the flight computer.",
    },
    flight_time: {
      name: "Flight Time",
      units: "ms",
      description: "The total time since the flight computer was launched.",
    },
    flight_mode: {
      name: "Flight Mode",
      units: "string",
      description: "The current flight mode of the computer.",
    },
  },
  location: {
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
};

module.exports = class FlightDataHandler {
  constructor(global_state) {
    this.global_state = global_state;
  }

  #getResponseBody(category) {
    let response_body = {};

    response_body.values = structuredClone(
      this.global_state.flight_data.getData(category)
    );
    response_body.metadata = responseMetadata[category];

    response_body.metadata["MS_SINCE_LAST_UPDATE"] = Math.floor(
      new Date() - response_body.values.last_updated
    );

    delete response_body.values.last_updated;

    return response_body;
  }

  handleGetRequest(req, res) {
    if (!req.query.hasOwnProperty("category")) {
      genericResponse(res, 400, "category is required.");
      return;
    }

    const validCategories = ["location", "general", "mission_clock"];

    let category = req.query.category;

    if (!validCategories.includes(category)) {
      genericResponse(res, 400, "category not found.");
      return;
    }

    if (category === "mission_clock") {
      res.json(this.global_state.flight_data.mission_clock.getJsonData());
    } else {
      res.json(this.#getResponseBody(category));
    }
  }
};
