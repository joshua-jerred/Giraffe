const express = require("express");
const router = express.Router();
const loadMetaData = require("../metadata/metaLoader");
const valueCheck = require("../metadata/value_check");
const genericResponse = require("./generic_response");

module.exports = function (global_state) {
  // GET /ggs/settings
  router.get("/settings", (req, res, next) => {
    const valid_include = ["all", "values", "metadata"];

    // Required query parameter 'category'
    if (!req.query.hasOwnProperty("category")) {
      genericResponse(res, 400, "category is required.");
      return;
    }
    let category = req.query.category;

    // Optional query parameter 'include', defaults to 'all'
    let include_values = true;
    let include_metadata = true;
    if (
      req.query.hasOwnProperty("include") &&
      !valid_include.includes(req.query.include)
    ) {
      genericResponse(
        res,
        400,
        "include parameter must be one of these values: all, values, metadata"
      );
      return;
    } else if (req.query.hasOwnProperty("include")) {
      if (req.query.include === "values") {
        include_metadata = false;
      } else if (req.query.include === "metadata") {
        include_values = false;
      }
    }

    let response_body = {};

    // Load the metadata
    if (include_metadata) {
      metadata = loadMetaData("ggs", "settings", category);
      if (metadata === null) {
        genericResponse(res, 404, "category metadata not found.");
        return;
      }
      response_body["metadata"] = metadata;
    }

    if (include_values) {
      values = global_state.ggs_db.get("settings", category);

      if (values === null) {
        genericResponse(res, 404, "category values not found.");
        return;
      }
      response_body["values"] = values;
    }

    res.json(response_body);
  });

  router.put("/settings", (req, res, next) => {
    // Required query parameter 'category'
    if (!req.query.hasOwnProperty("category")) {
      genericResponse(res, 400, "category is required.");
      return;
    }
    let category = req.query.category;

    if (req.get("Content-Type") !== "application/json") {
      genericResponse(res, 400, "request body must be JSON.");
      return;
    }

    let body = req.body;
    if (!(typeof body === "object" && body !== null)) {
      genericResponse(res, 400, "request body must be a JSON object.");
    }

    // Load the metadata
    let metadata = loadMetaData("ggs", "settings", category);
    if (metadata === null) {
      genericResponse(res, 404, "category not found.");
      return;
    }

    // Validate the request body
    let invalid_keys = [];
    for (key in body) {
      let result = valueCheck(metadata, key, body[key]);
      if (result[0] === false) {
        invalid_keys.push([key, result[1]]);
      }
    }
    if (invalid_keys.length > 0) {
      genericResponse(
        res,
        400,
        "invalid key(s) in request body." + JSON.stringify(invalid_keys)
      );
      return;
    }

    // Update the database
    for (key in body) {
      // Don't save the value every iteration.
      global_state.ggs_db.setKey("settings", category, key, body[key], false);
    }
    global_state.ggs_db.save();

    genericResponse(res, 200, "success");
  });

  router.delete("/settings", (req, res, next) => {
    if (!req.query.hasOwnProperty("category")) {
      genericResponse(res, 400, "category is required.");
      return;
    }
    let category = req.query.category;
    let result = global_state.ggs_db.resetCategory("settings", category);
    if (result === false) {
      genericResponse(res, 404, "category not found.");
      return;
    }
    genericResponse(res, 200, "category deleted");
  });

  return router;
};