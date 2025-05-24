const express = require("express");
const router = express.Router();
const parseGetQuery = require("./query_parse");
const gfsResources = require("../../../../project/metadata/gfs_resources.json");
const DataMeta = require("../../../../project/metadata/gfs_configuration.json");

module.exports = function (global_state) {
  const debug_data = {};

  for (let key in gfsResources) {
    debug_data[key] = global_state.getStreamData(key);
  }

  debug_data["settings"] = {};
  for (let category in DataMeta) {
    debug_data["settings"][category] = global_state.getSettings(category);
  }

  return debug_data;
};
