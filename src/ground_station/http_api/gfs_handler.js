const express = require("express");
const router = express.Router();
const parseGetQuery = require("./query_parse");
const path = require("path");

const genericResponse = require("./generic_response");

module.exports = function (global_state) {
  // /api/gfs/settings
  router.get("/settings", (req, res, next) => {
    parseGetQuery(req, res, "gfs", "settings", global_state);
  });

  router.put("/settings", (req, res, next) => {
    if (!req.query.hasOwnProperty("category")) {
      genericResponse(res, 400, "category is required.");
      return;
    }
    let category = req.query.category;
    if (!global_state.gfs_connection.doSettingsExist(category)) {
      genericResponse(res, 404, "Category not found.");
      return;
    }
    global_state.gfs_connection.updateSettings(category, req.body, res);
  });

  router.delete("/settings", (req, res, next) => {
    res.json({ message: "Not yet implemented!" });
  });

  // /api/gfs/data
  router.get("/data", (req, res, next) => {
    parseGetQuery(req, res, "gfs", "data", global_state);
  });

  // /api/gfs/images - get list of images
  router.get("/images", (req, res, next) => {
    res.json(global_state.gfs_connection.gfs_images.getImagesInfo());
  });

  // /api/gfs/images/sync - request an image to be synced from GFS
  router.put("/images/sync", (req, res, next) => {
    // parse the query
    if (!req.query.hasOwnProperty("filename")) {
      genericResponse(res, 400, "filename is required.");
      return;
    }

    let filename = req.query.filename;

    // ensure the path is valid/safe
    let base_name = path.basename(filename);
    if (base_name !== filename) {
      genericResponse(res, 400, "Invalid filename.");
      return;
    }
    let ext = path.extname(filename);
    const VALID_EXTENSIONS = [".jpg", ".jpeg", ".png"];
    if (!VALID_EXTENSIONS.includes(ext)) {
      genericResponse(res, 400, "Invalid file extension.");
      return;
    }

    global_state.gfs_connection.gfs_images.retrieveRemoteImage(
      filename,
      (success, message) => {
        if (success) {
          genericResponse(res, 200, message);
        } else {
          genericResponse(res, 500, message);
        }
      }
    );
  });

  return router;
};
