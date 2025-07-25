const express = require("express");
const genericResponse = require("./generic_response");

const responseMetadata = {
  general: {
    flight_phase: {
      name: "Flight Phase",
      units: "string",
      description: "The current flight phase of the computer.",
    },
    flight_software_uptime: {
      name: "GFS Uptime",
      units: "hh:mm:ss",
      description: "The uptime of the flight software.",
    },
    last_contact_tcp: {
      name: "Last TCP Contact",
      units: "ms",
      description: "The time since the last contact with the flight computer.",
    },
    last_contact_telemetry: {
      name: "Last Telemetry Contact",
      units: "ms",
      description: "The time since the last contact with the flight computer.",
    },
  },
  phase_prediction: {},
  location: {
    flight_location_status: {
      name: "Location Status",
      description: "Whether the current location data is valid.",
    },
    flight_location_age_seconds: {
      name: "Age of Flight Location",
      units: "seconds",
      description: "The age of the flight location data in seconds.",
    },
    distance_from_launch_position_km: {
      name: "Distance from Launch Position",
      units: "km",
      description: "The distance from the launch position in km.",
    },
    flight_path_distance_km: {
      name: "Flight Path Distance",
      units: "km",
      description:
        "The cumulative distance of the flight path in km since launch.",
    },
    // latitude: {
    // name: "Latitude",
    // units: "degrees",
    // description: "The latitude of the aircraft.",
    // round: 6,
    // },
    // longitude: {
    // name: "Longitude",
    // units: "degrees",
    // description: "The longitude of the aircraft.",
    // round: 6,
    // },
    // altitude: {
    //   name: "Altitude",
    //   units: "meters",
    //   description: "The altitude of the flight computer.",
    //   round: 1,
    // },
    // heading: {
    //   name: "Heading",
    //   units: "degrees",
    //   description: "The heading of the flight computer.",
    //   round: 1,
    // },
    // horizontal_speed: {
    //   name: "H Speed",
    //   units: "m/s",
    //   description: "The GPS indicated horizontal speed of the flight computer.",
    //   round: 1,
    // },
    // vertical_speed: {
    //   name: "V Speed",
    //   units: "m/s",
    //   description: "The GPS indicated vertical speed of the flight computer.",
    //   round: 1,
    // },
    // gps_time: {
    // name: "GPS Time",
    // units: "hh:mm:ss",
    // description: "The time since the last update of the location data.",
    // },
    // last_update_source: {
    //   name: "Last Update Source",
    //   description: "The source of the last update of the location data.",
    // },
    // have_gps: {
    // name: "Have GPS",
    // description: "Whether GFS has a GPS module.",
    // },
  },
  error_frame: {},
  extensions: {},
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

    const validCategories = [
      "location",
      "flight_path",
      "general",
      "mission_clock",
      "phase_prediction",
      "error_frame",
      "extensions",
    ];

    let category = req.query.category;

    if (!validCategories.includes(category)) {
      genericResponse(res, 400, "category not found.");
      return;
    }

    if (category === "mission_clock") {
      res.json(this.global_state.flight_data.mission_clock.getJsonData());
    } else if (category === "flight_path") {
      res.json(this.global_state.flight_data.location_data.getFlightPathData());
    } else {
      res.json(this.#getResponseBody(category));
    }
  }

  handlePutRequest(req, res) {
    try {
      if (!req.query.hasOwnProperty("category")) {
        genericResponse(res, 400, "category is required.");
        return;
      }

      const validCategories = [
        "start_mission_clock",
        "stop_mission_clock",
        "set_launch_position",
      ];
      let category = req.query.category;
      if (!validCategories.includes(category)) {
        genericResponse(res, 400, "category not found.");
        return;
      }

      if (category === "start_mission_clock") {
        let resp_status =
          this.global_state.flight_data.mission_clock.resetClock(
            req.body.clock_skew
          );
        res.json({ status: resp_status });
      } else if (category === "stop_mission_clock") {
        let resp_status =
          this.global_state.flight_data.mission_clock.stopClock();
        res.json({ status: resp_status });
      } else if (category === "set_launch_position") {
        let resp_status =
          this.global_state.flight_data.location_data.setLaunchPosition(
            req.body
          );

        if (resp_status === "success") {
          res.json({ status: resp_status });
        } else {
          genericResponse(res, 400, resp_status);
        }
      } else {
        // res.json(this.#getResponseBody(category));
      }
    } catch (error) {
      console.error(error);
      genericResponse(res, 500, error);
    }
  }
};
