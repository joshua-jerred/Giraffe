const valid_sources = ["telemetry", "ggs", "client", "gfs"];
const valid_types = ["status", "path", "data_stream_request", "clear_streams", "data"];

const valid_data_streams = ["ggs_stats"];

class Message {
  constructor(source = null, type = null, body = {}) {
    this.source = "";
    this.type = "";
    this.body = body;

    if (!valid_sources.includes(source) && source !== null) {
      throw new Error("Invalid source: " + source);
    }
    this.source = source;

    if (!valid_types.includes(type) && type !== null) {
      throw new Error("Invalid type: " + type);
    }
    this.type = type;
  }

  json() {
    return { source: this.source, type: this.type, body: this.body };
  }

  string() {
    return JSON.stringify(this.json());
  }

  toString() {
    return "Source: " + this.source + " Type: " + this.type + "\n  >Body: " + JSON.stringify(this.body, null, 2);
  }
};

class PathMessage extends Message {
  constructor(path) {
    super("client", "path", path);
  }
};

class StatusMessage extends Message {
  constructor(source, status) {
    super(source, "status", status );
  }
}

class DataStreamRequestMessage extends Message {
  // stream, rat in ms
  constructor(stream, rate) {
    if (!Array.isArray(data_stream_requests)) {
      throw new Error("Data stream requests must be an array");
    }
    for (let i = 0; i < data_stream_requests.length; i++) {
      if (!valid_data_streams.includes(data_stream_requests[i])) {
        throw new Error("Invalid data stream request: " + data_stream_requests[i]);
      }
    }
    super("client", "data_stream_request", data_stream_requests);
  }
}

class DataMessage extends Message {
  constructor(stream_name, data) {
    let body = {
      stream_name: stream_name,
      data: data
    }
    super("ggs", "data", body);
  }
}

function parseMessage(json) {
  if (!valid_sources.includes(json["source"])) {
    throw new Error("Invalid source: " + json.source);
  }
  if (!valid_types.includes(json.type)) {
    throw new Error("Invalid type: " + json.type);
  }

  if (json.type == "data_stream_request") {
    // Verify it's a string
    if (typeof json.body !== "string") {
      throw new Error("Invalid data stream request: " + json.body);
    }

    // Verify it's a valid data stream request
    if (!valid_data_streams.includes(json.body)) {
      throw new Error("Invalid data stream request: " + json.body);
    }
  }
  return new Message(json.source, json.type, json.body);
}

module.exports = {
  Message: Message,
  PathMessage: PathMessage,
  StatusMessage: StatusMessage,
  DataStreamRequestMessage: DataStreamRequestMessage,
  DataMessage: DataMessage,
  parseMessage: parseMessage,
}