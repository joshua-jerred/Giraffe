const net = require("net");
const { SetMessage } = require("giraffe-protocol");

module.exports = class GfsSocketRequester {
  constructor(global_state) {
    this.global_state = global_state;
  }

  /**
   * Sends a set request to GFS.
   *
   * @param {string} resource - rsc field of the set message, e.g. "setting/extensions"
   * @param {object} data - dat field of the set message, must be an object.
   * @param {function(success, error_code/response_object)} callback - called with the response from GFS.
   */
  sendSetRequest(resource, data, callback, timeout = 5000) {
    let msg = new SetMessage("ggs", "gfs", resource, data);
    let con = new net.Socket();
    let received_response = {};
    con.setTimeout(timeout);
    con.connect(
      this.global_state.gfs_connection.port,
      this.global_state.gfs_connection.address,
      function () {
        con.write(msg.getString());
      }
    );
    con.on("data", function (data) {
      received_response = JSON.parse(data);
      callback(true, received_response);
      con.destroy();
    });
    con.on("close", () => {
      // if (Object.keys(received_response).length === 0) {
      // callback(false, "No response from GFS");
      // }
    });
    con.on("timeout", function () {
      callback(false, "Timeout");
      con.destroy();
    });
    con.on("error", function (error) {
      callback(false, "Error: " + error);
      con.destroy();
    });
  }
};
