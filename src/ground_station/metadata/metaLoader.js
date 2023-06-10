const gfs_meta_data = require("./gfs_meta.json");
const ggs_meta_data = require("./ggs_meta.json");

function loadMetaData(api, resource = null, category = null) {
  var meta_data = null;
  if (api === "gfs") {
    meta_data = gfs_meta_data;
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
