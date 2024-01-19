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
  router.get("/data", (req, res, next) => {
    parseGetQuery(req, res, "gdl", "data", global_state);
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
    let data = global_state.gdl_connection.telemetry_data[category];
    res.json(data);
  });

  router.post("/broadcast", (req, res, next) => {
    // parse the query
    if (req.query["data"] === undefined) {
      res
        .status(400)
        .json({ message: "Invalid query: must contain 'data':string" });
      return;
    }

    let data = req.query["data"];
    if (typeof data !== "string") {
      res.status(400).json({ message: "Invalid query: 'data' must be string" });
      return;
    }

    // arbitrary limit
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

    global_state.gdl_connection.sendBroadcast(data);
    res.json({ message: "sent" });
  });

  return router;
};
