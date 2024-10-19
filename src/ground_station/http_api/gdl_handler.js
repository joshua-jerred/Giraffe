const express = require("express");
const router = express.Router();
// const parseGetQuery = require("./query_parse");

module.exports = function (global_state) {
  // /api/gdl/settings
  router.get("/settings", (req, res, next) => {
    res.json(global_state.gdl_connection.getConfig());
  });

  router.put("/settings", (req, res, next) => {
    let result = global_state.gdl_connection.setConfig(req.body);
    if (result !== "success") {
      res.status(400).json({
        message: "Failed to set config. " + result,
      });
      return;
    }
    res.json({ message: "success" });
  });

  // /api/gdl/data
  router.get("/status", (req, res, next) => {
    res.json(global_state.gdl_connection.getStatus());
  });

  router.get("/telemetry", (req, res, next) => {
    const valid_categories = [
      "position_packets",
      "received_messages",
      "sent_messages",
    ];

    if (
      req.query["category"] === undefined ||
      !valid_categories.includes(req.query["category"])
    ) {
      res.status(400).json({ message: "malformed" });
      return;
    }

    let category = req.query["category"];
    let data = [];
    if (category === "received_messages") {
      global_state.database.getReceivedMessages((data) => {
        res.json(data);
      });
      return;
    } else if (category === "sent_messages") {
      global_state.database.getSentMessages((data) => {
        res.json(data);
      });
      return;
    }
    /// @todo implement other categories
    res.json(data);
  });

  router.post("/broadcast", (req, res, next) => {
    // parse the query
    if (req.body["data"] === undefined) {
      res
        .status(400)
        .json({ message: "Invalid body: must contain 'data':string" });
      return;
    }

    let data = req.body["data"];
    if (typeof data !== "string") {
      res.status(400).json({ message: "Invalid body: 'data' must be string" });
      return;
    }

    // arbitrary limits
    if (data.length < 5 || data.length > 150) {
      res.status(400).json({ message: "Data too short or too long" });
      return;
    }

    if (!global_state.gdl_connection.connected) {
      res
        .status(400)
        .json({ message: "GDL Server not connected or not running" });
      return;
    }

    let success = global_state.gdl_connection.sendBroadcast(data);
    if (!success) {
      res.status(400).json({
        message:
          "Failed to broadcast. Probably due to another broadcast request pending.",
      });
      return;
    }
    res.json({ message: "sent" });
  });

  router.post("/config", (req, res, next) => {
    if (typeof req.body !== "object") {
      res.status(400).json({ message: "Invalid body: must be object" });
      return;
    }

    let result = global_state.gdl_connection.setConfig(req.body);
    if (result !== "success") {
      res.status(400).json({
        message: "Failed to set config. " + result,
      });
      return;
    }

    res.json({ message: "success" });
  });

  router.get("/aprs_telemetry", (req, res, next) => {
    const start_time = parseInt(req.query["start_time"]) || 0;
    const end_time =
      parseInt(req.query["end_time"]) || (Date.now() / 1000).toFixed(0);
    const limit = parseInt(req.query["limit"]) || 50;

    if (typeof start_time !== "number" || typeof end_time !== "number") {
      const start_time_type = typeof start_time;
      const end_time_type = typeof end_time;
      res.status(400).json({
        message: `start_time and end_time must be numbers. They are ${start_time_type} and ${end_time_type}`,
      });
      return;
    }

    global_state.database.getReceivedAprsTelemetryData(
      start_time,
      end_time,
      limit,
      (data) => {
        res.json(data);
      }
    );
  });

  return router;
};
