const test = require("ava");
const GgsDataBase = require("../db/ggs_db");

test("load_value", (t) => {
  const db = new GgsDataBase();

  db.resetDb();

  const value = db.get("settings", "gfs_connection", "gfs_server_address");
  t.is(value, "localhost");
});

test("set_value", async (t) => {
  const db = new GgsDataBase();

  db.setKey("settings", "gfs_connection", "gfs_server_address", "bar");
  const bar = db.get("settings", "gfs_connection", "gfs_server_address");
  t.is(await bar, "bar");

  db.setKey("settings", "gfs_connection", "gfs_server_address", "localhost");
  const localhost = db.get("settings", "gfs_connection", "gfs_server_address");
  t.is(await localhost, "localhost");
});
