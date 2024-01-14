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
    global_state.gdl_telemetry.handleRequest(req, res);
  });

  return router;
};
