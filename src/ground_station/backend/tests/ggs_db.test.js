const test = require("ava");
const GgsDataBase = require("../db/ggs_db");

test("Load value from DB", (t) => {
  const db = new GgsDataBase();

  // db.resetDb();

  const value = db.get("settings", "ggs_settings", "logging_level");
  t.is(value, "info");
});

/// @todo bring back to life without breaking dev environments
// test("set_value", async (t) => {
//   const db = new GgsDataBase();

//   db.setKey("settings", "gfs_connection", "gfs_server_address", "bar");
//   const bar = db.get("settings", "gfs_connection", "gfs_server_address");
//   t.is(await bar, "bar");

//   db.setKey("settings", "gfs_connection", "gfs_server_address", "localhost");
//   const localhost = db.get("settings", "gfs_connection", "gfs_server_address");
//   t.is(await localhost, "localhost");
// });
