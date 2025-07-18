const test = require("ava").serial;
const request = require("supertest");
// const app = require("../index");
const loadMetaData = require("../metadata/metaLoader");

async function getSettings(t, resource, setting_category, include = "all") {
  const path = "/api/" + resource + "/settings";
  const response = await request(app)
    .get(path)
    .query({ category: setting_category, include: include });
  t.is(response.status, 200);

  if (include === "all") {
    t.true(response.body.hasOwnProperty("metadata"));
    t.true(response.body.hasOwnProperty("values"));
  } else if (include === "values") {
    t.false(response.body.hasOwnProperty("metadata"));
    t.true(response.body.hasOwnProperty("values"));
  } else if (include === "metadata") {
    t.true(response.body.hasOwnProperty("metadata"));
    t.false(response.body.hasOwnProperty("values"));
  }
  return response.body;
}

async function setSettings(t, resource, setting_category, values) {
  const path = "/api/" + resource + "/settings";
  const response = await request(app)
    .put(path)
    .query({ category: setting_category })
    .send(values);
  t.is(response.status, 200);
  return response.body;
}

test.skip("GET /api/status", async (t) => {
  const response = await request(app).get("/api/status");
  t.is(response.status, 200);
  t.true(response.body.hasOwnProperty("gfs"));
  t.true(response.body.hasOwnProperty("influxdb"));
  t.true(response.body.hasOwnProperty("gdl"));
});

test.skip("GET /api/ggs/settings", async (t) => {
  settings = loadMetaData("ggs", "settings");

  for (key in settings) {
    let resource = settings[key];

    const body = await getSettings(t, "ggs", key, "all");

    t.deepEqual(body.metadata, resource);
    for (key in resource) {
      t.true(body.values.hasOwnProperty(key));
    }
  }
});

// test("PUT /api/ggs/settings", async (t) => {
//   const setting_category = "gfs_connection";
//   const setting_name = "gfs_server_address";
//   const new_setting_value = "new_setting_value";

//   async function get() {
//     let result = await getSettings(t, "ggs", setting_category, "values");
//     t.true(result.values.hasOwnProperty(setting_name));
//     return result.values[setting_name];
//   }

//   async function set(value) {
//     let vals = {  };
//     vals[setting_name] = value;
//     await setSettings(t, "ggs", setting_category, vals);
//   }

//   let original_setting_value = await get();

//   await set(new_setting_value);
//   let updated_setting_value = await get();
//   t.is(updated_setting_value, new_setting_value);
//   await set(original_setting_value);
//   updated_setting_value = await get();
//   t.is(updated_setting_value, original_setting_value);
// });

// test("DELETE /api/ggs/settings", async (t) => {
//   const setting_category = "gfs_connection";
//   const values = {
//     gfs_server_address: "TEMPORARY_VALUE",
//     gfs_server_port: 12,
//   };
//   const path = "/api/ggs/settings";

//   await setSettings(t, "ggs", setting_category, values);

//   const body = await getSettings(t, "ggs", setting_category, "values");
//   t.true(body.values.hasOwnProperty("gfs_server_address"));
//   t.true(body.values.hasOwnProperty("gfs_server_port"));

//   t.is(body.values.gfs_server_address, values.gfs_server_address);
//   t.is(body.values.gfs_server_port, values.gfs_server_port);

//   const response1 = await request(app)
//     .delete(path)
//     .query({ category: setting_category });
//   t.is(response1.status, 200);

//   const body2 = await getSettings(t, "ggs", setting_category, "values");
//   t.true(body2.values.hasOwnProperty("gfs_server_address"));
//   t.true(body2.values.hasOwnProperty("gfs_server_port"));
//   t.not(body2.values.gfs_server_address, values.gfs_server_address);
//   t.not(body2.values.gfs_server_port, values.gfs_server_port);
// });
