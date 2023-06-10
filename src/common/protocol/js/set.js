const Message = require("./protocol");

class SetMessage extends Message {
  constructor(src, dst, resource, data) {
    if (typeof resource !== "string") {
      throw new Error(`Invalid resource: ${resource}`);
    }
    if (typeof data !== "object") {
      throw new Error(`Invalid data: ${data}`);
    }
    super(src, dst, "set", null, { rsc: resource, dat: data });
  }
}

module.exports = {
  SetMessage,
};
