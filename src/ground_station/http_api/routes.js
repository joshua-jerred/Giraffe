const express = require("express");
const router = express.Router();

const ggsHandler = require("./ggs_handler");
const gfsHandler = require("./gfs_handler");
const gdlHandler = require("./gdl_handler");
const debugHandler = require("./debug_handler");

module.exports = function (global_state) {
  // API /api/ggs/*
  router.use("/ggs", ggsHandler(global_state));

  // API /api/gfs/*
  router.use("/gfs", gfsHandler(global_state));

  // API /api/gdl/*
  router.use("/gdl", gdlHandler(global_state));

  // API Endpoint - GET /api/status
  router.get("/status", (req, res) => {
    res.json(global_state.getStatus());
  });

  // API Endpoint - GET /api/debug
  router.get("/debug", (req, res) => {
    res.json(debugHandler(global_state));
  });

  return router;
};
