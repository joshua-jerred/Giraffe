const express = require("express");
const router = express.Router();

module.exports = function (global_state) {
  // gfs/*
  router.get("/", (req, res, next) => {
    res.json({ message: "hello world, gfs!" });
  });

  return router;
};