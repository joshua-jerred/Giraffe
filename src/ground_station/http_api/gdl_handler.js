const express = require("express");
const router = express.Router();
const parseGetQuery = require("./query_parse");

module.exports = function (global_state) {
  // /api/gdl/settings
  // router.get("/settings", (req, res, next) => {
  // parseGetQuery(req, res, "gfs", "settings", global_state);
  // });

  // router.put("/settings", (req, res, next) => {
  // res.json({ message: "Not yet implemented!" });
  // });

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

  return router;
};
