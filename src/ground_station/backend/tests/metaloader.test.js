const test = require("ava");
const loadMetaData = require("../metadata/metaLoader");
const Ajv = require("ajv");
const ajv = new Ajv();

const setting_schema = {
  type: "object",
  properties: {
    type: {
      enum: ["string", "int", "float", "bool", "enum"],
    },
    name: {
      type: "string",
      minLength: 4,
      maxLength: 35,
    },
    description: {
      type: "string",
      minLength: 10,
      maxLength: 300,
    },
    default: {},
  },
  required: ["type", "name", "description", "default"],
  additionalProperties: true,
};

test("GGS Settings Metadata", (t) => {
  const settings = loadMetaData("ggs", "settings");
  for (var i in settings) {
    for (var j in settings[i]) {
      const valid = ajv.validate(setting_schema, settings[i][j]);
      if (!valid) {
        if (ajv.errors) {
          console.log(ajv.errors);
        }
        t.fail();
      }
    }
  }
  t.pass();
});
