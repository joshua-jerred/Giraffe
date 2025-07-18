const express = require("express");
const router = express.Router();
const parseGetQuery = require("./query_parse");
const path = require("path");

const genericResponse = require("./generic_response");
const { SetMessage } = require("giraffe-protocol");

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

  // POST /api/gfs/extensions - create a new extension
  router.post("/extensions", (req, res, next) => {
    const resource = "setting/extensions";
    const data = {
      ext_data: req.body,
      action: "add",
    };
    global_state.gfs_socket_requester.sendSetRequest(
      resource,
      data,
      (success, response) => {
        if (
          success &&
          response.hasOwnProperty("bdy") &&
          response.bdy.hasOwnProperty("cde") &&
          response.bdy.cde === "ok"
        ) {
          res.json({ message: "Extension added." });
        } else {
          res.status(500).json(response);
        }
      }
    );
  });

  // PUT /api/gfs/extensions - update an extension
  router.put("/extensions", (req, res, next) => {
    if (!req.query.hasOwnProperty("extension_name")) {
      genericResponse(res, 400, "extension_name is required in query.");
      return;
    }

    // the current name of the extension, it may be updated in the body
    let extension_name = req.query.extension_name;

    const resource = "setting/extensions";
    const data = {
      ext_name: extension_name,
      ext_data: req.body,
      action: "update",
    };
    global_state.gfs_socket_requester.sendSetRequest(
      resource,
      data,
      (success, response) => {
        if (
          success &&
          response.hasOwnProperty("bdy") &&
          response.bdy.hasOwnProperty("cde") &&
          response.bdy.cde === "ok"
        ) {
          res.json({ message: "Extension updated." });
        } else {
          res.status(500).json(response);
        }
      }
    );
  });

  // DELETE /api/gfs/extensions - delete an extension
  router.delete("/extensions", (req, res, next) => {
    if (!req.query.hasOwnProperty("extension_name")) {
      genericResponse(res, 400, "extension_name is required in query.");
      return;
    }

    // the current name of the extension, it may be updated in the body
    let extension_name = req.query.extension_name;

    const resource = "setting/extensions";
    const data = {
      ext_name: extension_name,
      action: "remove",
    };
    global_state.gfs_socket_requester.sendSetRequest(
      resource,
      data,
      (success, response) => {
        if (
          success &&
          response.hasOwnProperty("bdy") &&
          response.bdy.hasOwnProperty("cde") &&
          response.bdy.cde === "ok"
        ) {
          res.json({ message: "Extension deleted." });
        } else {
          res.status(500).json(response);
        }
      }
    );
  });

  // -- ADC Value Mapping Handlers --

  // POST /api/gfs/adc_mappings - create a new ADC value map
  router.post("/adc_mappings", (req, res, next) => {
    const resource = "setting/adc_mappings";
    const data = {
      map_data: req.body,
      action: "add",
    };
    global_state.gfs_socket_requester.sendSetRequest(
      resource,
      data,
      (success, response) => {
        if (
          success &&
          response.hasOwnProperty("bdy") &&
          response.bdy.hasOwnProperty("cde") &&
          response.bdy.cde === "ok"
        ) {
          res.json({ message: "ADC Value Map added." });
        } else {
          res.status(500).json(response);
        }
      }
    );
  });

  // PUT /api/gfs/adc_mappings - update an ADC value map
  router.put("/adc_mappings", (req, res, next) => {
    if (!req.query.hasOwnProperty("map_label")) {
      genericResponse(res, 400, "map_label is required in query.");
      return;
    }

    // the current label of the map, it may be updated in the body
    let map_label = req.query.map_label;

    const resource = "setting/adc_mappings";
    const data = {
      map_label: map_label,
      map_data: req.body,
      action: "update",
    };
    global_state.gfs_socket_requester.sendSetRequest(
      resource,
      data,
      (success, response) => {
        if (
          success &&
          response.hasOwnProperty("bdy") &&
          response.bdy.hasOwnProperty("cde") &&
          response.bdy.cde === "ok"
        ) {
          res.json({ message: "ADC value map updated." });
        } else {
          res.status(500).json(response);
        }
      }
    );
  });

  // DELETE /api/gfs/adc_mappings - delete an ADC value map
  router.delete("/adc_mappings", (req, res, next) => {
    if (!req.query.hasOwnProperty("map_label")) {
      genericResponse(res, 400, "map_label is required in query.");
      return;
    }

    // the current label of the value map, it may be updated in the body
    let map_label = req.query.map_label;

    const resource = "setting/adc_mappings";
    const data = {
      map_label: map_label,
      action: "remove",
    };
    global_state.gfs_socket_requester.sendSetRequest(
      resource,
      data,
      (success, response) => {
        if (
          success &&
          response.hasOwnProperty("bdy") &&
          response.bdy.hasOwnProperty("cde") &&
          response.bdy.cde === "ok"
        ) {
          res.json({ message: "ADC value map deleted." });
        } else {
          res.status(500).json(response);
        }
      }
    );
  });

  return router;
};
