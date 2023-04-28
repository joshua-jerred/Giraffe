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
      maxLength: 30,
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

test("verify_settings", (t) => {
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

const data_schema = {
  type: "object",
  properties: {
    name: {
      type: "string",
    },
    description: {
      type: "string",
    },
    initial_value: {},
    units: {
      type: "string",
    },
  },
  required: ["name", "description", "initial_value"],
  additionalProperties: false,
};
test("verify_data", async (t) => {
  const data = loadMetaData("ggs", "data");
  for (var i in data) {
    for (var j in data[i]) {
      const valid = ajv.validate(data_schema, data[i][j]);
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
