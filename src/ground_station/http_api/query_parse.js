const loadMetaData = require("../metadata/metaLoader");
const genericResponse = require("./generic_response");

module.exports = function parseGetQuery(req, res, api, resource, global_state) {
  const valid_include = ["all", "values", "metadata"];

  // Required query parameter 'category'
  if (!req.query.hasOwnProperty("category")) {
    genericResponse(res, 400, "category is required.");
    return;
  }
  let category = req.query.category;

  // Optional query parameter 'include', defaults to 'all'
  let include_values = true;
  let include_metadata = true;
  if (
    req.query.hasOwnProperty("include") &&
    !valid_include.includes(req.query.include)
  ) {
    genericResponse(
      res,
      400,
      "include parameter must be one of these values: all, values, metadata"
    );
    return;
  } else if (req.query.hasOwnProperty("include")) {
    if (req.query.include === "values") {
      include_metadata = false;
    } else if (req.query.include === "metadata") {
      include_values = false;
    }
  }

  let response_body = {};

  // Load the metadata
  if (include_metadata) {
    metadata = loadMetaData(api, resource, category);
    if (metadata === null) {
      genericResponse(res, 404, "category metadata not found.");
      return;
    }
    response_body["metadata"] = metadata;
  }

  if (include_values) {
    if (api === "gfs") {
      if (resource === "data") {
        values = global_state.gfs_connection.getData(category);
        if (include_metadata) {
          response_body["metadata"]["MS_SINCE_LAST_UPDATE"] =
            global_state.gfs_connection.getMsSinceLastUpdate(category);
        }
      } else if (resource === "settings") {
        values = global_state.gfs_connection.getSettings(category);
      }
    } else if (api === "ggs") {
      if (resource === "settings") {
        values = global_state.ggs_db.get(resource, category);
      } else {
        if (category === "status") {
          values = global_state.status;
        }
      }
    }

    if (values === null) {
      genericResponse(res, 404, "category values not found.");
      return;
    }
    response_body["values"] = values;
  }

  res.json(response_body);
};
