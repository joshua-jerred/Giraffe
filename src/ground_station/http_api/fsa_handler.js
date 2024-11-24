const express = require("express");
const router = express.Router();

function wantsMetadata(req) {
  if (req.query.hasOwnProperty("include") && req.query.include === "values") {
    return false;
  }
  return true;
}

function wantsValues(req) {
  if (req.query.hasOwnProperty("include") && req.query.include === "metadata") {
    return false;
  }
  return true;
}

function getCategory(req) {
  if (req.query.hasOwnProperty("category")) {
    return req.query.category;
  }
  return null;
}

module.exports = function (global_state) {
  // GET /api/fsa/data
  router.get("/data", (req, res, next) => {
    let category = getCategory(req);
    if (category === null) {
      res.status(400).json({ error: "Category is required" });
      return;
    }

    global_state.fsa_connection.getDataForResource(
      category,
      res,
      wantsValues(req),
      wantsMetadata(req)
    );
  });

  // GET /api/fsa/settings
  router.get("/settings", (req, res, next) => {
    global_state.fsa_connection.getDataForResource(
      "settings",
      res,
      wantsValues(req),
      wantsMetadata(req)
    );
  });

  // PUT /api/fsa/settings
  router.put("/settings", (req, res, next) => {
    global_state.fsa_connection.updateConfig(req.body, res);
  });

  return router;
};
