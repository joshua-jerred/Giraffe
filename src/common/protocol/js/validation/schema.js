const valid_src_dst = ["client", "telemetry", "ggs", "gfs"];
const valid_typ = ["info", "req", "rsp"];
const valid_cat = ["ping", "data"];

const base_message_schema = {
  type: "object",
  properties: {
    src: { type: "string", enum: valid_src_dst },
    dst: { type: "string", enum: valid_src_dst },
    typ: { type: "string", enum: valid_typ },
    cat: { type: "string", enum: valid_cat },
    id: { type: "string" },
    body: { type: "object" },
  },
  required: ["src", "dst", "typ", "cat", "id", "body"],
  additionalProperties: false,
};

let _ping = structuredClone(base_message_schema);
_ping.properties.typ.enum = ["req", "rsp"];
_ping.properties.cat.enum = ["ping"];

let _data_request = structuredClone(base_message_schema);
_data_request.properties.typ.enum = ["req"];
_data_request.properties.cat.enum = ["data"];
_data_request.properties.body = {
  type: "object",
  properties: {
    "params": { type: "object" },
  },
  required: ["params"],
  additionalProperties: false,
};

let _data_response = structuredClone(_data_request);
_data_response.properties.typ.enum = ["rsp"];
_data_response.properties.body = {
  type: "object",
  properties: {
    "params": { type: "object" },
    "data": { type: "object" },
  },
  required: ["params", "data"],
  additionalProperties: false,
};

const data_request_schema = _data_request;
const data_response_schema = _data_response;


const ping_schema = _ping;
module.exports = {
  valid_src_dst,
  valid_typ,
  valid_cat,
  base_message_schema,
  ping_schema,
  data_request_schema,
  data_response_schema
}