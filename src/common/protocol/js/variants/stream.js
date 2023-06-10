const { RequestMessage } = require("../req");
const { ResponseMessage } = require("../rsp");

const valid_actions = ["add", "remove", "clear_all", "get"];
const valid_streams = ["status", "critical", "position_gps", "position_imu", "gfs_status"];

class StreamRequest extends RequestMessage {
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

    let resource = "stream/" + action + "/" + name;

    super(src, dst, "req", null, resource);
  }
}

class StreamResponse extends ResponseMessage {
  constructor(src, dst, stream_name, data) {
    if (!valid_streams.includes(stream_name)) {
      throw new Error(`Invalid stream: ${stream_name}`);
    }

    if (typeof data !== "object") {
      throw new Error(`Invalid data: ${data}`);
    }

    let body = { stream: stream_name, data: data };

    super(src, dst, null, "ok", body);
  }
}

module.exports = {
  StreamRequest,
  StreamResponse,
};
