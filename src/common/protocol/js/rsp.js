const Message = require("./protocol");

const valid_response_codes = ["ok", "er", "un"];

class ResponseMessage extends Message {
  constructor(src, dst, id, response_code, data) {
    if (
      typeof response_code !== "string" ||
      !valid_response_codes.includes(response_code)
    ) {
      throw new Error(`Invalid response_code: ${response_code}`);
    }
    if (typeof data !== "object") {
      throw new Error(`Invalid data: ${data}`);
    }
    super(src, dst, "rsp", id, { cde: response_code, dat: data });
  }
}

module.exports = {
  ResponseMessage,
};
