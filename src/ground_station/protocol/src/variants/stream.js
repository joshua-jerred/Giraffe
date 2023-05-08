const Message = require("../message");

const valid_actions = ["add", "remove", "clear_all", "get"];
const valid_streams = ["status", "critical", "position_gps", "position_imu", "gfs_status"];

class StreamRequest extends Message {
  constructor(src, dst, action, name) {
    if (!valid_actions.includes(action)) {
      throw new Error(`Invalid action: ${action}`);
    }

    if (
      (action === "add" || action === "remove") &&
      !valid_streams.includes(name)
    ) {
      throw new Error(`Invalid stream: ${name}`);
    }

    super(src, dst, "req", "stream", null, {
      action: action,
      stream: name,
    });
  }
}

class StreamResponse extends Message {
  constructor(src, dst, stream_name, data) {
    if (!valid_streams.includes(stream_name)) {
      throw new Error(`Invalid stream: ${stream_name}`);
    }

    if (typeof data !== "object") {
      throw new Error(`Invalid data: ${data}`);
    }

    super(src, dst, "rsp", "stream", stream_name, data);
  }
}

module.exports = {
  StreamRequest,
  StreamResponse,
};
