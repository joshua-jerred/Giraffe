const valid_src_dst = ["telemetry", "ggs", "client", "gfs"];
const valid_types = ["info", "req", "rsp"];
const valid_categories = ["ping", "data", "error"];

const GenId = require("./id");

module.exports = class Message {
  constructor(
    src = null,
    dst = null,
    typ = null,
    cat = null,
    id = null,
    body = {}
  ) {
    this.src = src;
    this.dst = dst;
    this.typ = typ; // [info, req, rsp]
    this.cat = cat; // [ping]
    this.id = id; // ID
    this.body = body; // Body JSON Object

    // Verify source validity
    if (!valid_src_dst.includes(this.src))
      throw new Error("Invalid source: " + this.src);

    // Verify destination validity
    if (!valid_src_dst.includes(this.dst) && this.dst !== null) {
      throw new Error("Invalid destination: " + this.dst);
    }

    // Verify type validity
    if (!valid_types.includes(this.typ) && this.typ !== null) {
      throw new Error("Invalid type: " + this.typ);
    }

    // Verify category validity
    if (!valid_categories.includes(this.cat) && this.cat !== null) {
      throw new Error("Invalid category: " + this.cat);
    }

    // Verify id validity
    if (typeof this.id !== "string" && this.id !== null) {
      this.id = GenId();
    }

    // Verify body validity
    if (typeof this.body !== "object" && this.body !== null) {
      throw new Error("Invalid body: " + this.body);
    }
  }

  getJson() {
    return { 
      src: this.src, 
      dst: this.dst,
      typ: this.typ, 
      cat: this.cat,
      id: this.id,
      body: this.body 
    };
  }

  getString() {
    return JSON.stringify(this.getJson());
  }

  toString() {
      return JSON.stringify(this.getJson());
  }
}

// class DataStreamRequestMessage extends Message {
//   // stream, rat in ms
//   constructor(stream, rate) {
//     if (!Array.isArray(data_stream_requests)) {
//       throw new Error("Data stream requests must be an array");
//     }
//     for (let i = 0; i < data_stream_requests.length; i++) {
//       if (!valid_data_streams.includes(data_stream_requests[i])) {
//         throw new Error(
//           "Invalid data stream request: " + data_stream_requests[i]
//         );
//       }
//     }
//     super("client", "data_stream_request", data_stream_requests);
//   }
// }