const errorResponse = require("./error_response");

const valid_resources = ["status", "static", "ggs", "gfs"];

const valid_ggs_categories = ["settings"];
const valid_gfs_categories = ["settings"];

var verifyPathSchema = function (req, res, next) {
  let path_parts = req.path.split("/");

  // All API endpoints must start with /api
  if (path_parts.length < 3 || path_parts[1] != "api") {
    errorResponse(res, 404, "Invalid API endpoint.");
    return;
    // Verify that the resource is valid
  } else if (!valid_resources.includes(path_parts[2])) {
    errorResponse(res, 404, "Invalid API resource.");
    return;
    // If it's 'static' or 'status', no need to check further
  } else if (path_parts[2] == "static" || path_parts[2] == "status") {
    next();
    return;
  } else if (path_parts.length < 4) {
    errorResponse(res, 404, "Invalid API endpoint.");
    return;
  }

  let resource = path_parts[2];
  let category = path_parts[3];
  // Verify that the category is valid for the resource
  if (resource == "ggs" && !valid_ggs_categories.includes(category)) {
    errorResponse(res, 404, "Invalid API category.");
  } else if (resource == "gfs" && !valid_gfs_categories.includes(category)) {
    errorResponse(res, 404, "Invalid API category.");
  } else {
    next();
  }
};

function verifySettingsSchema(req, res, next) {
  
  if (req.method == "GET") {


    
  } else if (req.method == "POST") {
  } else {
    errorResponse(res, 404, "Invalid API method.");
    return;
  }
  next();
}

module.exports = {
  verifyPathSchema: verifyPathSchema,
};
