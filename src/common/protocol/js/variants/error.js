const Message = require("../protocol.js");

module.exports = class ErrorMessage extends Message {
  constructor(src, dst, error_message) {
    if (typeof error_message !== "string") {
      throw new Error(`Invalid error_code: ${error_message}`);
    }

    let data = { error: error_message };

    super(src, dst, "rsp", null, { cde: "er", dat: data });
  }
}