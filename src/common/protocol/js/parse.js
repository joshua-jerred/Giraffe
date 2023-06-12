const Message = require("./protocol");

module.exports = function (message) {
  if (typeof message === "string") {
    try {
      message = JSON.parse(message);
    } catch (e) {
      throw new Error("Invalid JSON string: " + message + " " + e.message);
    }
  } else if (typeof message !== "object") {
    throw new Error("Invalid message: " + message);
  }

  try {
    let msg = new Message(
      message.src,
      message.dst,
      message.typ,
      message.id,
      message.bdy
    );
    return msg;
  } catch (e) {
    throw new Error("Couldn't parse the message: " + e);
  }
};
