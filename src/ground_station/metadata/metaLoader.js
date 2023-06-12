const ggs_meta_data = require("./ggs_meta.json");
const gfs_data_meta = require("../../common/metadata/gfs_resources.json");
const gfs_configuration_meta = require("../../common/metadata/gfs_configuration.json")

function loadMetaData(api, resource = null, category = null) {
  var meta_data = {};
  if (api === "gfs") {
    meta_data["settings"] = gfs_configuration_meta;
    meta_data["data"] = gfs_data_meta;
  } else if (api === "ggs") {
    meta_data = ggs_meta_data;
  } else {
    throw new Error("invalid api");
  }

  if (resource === null) {
    return meta_data;
  } else if (meta_data[resource] === undefined) {
    return null;
  } else if (category === null) {
    return meta_data[resource];
  } else if (meta_data[resource][category] === undefined) {
    return null;
  } else {
    return meta_data[resource][category];
  }
}

module.exports = loadMetaData;
