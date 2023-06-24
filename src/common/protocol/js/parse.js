/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   parse.js
 * @brief  For parsing giraffe protocol messages.
 * @addtogroup protocol
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-11
 * @copyright  2023 (license to be defined)
 */

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
    throw new Error(e);
  }
};
