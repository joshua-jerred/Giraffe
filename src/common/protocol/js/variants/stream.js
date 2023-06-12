const Message = require("../protocol");

// add: add a stream
// remove: remove a stream
// remove_all: remove all streams
// get: get stream data one time
const valid_actions = ["add", "remove", "remove_all", "get"];

/**
 * @brief For requesting a stream action: add, remove, remove_all.
 *
 * @details If it's remove_all, the stream name is ignored.
 *
 * @param src - source endpoint
 * @param dst - destination endpoint
 * @param action - action to perform (add, remove, remove_all)
 * @param name - name of stream to perform action on
 */
class StreamRequest extends Message {
  constructor(src, dst, action, name) {
    if (!valid_actions.includes(action)) {
      throw new Error(`Invalid action: ${action}`);
    }

    const typ = "req"; // request type (must have "rsc" field)
    const body = { rsc: "stream", action: action, stream: name };
    super(src, dst, typ, null, body); // id is generated
  }
}

/**
 * @brief For responding to a stream request.
 *
 * @details if is_valid is false, the data and stream_name are included, but
 * really don't matter.
 *
 * @param src - source endpoint
 * @param dst - destination endpoint
 * @param stream_name - name of stream
 * @param data - data to send
 * @param is_valid - whether the data is valid (default: true)
 */
class StreamResponse extends Message {
  constructor(src, dst, stream_name, data, is_valid = true) {
    if (typeof data !== "object") {
      throw new Error(`Invalid data: ${data}`);
    }

    const typ = "rsp"; // response type (must have "cde" field)
    const cde = is_valid ? "ok" : "er";
    const body = { cde: cde, stream: stream_name, data: data };

    super(src, dst, typ, null, body); // id is doesn't matter, but is generated
  }
}

module.exports = {
  StreamRequest,
  StreamResponse,
};
