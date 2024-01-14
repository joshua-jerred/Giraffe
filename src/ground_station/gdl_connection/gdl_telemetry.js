var Deque = require("double-ended-queue");

module.exports = class GdlTelemetry {
  constructor(global_state) {
    this.global_state = global_state;

    this.aprs_position_queue = new Deque(100);
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
    this.aprs_position_queue.insertFront(packet);
  }

  getAprsPositionPacketArray() {
    return this.aprs_position_queue.toArray();
  }
};
