const express = require("express");
const router = express.Router();
const parseGetQuery = require("./query_parse");

module.exports = function (global_state) {
  // /api/gfs/settings
  router.get("/settings", (req, res, next) => {
    parseGetQuery(req, res, "gfs", "settings", global_state);
  });

  router.put("/settings", (req, res, next) => {
    res.json({ message: "Not yet implemented!" });
  });

  router.delete("/settings", (req, res, next) => {
    res.json({ message: "Not yet implemented!" });
  });

  // /api/gfs/data
  router.get("/data", (req, res, next) => {
    parseGetQuery(req, res, "gfs", "data", global_state);
  });

  return router;
};
