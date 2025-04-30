const express = require("express");
const router = express.Router();
const genericResponse = require("./generic_response");

module.exports = function (global_state) {
  // POST /api/command
  router.post("/", (req, res, next) => {
    let command_string = "";
    let send_method = "";
    try {
      command_string = req.body.command_string;
      send_method = req.body.send_method;
    } catch (err) {
      genericResponse(res, 400, "Invalid request body.");
      return;
    }

    const errorResponse = (message) => {
      global_state.error(
        `command_handler [${command_string}, ${send_method}]: ${message}`
      );
      genericResponse(res, 400, message);
      return;
    };

    const valid_send_methods = ["tcp_socket", "gdl_broadcast", "gdl_exchange"];
    if (!valid_send_methods.includes(send_method)) {
      errorResponse("Invalid send method");
      return;
    }

    // command format example: "cmd/dat/cae/"
    if (
      typeof command_string !== "string" ||
      command_string.length < 12 ||
      command_string.slice(0, 4) !== "cmd/" ||
      command_string[7] !== "/" ||
      command_string[11] !== "/"
    ) {
      errorResponse("Invalid command string");
      return;
    }

    // -- Command Method: TCP Socket
    let gfs_status = global_state.ggs_status.gfs;
    if (send_method === "tcp_socket") {
      if (gfs_status !== "connected") {
        errorResponse("GFS tcp status " + gfs_status);
        return;
      }
      global_state.gfs_connection.sendCommand(command_string, (response) => {
        genericResponse(res, 200, response);
        global_state.info(
          `command_handler [${command_string}, ${send_method}]: SUCCESS : ${response.message}`
        );
      });
      return;
    }

    // -- Command Method: GDL Broadcast/Exchange
    let gdl_status = global_state.ggs_status.gdl;
    if (send_method === "gdl_broadcast" || send_method === "gdl_exchange") {
      if (gdl_status !== "connected") {
        errorResponse("GDL status: " + gdl_status);
        return;
      } else if (send_method === "gdl_broadcast") {
        if (global_state.gdl_connection.sendBroadcast(command_string)) {
          errorResponse("Broadcast sent.");
          return;
        }
        errorResponse("Error sending broadcast.");
        return;
      } else if (send_method === "gdl_exchange") {
        if (global_state.gdl_connection.sendExchange(command_string)) {
          genericResponse(res, 200, "Exchange sent.");
          return;
        }
        errorResponse("Error sending exchange.");
        return;
      }
    }

    errorResponse("Internal logic error.");
    return;
  });

  return router;
};
