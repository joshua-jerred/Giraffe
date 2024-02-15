const express = require("express");
const router = express.Router();
const parseGetQuery = require("./query_parse");
const genericResponse = require("./generic_response");

module.exports = function (global_state) {
  // /api/gfs/settings
  router.get("/settings", (req, res, next) => {
    parseGetQuery(req, res, "gfs", "settings", global_state);
  });

  router.put("/settings", (req, res, next) => {
    if (!req.query.hasOwnProperty("category")) {
      genericResponse(res, 400, "category is required.");
      return;
    }
    let category = req.query.category;
    if (!global_state.gfs_connection.doSettingsExist(category)) {
      genericResponse(res, 404, "Category not found.");
      return;
    }
    global_state.gfs_connection.updateSettings(category, req.body, res);
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
