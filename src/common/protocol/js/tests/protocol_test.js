const test = require("ava");
const protocol = require("../index");

test("Giraffe Protocol Message Constructor", (t) => {
  let valid_msg = new protocol.Message("gfs", "ggs", "req", "1234", {
    foo: "bar",
  });

  let valid_msg_json = {
    src: "gfs",
    dst: "ggs",
    typ: "req",
    id: "1234",
    bdy: {
      foo: "bar",
    },
  };

  t.deepEqual(valid_msg.json, valid_msg_json);
});

test("Giraffe Protocol Parse", (t) => {
  let valid_msg_json = {
    src: "gfs",
    dst: "ggs",
    typ: "req",
    id: "1234",
    bdy: {
      foo: "bar",
    },
  };

  let msg = protocol.parse(valid_msg_json);
  t.deepEqual(msg.json, valid_msg_json);
});

test("Giraffe Protocol Parse - Invalid src", (t) => {
  let valid_msg_json = {
    src: "ges",
    dst: "ggs",
    typ: "req",
    id: "1234",
    bdy: {
      foo: "bar",
    },
  };

  t.throws(
    () => {
      protocol.parse(valid_msg_json);
    },
    { instanceOf: Error, message: "Error: Invalid source: ges" }
  );
});

test("Giraffe Protocol Parse - Invalid dst", (t) => {
  let valid_msg_json = {
    src: "ggs",
    dst: "ges",
    typ: "req",
    id: "1234",
    bdy: {
      foo: "bar",
    },
  };

  t.throws(
    () => {
      protocol.parse(valid_msg_json);
    },
    { instanceOf: Error, message: "Error: Invalid destination: ges" }
  );
});

test("Giraffe Protocol Parse - Invalid typ", (t) => {
  let valid_msg_json = {
    src: "ggs",
    dst: "gwc",
    typ: "nul",
    id: "1234",
    bdy: {
      foo: "bar",
    },
  };

  t.throws(
    () => {
      protocol.parse(valid_msg_json);
    },
    { instanceOf: Error, message: "Error: Invalid type: nul" }
  );
});

test("Giraffe Protocol Parse - Invalid bdy", (t) => {
  let valid_msg_json = {
    src: "ggs",
    dst: "gwc",
    typ: "req",
    id: "1234",
    bdy: "foo",
  };

  t.throws(
    () => {
      protocol.parse(valid_msg_json);
    },
    { instanceOf: Error, message: "Error: Invalid body: foo" }
  );
});
