const Message = require("../message");

class DataRequest extends Message {
  constructor(src, dst, parameters) {
    if (typeof parameters !== "object") {
      throw new Error(`Invalid parameters: ${parameters}`);
    }

    super(src, dst, "req", "data", null, { params: parameters });
  }
}

class DataResponse extends Message {
  constructor(src, dst, id, data, parameters={}) {
    if (typeof parameters !== "object") {
      throw new Error(`Invalid parameters: ${parameters}`);
    }

    if (typeof data !== "object") {
      throw new Error(`Invalid data: ${data}`);
    }

    super(src, dst, "rsp", "data", id, { data: data });
  }
}

module.exports = {
  DataRequest,
  DataResponse,
};
