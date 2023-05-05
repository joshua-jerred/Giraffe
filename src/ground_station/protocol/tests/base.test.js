const test = require("ava").serial;
const { ajv } = require("../src/validation/validate");
const base_validate = ajv.getSchema("base_message");
const Message = require("../src/message");

test("constructor_valid", (t) => {
  t.notThrows(() => {
    new Message("client", "ggs", "info", "ping", "123", { test: "test " });
  }, "This is a valid message and it should not throw an error");
});

test("constructor_invalid_source", (t) => {
  try {
    new Message("invalid", "ggs", "info", "ping", "123", { test: "test " });
    t.fail("Exception not thrown");
  } catch (e) {
    t.true(e.message.includes("Invalid source: invalid"));
  }
});

test("constructor_invalid_destination", (t) => {
  try {
    new Message("client", "invalid", "info", "ping", "123", { test: "test " });
    t.fail("Exception not thrown");
  } catch (e) {
    t.true(e.message.includes("Invalid destination: invalid"));
  }
});

test("constructor_invalid_type", (t) => {
  try {
    new Message("client", "ggs", "invalid", "ping", "123", { test: "test " });
    t.fail("Exception not thrown");
  } catch (e) {
    t.true(e.message.includes("Invalid type: invalid"));
  }
});

test("constructor_invalid_category", (t) => {
  try {
    new Message("client", "ggs", "info", "invalid", "123", { test: "test " });
    t.fail("Exception not thrown");
  } catch (e) {
    t.true(e.message.includes("Invalid category: invalid"));
  }
});

test("constructor_invalid_id", (t) => {
  let id = 123;

  let msg = new Message("client", "ggs", "info", "ping", 123, { test: "test " });
  t.true(typeof msg.id === "string");
  t.not(msg.id, id);
});

test("constructor_invalid_body", (t) => {
  try {
    new Message("client", "ggs", "info", "ping", "123", "test");
    t.fail("Exception not thrown");
  } catch (e) {
    t.true(e.message.includes("Invalid body: test"));
  }
});

test("getJson", (t) => {
  const msg = new Message("client", "ggs", "info", "ping", "123", { t: "1" });
  t.deepEqual(msg.getJson(), {
    src: "client",
    dst: "ggs",
    typ: "info",
    cat: "ping",
    id: "123",
    body: {
      t: "1",
    },
  });
});

test("getString", (t) => {
  const msg = new Message("client", "ggs", "info", "ping", "123", { t: "1" });
  let expected = JSON.stringify({
    src: "client",
    dst: "ggs",
    typ: "info",
    cat: "ping",
    id: "123",
    body: {
      t: "1",
    },
  });

  t.deepEqual(msg.getString(), expected);
});

test("toString", (t) => {
  const msg = new Message("client", "ggs", "info", "ping", "123", { t: "1" });
  let ret = msg.toString();
  if (ret === undefined) {
    t.fail("toString returned undefined");
  } else if (ret.length < 20) {
    t.fail("toString returned a string that is too short" + ret);
  } else {
    t.pass();
  }
});

test("validate_valid", (t) => {
  const msg = {
    src: "client",
    dst: "ggs",
    typ: "info",
    cat: "ping",
    id: "123",
    body: {},
  };
  t.true(base_validate(msg));
});

test("validate_src_valid_invalid_missing", (t) => {
  let valid = {
    src: "ggs",
    dst: "ggs",
    typ: "info",
    cat: "ping",
    id: "123",
    body: {},
  };
  let invalid = {
    src: "invalid",
    dst: "ggs",
    typ: "info",
    cat: "ping",
    id: "123",
    body: {},
  };
  let missing = {
    dst: "ggs",
    typ: "info",
    cat: "ping",
    id: "123",
    body: {},
  };

  t.true(base_validate(valid));
  t.false(base_validate(invalid));
  t.false(base_validate(missing));
});

test("validate_dst_valid_invalid_missing", (t) => {
  let valid = {
    src: "ggs",
    dst: "ggs",
    typ: "info",
    cat: "ping",
    id: "123",
    body: {},
  };
  let invalid = {
    src: "ggs",
    dst: "invalid",
    typ: "info",
    cat: "ping",
    id: "123",
    body: {},
  };
  let missing = {
    src: "ggs",
    typ: "info",
    cat: "ping",
    id: "123",
    body: {},
  };

  t.true(base_validate(valid));
  t.false(base_validate(invalid));
  t.false(base_validate(missing));
});

test("validate_typ_valid_invalid_missing", (t) => {
  let valid = {
    src: "ggs",
    dst: "ggs",
    typ: "info",
    cat: "ping",
    id: "123",
    body: {},
  };
  let invalid = {
    src: "ggs",
    dst: "ggs",
    typ: "invalid",
    cat: "ping",
    id: "123",
    body: {},
  };
  let missing = {
    src: "ggs",
    dst: "ggs",
    cat: "ping",
    id: "123",
    body: {},
  };

  t.true(base_validate(valid));
  t.false(base_validate(invalid));
  t.false(base_validate(missing));
});