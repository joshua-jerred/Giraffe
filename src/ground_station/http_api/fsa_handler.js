const express = require("express");
const router = express.Router();

module.exports = function (global_state) {
  // GET /api/fsa/status
  router.get("/status", (req, res, next) => {
    global_state.fsa_connection.getDataForResource("status", res);
  });

  // GET /api/fsa/settings
  router.get("/settings", (req, res, next) => {
    global_state.fsa_connection.getDataForResource("settings", res);
  });

  // PUT /api/fsa/settings
  router.put("/settings", (req, res, next) => {
    global_state.fsa_connection.updateConfig(req.body, res);
  });

  return router;
};
