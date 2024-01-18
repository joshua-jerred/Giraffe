var Deque = require("double-ended-queue");
const { Point } = require("@influxdata/influxdb-client");

module.exports = class GdlTelemetry {
  constructor(global_state) {
    this.global_state = global_state;

    this.aprs_position_queue = new Deque(100);

    this.have_initial_aprs_position = false;
  }

  handleRequest(req, res) {
    if (!req.query.hasOwnProperty("category")) {
      genericResponse(res, 400, "category is required.");
      return;
    }
    let category = req.query.category;

    if (category === "all_aprs_positions") {
      res.json(this.getAprsPositionPacketArray());
    } else if (category === "last_aprs_position") {
      res.json(this.aprs_position_queue.peekFront());
    } else {
      genericResponse(res, 400, "category not found.");
    }
  }

  addAprsPositionPacket(packet) {
    try {
      if (this.global_state.influx_enabled) {
        const point = new Point("aprs_position")
          .tag("dst-ssid", packet.dst + "-" + packet.dst_ssid)
          .tag("src-ssid", packet.src + "-" + packet.src_ssid)
          .stringField("gps_time", packet.gps_time)
          .stringField("utc_timestamp", packet.utc_timestamp)
          .floatField("latitude", packet.latitude)
          .floatField("longitude", packet.longitude)
          .floatField("altitude", packet.altitude)
          .floatField("course", packet.course)
          .floatField("speed", packet.speed);
        this.global_state.influx_writer.write(point);
      }
    } catch (error) {
      console.log(error);
    }
    this.have_initial_aprs_position = true;
    this.aprs_position_queue.insertFront(packet);
  }

  getAprsPositionPacketArray() {
    return this.aprs_position_queue.toArray();
  }

  getMostRecentAprsPositionPacket() {
    if (!this.have_initial_aprs_position) {
      return null;
    }
    return this.aprs_position_queue.peekFront();
  }
};
