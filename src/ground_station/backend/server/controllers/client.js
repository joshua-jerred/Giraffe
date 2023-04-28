const GgsConfigure = require("../db/ggs_db");
const loadMetaData = require("../metadata/metaLoader");
const create = require("giraffe-protocol/message").create;
const save_ack = require("giraffe-protocol/message").create_save_ack;

const ggs_db = new GgsConfigure();

function ggsDataSaver(table, category, values) {
  console.log("Saving data for " + category + " " + subcategory + ":" + values);
  let res = ggs_db.setCategory(table, category, values);
  if (res === undefined || res === null) {
    console.log("Invalid category");
    return null;
  }
}

function ggsDataLoader(table, category) {
  let res = ggs_db.get(table, category);
  if (res === undefined || res === null) {
    return null;
  }
  return res;
}

function ggsNak(ws, category, subcategory) {
  if (category === null || category === undefined) {
    category = "null";
  }
  if (subcategory === null || subcategory === undefined) {
    subcategory = "null";
  }
  let pkt = {
    source: "ggs",
    type: "nack",
    body: {
      api: "ggs",
      category: category,
      subcategory: subcategory,
    },
  };
  ws.send(JSON.stringify(pkt));
}

function clientController(message, ws) {
  let values = null;
  let api = null;
  let table = null;
  let category = null;

  switch (message.type) {
    case "meta":
      api = message["body"].api;
      table = message["body"].table;
      category = message["body"].category;

      let meta = loadMetaData(api, table, category);

      values = {};
      if (api === "ggs") {
        values = ggsDataLoader(table, category);
        if (values === null) {
          ggsNak(ws, table, category);
          return;
        }
      } else {
        ggsNak(ws, table, category);
        return;
      }

      let body = {
        api: api,
        table: table,
        category: category,
        meta: meta,
        values: values,
      };
      let pkt = create("ggs", "meta", body);
      ws.send(JSON.stringify(pkt));
      break;
    case "save":
      values = message["body"].values;
      api = message["body"].api;

      table = message["body"].table;
      category = message["body"].category;

      let saved = false;
      if (api === "ggs" && ggsDataSaver(table, category, values)) {
        saved = true;
      } else {
        ggsNak(ws, api, table, category);
        return;
        saved = false;
      }
      if (saved) {
        let pkt = save_ack(api, table, category);
        ws.send(JSON.stringify(pkt));
      }
      break;
    case "path":
      console.log("This should not happen");
      break;
    default:
      console.log("Invalid message type");
      ggsNak(ws, api, table, category);
      break;
  }
}

module.exports = clientController;
