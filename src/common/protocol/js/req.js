const Message = require("./protocol");

class RequestMessage extends Message {
  constructor(src, dst, resource) {
    if (typeof resource !== "string") {
      throw new Error(`Invalid resource: ${resource}`);
    }
    super(src, dst, "req", null, { rsc: resource });
  }
}

module.exports = {
  RequestMessage
};
