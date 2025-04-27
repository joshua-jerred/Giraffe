const ggs_meta_data = require("../../../../project/metadata/ggs_meta.json");
const gfs_data_meta = require("../../../../project/metadata/gfs_resources.json");
const gdl_meta_data = require("../../../../project/metadata/gdl_resources.json");
const gfs_configuration_meta = require("../../../../project/metadata/gfs_configuration.json");
const fsa_meta_data = require("../../../../project/metadata/fsa_metadata.json");

for (let category in gfs_configuration_meta) {
  for (let data_item in gfs_configuration_meta[category]) {
    if (data_item === "SECTION_TYPE") {
      delete gfs_configuration_meta[category][data_item];
    }
  }
}

function loadMetaData(api, resource = null, category = null) {
  var meta_data = {};
  if (api === "gfs") {
    meta_data["settings"] = gfs_configuration_meta;
    meta_data["data"] = gfs_data_meta;
  } else if (api === "ggs") {
    meta_data = ggs_meta_data;
  } else if (api === "gdl") {
    meta_data = gdl_meta_data;
  } else if (api === "fsa") {
    meta_data = fsa_meta_data;
  } else {
    throw new Error("invalid api");
  }

  // console.log("api: " + api, "resource: " + resource, "category: " + category);
  // console.log(meta_data);

  if (resource === null) {
    return meta_data;
  } else if (meta_data[resource] === undefined) {
    return {};
  } else if (category === null) {
    return meta_data[resource];
  } else if (meta_data[resource][category] === undefined) {
    return {};
  } else {
    return meta_data[resource][category];
  }
}

module.exports = loadMetaData;
