const Message = require("../message");

class PingRequest extends Message {
  constructor(src, dst) {
    super(src, dst, "req", "ping", null, {});
  }
};


class PingResponse extends Message {
  constructor(src, dst, request_id) {
    super(src, dst, "rsp", "ping", request_id, {});
  }
}

module.exports = {
  PingRequest,
  PingResponse,
};