const Ajv = require("ajv");
const schema = require("./schema");

const ajv = exports.ajv = new Ajv();

ajv.addSchema(schema.base_message_schema, "base_message");
ajv.addSchema(schema.ping_schema, "ping");
ajv.addSchema(schema.data_request_schema, "data_request");
ajv.addSchema(schema.data_response_schema, "data_response");