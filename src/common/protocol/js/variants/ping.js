const Message = require("../protocol");
const { RequestMessage } = require("../req");
const { ResponseMessage } = require("../rsp");

class PingRequest extends RequestMessage {
  constructor(src, dst) {
    super(src, dst, "ping");
  }
}

class PingResponse extends ResponseMessage {
  constructor(src, dst, request_id) {
    super(src, dst, request_id, "ok", { "": "" });
  }
}

module.exports = {
  PingRequest,
  PingResponse,
};
