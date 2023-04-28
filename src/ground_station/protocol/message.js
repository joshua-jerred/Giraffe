const valid_sources = ["telemetry", "ggs", "client", "gfs"];
const valid_types = [
  "meta",
  "save",
  "ack",
  "save_ack",
  "nack",
  "status",
  "alert",
  "data",
  "delete",
  "error",
  "command",
  "test",
  "path",
];

class Message {
  constructor(source, type, body) {
    this.source = source;
    this.type = type;
    this.body = body;
  }

  validate() {
    if (!valid_sources.includes(this.source)) {
      return false;
    }
    if (!valid_types.includes(this.type)) {
      return false;
    }
    if (!isObject(this.body)) {
      return false;
    }
    return true;
  }

  add_path(api, category, item) {
    this.body.api = api;
    this.body.category = category;
    this.body.item = item;
  }
}

function create_request_meta(source, api, category, subcategory) {
  return {
    source: source,
    type: "meta",
    body: {
      api: api,
      category: category,
      subcategory: subcategory,
    },
  };
}

function create_save_values(api, category, subcategory, values) {
  return {
    source: "client",
    type: "save",
    body: {
      api: api,
      category: category,
      subcategory: subcategory,
      values: values,
    },
  };
}

function create_save_ack(api, category, subcategory) {
  return {
    source: "gfs",
    type: "save_ack",
    body: {
      api: api,
      category: category,
      subcategory: subcategory,
    },
  };
}

module.exports = {
  validate: validate,
  create: create,
  create_request_meta: create_request_meta,
  create_save_values: create_save_values,
  create_save_ack: create_save_ack,
};
