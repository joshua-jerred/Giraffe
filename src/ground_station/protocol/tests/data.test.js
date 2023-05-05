const test = require("ava").serial;
const data = require("../src/variants/data");
const { ajv } = require("../src/validation/validate");
const validate_req = ajv.getSchema("data_request");
const validate_rsp = ajv.getSchema("data_response");

test("create_request_valid", (t) => {
  let params = {
    resource: "rsrc",
    category: "cat",
  }
  let msg = new data.DataRequest(
    "client",
    "ggs",
    params
  );
  t.is(msg.src, "client");
  t.is(msg.dst, "ggs");
  t.is(msg.typ, "req");
  t.is(msg.cat, "data");
  t.deepEqual(msg.body, {
    params: params,
  });
});

test("create_response_valid", (t) => {
  let params = {
    resource: "rsrc",
    category: "cat",
  }
  
  let msg = new data.DataResponse(
    "client",
    "ggs",
    "123",
    { a: "b" },
    params
  );
  t.is(msg.src, "client");
  t.is(msg.dst, "ggs");
  t.is(msg.typ, "rsp");
  t.is(msg.cat, "data");
  t.is(msg.id, "123");
  t.deepEqual(msg.body, {
    params: params,
    data: {
      a: "b",
    },
  });
});

test("validate_req", (t) => {
  const msg = {
    src: "client",
    dst: "ggs",
    typ: "req",
    cat: "data",
    id: "123",
    body: {
      params: {},
    },
  };
  t.true(validate_req(msg));
});

test("validate_rsp", (t) => {
  const msg = {
    src: "client",
    dst: "ggs",
    typ: "rsp",
    cat: "data",
    id: "123",
    body: {
      params: {},
      data: {},
    },
  };
  t.true(validate_rsp(msg), `${JSON.stringify(validate_rsp.errors)}`);
});
