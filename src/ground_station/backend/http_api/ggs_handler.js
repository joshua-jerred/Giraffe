const express = require("express");
const router = express.Router();
const loadMetaData = require("../metadata/metaLoader");
const valueCheck = require("../metadata/value_check");
const genericResponse = require("./generic_response");
const parseGetQuery = require("./query_parse");

module.exports = function (global_state) {
  // GET /ggs/settings
  router.get("/settings", (req, res, next) => {
    parseGetQuery(req, res, "ggs", "settings", global_state);
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

  router.get("/data", (req, res, next) => {
    parseGetQuery(req, res, "ggs", "data", global_state);
  });

  router.get("/aprs_fi", (req, res) => {
    global_state.database.getRecentAprsFiData((rows) => {
      res.json(rows);
      res.end();
    });
  });

  router.get("/log", (req, res) => {
    global_state.database.getRecentLogData((rows) => {
      res.json(rows);
      res.end();
    });
  });

  router.delete("/log", (req, res) => {
    if (!req.body.hasOwnProperty("id")) {
      genericResponse(res, 400, "id is required.");
      return;
    }

    if (req.body.id === "all") {
      global_state.database.deleteAllLogEntries((err, deleted_count) => {
        if (err) {
          genericResponse(res, 500, err);
          return;
        }
        genericResponse(
          res,
          200,
          "success - deleted " + deleted_count + " records."
        );
      });
      return;
    }

    global_state.database.deleteLogEntry(req.body.id, (err, updates) => {
      if (err) {
        genericResponse(res, 500, err);
        return;
      }

      if (updates > 0) {
        genericResponse(res, 200, "success");
      } else if (updates === 0) {
        genericResponse(res, 401, "id not found, no records updated.");
      } else {
        genericResponse(res, 500, "unknown error.");
      }
    });
  });

  router.get("/log/level", (req, res) => {
    const level = global_state.ggs_db.get(
      "settings",
      "ggs_settings",
      "logging_level"
    );
    res.json({
      level: level,
    });
  });

  router.put("/log/level", (req, res) => {
    if (!req.body.hasOwnProperty("level")) {
      genericResponse(res, 400, "level is required.");
      return;
    }

    if (["error", "warning", "info", "debug"].includes(req.body.level)) {
      global_state.ggs_db.setKey(
        "settings",
        "ggs_settings",
        "logging_level",
        req.body.level,
        true // Save
      );
      genericResponse(res, 200, "success");
      global_state.logging_level = req.body.level;
      global_state.info("Log level set to " + req.body.level);
      return;
    }

    genericResponse(res, 400, "invalid level.");
  });

  return router;
};
