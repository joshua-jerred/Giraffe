const valid_sources = ["telemetry", "ggs", "client", "gfs"];
const valid_types = ["status", "path"];

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
    super("client", "path", { path: path });
  }
};

class StatusMessage extends Message {
  constructor(source, status) {
    super(source, "status", status );
  }
}

function parseMessage(json) {
  if (!valid_sources.includes(json["source"])) {
    throw new Error("Invalid source: " + json.source);
  }
  if (!valid_types.includes(json.type)) {
    throw new Error("Invalid type: " + json.type);
  }
  return new Message(json.source, json.type, json.body);
}

module.exports = {
  Message: Message,
  PathMessage: PathMessage,
  StatusMessage: StatusMessage,
  parseMessage: parseMessage,
}