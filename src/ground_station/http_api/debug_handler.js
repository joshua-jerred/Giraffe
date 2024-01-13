const express = require("express");
const router = express.Router();
const parseGetQuery = require("./query_parse");
const gfsResources = require("../../../project/metadata/gfs_resources.json");

module.exports = function (global_state) {
  const debug_data = {};

  for (let key in gfsResources) {
    debug_data[key] = global_state.getStreamData(key);
  }

  return debug_data;
};
