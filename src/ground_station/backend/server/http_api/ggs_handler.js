const express = require("express");
const router = express.Router();
const loadMetaData = require("../metadata/metaLoader");
const errorResponse = require("./error_response");

module.exports = function (global_state) {
  // GET /ggs/settings
  router.get("/settings", (req, res, next) => {
    const valid_include = ["all", "values", "metadata"];

    // Required query parameter 'category'
    if (!req.query.hasOwnProperty("category")) {
      errorResponse(res, 400, "category is required.");
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
      errorResponse(
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
        errorResponse(res, 404, "category metadata not found.");
        return;
      }
      response_body["metadata"] = metadata;
    }

    if (include_values) {
      values = global_state.ggs_db.get("settings", category);
      
      if (values === null) {
        errorResponse(res, 404, "category values not found.");
        return;
      }

      if (include_metadata) {
        response_body["values"] = values;
      }
    }

    res.json(response_body);
  });

  router.put("/settings", (req, res, next) => {
    // Required query parameter 'category'
    if (!req.query.hasOwnProperty("category")) {
      errorResponse(res, 400, "category is required.");
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
      errorResponse(
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
        errorResponse(res, 404, "category metadata not found.");
        return;
      }
      response_body["metadata"] = metadata;
    }

    if (include_values) {
      values = global_state.ggs_db.get("settings", category);
      
      if (values === null) {
        errorResponse(res, 404, "category values not found.");
        return;
      }

      if (include_metadata) {
        response_body["values"] = values;
      }
    }

    res.j
  });

  return router;
};
