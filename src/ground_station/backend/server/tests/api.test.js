const test = require('ava');
const request = require('supertest');
const app =  require('../index');
const loadMetaData = require("../metadata/metaLoader");

test('GET /api/status', async t => {
  const response = await request(app)
    .get('/api/status');
    t.is(response.status, 200);
    t.true(response.body.hasOwnProperty('gfs'));
    t.true(response.body.hasOwnProperty('ggs'));
    t.true(response.body.hasOwnProperty('influx_db'));
    t.true(response.body.hasOwnProperty('telemetry'));

})

test('GET /api/ggs/settings - all', async t => {
  settings = loadMetaData("ggs", "settings");
  for (key in settings) {
    let path = '/api/ggs/settings?category=' + key + '&include=all';
    let resource = settings[key];

    const response = await request(app)
    .get(path);
    t.is(response.status, 200);
    t.true(response.body.hasOwnProperty('metadata'));
    t.true(response.body.hasOwnProperty('values'));
    t.deepEqual(response.body.metadata, resource);
    for (key in resource) {
      t.true(response.body.values.hasOwnProperty(key));
    }
  }
})

test('PUT /api/ggs/settings', async t => {
  const setting_category = 'gfs_connection';
  const setting_name = 'gfs_server_address';
  const path = '/api/ggs/settings?category=' + setting_category;
  async function get_settings() {
    const response = await request(app)
    .get(path);
    t.true(response.body.hasOwnProperty('metadata'));
    t.true(response.body.hasOwnProperty('values'));
    t.true(response.body.values.hasOwnProperty(setting_name));
    return response.body.values[setting_name];
  }

  const original_setting_value = await get_settings();

  const new_setting_value = 'new_setting_value';

  const response = await request(app)
    .put(path)
    .send({
      [setting_name]: new_setting_value
    });
  t.is(response.status, 200);
  

  console.log(original_setting_value);
  t.pass();
})