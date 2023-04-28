const gfs_meta_data = require("./gfs_meta.json");
const ggs_meta_data = require("./ggs_meta.json");

function loadMetaData(api, category = null, subcategory = null, key = null) {
  var meta_data = null;
  if (api === "gfs") {
    meta_data = gfs_meta_data;
  } else if (api === "ggs") {
    meta_data = ggs_meta_data;
  } else {
    throw new Error("invalid api");
  }

  if (meta_data === null || meta_data[category] === undefined) {
    return null;
  } else if (subcategory === null) {
    return meta_data[category];
  } else if (meta_data[category][subcategory] === undefined) {
    return null;
  } else if (key === null) {
    return meta_data[category][subcategory];
  } else {
    return meta_data[category][subcategory][key];
  }
}

module.exports = loadMetaData;
