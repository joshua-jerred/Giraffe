const express = require("express");
const router = express.Router();

const ggsHandler = require("./ggs_handler");
const gfsHandler = require("./gfs_handler");
//const gfsHandler = require("./gfs");

module.exports = function (global_state) {
  // API /api/ggs/*
  router.use("/ggs", ggsHandler(global_state));

  // API /api/gfs/*
  router.use("/gfs", gfsHandler(global_state));

  // API Endpoint - GET /api/status
  router.get("/status", (req, res) => {
    res.json(global_state.getStatus());
  });

  return router;
};
