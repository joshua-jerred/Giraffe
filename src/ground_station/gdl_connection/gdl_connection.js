const net = require("net");
const { RequestMessage, parse } = require("giraffe-protocol");

module.exports = class Telemetry {
  constructor(global_state) {
    this.global_state = global_state;
    this.gdl_connected = false;
  }

  async update() {
    this.gdl_connected = true;

    this.#requestData("oof");
  }

  #requestData(category) {
    let request = new RequestMessage("ggs", "gdl", category);
    let con = new net.Socket();
    let that = this;

    const TIMEOUT = 1000;
    con.setTimeout(TIMEOUT);

    con.connect(9557, "localhost", function () {
      con.write(JSON.stringify(request));
    });

    con.on("data", function (data) {
      that.connected = true;
      try {
        let msg = parse(data.toString());
        console.log("Response: " + msg);
        if (msg.bdy.cde !== "ok") {
          console.log("Error: " + msg.bdy.dat);
          return;
        }
        // that.#setLocalResource(category, msg);
      } catch (e) {
        console.log("Error parsing data for category:" + category);
      }
      con.destroy();
    });

    con.on("close", function () {});

    con.on("error", function (err) {
      that.connected = false;
      /// @todo Need to figure out why we are getting here so often. Most likely on the C++ side.
      console.log(
        "Socket Error during the category: " + category + " - " + err
      );
    });
  }

  get status() {
    return this.gdl_connected ? "connected" : "disconnected";
  }
};
