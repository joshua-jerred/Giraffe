const test = require("ava").serial;
const ping = require("../src/variants/ping");
const { ajv } = require("../src/validation/validate");
const validate = ajv.getSchema("ping");


test("create_request_valid", (t) => {
  let msg = new ping.PingRequest("client", "ggs");
  t.is(msg.src, "client");
  t.is(msg.dst, "ggs");
  t.is(msg.typ, "req");
  t.is(msg.cat, "ping");
  t.deepEqual(msg.body, {});
});

test("create_response_valid", (t) => {
  let msg = new ping.PingResponse("client", "ggs", "123");
  t.is(msg.src, "client");
  t.is(msg.dst, "ggs");
  t.is(msg.typ, "rsp");
  t.is(msg.cat, "ping");
  t.is(msg.id, "123");
  t.deepEqual(msg.body, {});
});

test("validate_valid", (t) => {
  const msg = {
    src: "client",
    dst: "ggs",
    typ: "req",
    cat: "ping",
    id: "123",
    body: {},
  };
  t.true(validate(msg));
});

test("validate_invalid", (t) => {
  const msg = {
    src: "client",
    dst: "ggs",
    typ: "info",
    cat: "ping",
    id: "123",
    body: {},
  };
  t.false(validate(msg));
});