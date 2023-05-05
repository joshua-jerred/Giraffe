const Message = require("../message");

module.exports = class ErrorMessage extends Message {
  constructor(src, dst, error_message, id=null) {
    super(src, dst, "rsp", "error", id, { message: error_message});
  }
};