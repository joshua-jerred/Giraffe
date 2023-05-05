const test = require('ava').serial;

const Message = require('../src/message');
const parse = require('../src/parse');

test('prase_message_json', (t) => {
  t.notThrows(() => {
    let cont = {
      src: 'client',
      dst: 'ggs',
      typ: 'info',
      cat: 'ping',
      id: '123',
      body: { test: 'test' },
    };
    let msg = new Message(
      cont.src,
      cont.dst,
      cont.typ,
      cont.cat,
      cont.id,
      cont.body
    );
    let json_msg = msg.getJson();
    let parsed_msg = parse(json_msg);

    t.deepEqual(msg.getJson(), parsed_msg.getJson());
  }, 'This is a valid message and it should not throw an error');
});

test('prase_message_string', (t) => {
  t.notThrows(() => {
    let cont = {
      src: 'client',
      dst: 'ggs',
      typ: 'info',
      cat: 'ping',
      id: '123',
      body: { test: 'test' },
    };
    let msg = new Message(
      cont.src,
      cont.dst,
      cont.typ,
      cont.cat,
      cont.id,
      cont.body
    );

    let json_msg = msg.getString();
    let parsed_msg = parse(json_msg);

    t.deepEqual(cont, parsed_msg.getJson());
  }, 'This is a valid message and it should not throw an error');
});

test('parse_json', (t) => {
  t.notThrows(() => {
    let cont = {
      src: 'client',
      dst: 'ggs',
      typ: 'info',
      cat: 'ping',
      id: '123',
      body: { test: 'test' },
    };
    let parsed_msg = parse(cont);

    t.deepEqual(cont, parsed_msg.getJson());
  }, 'This is a valid message and it should not throw an error');
});

test('parse_string', (t) => {
  t.notThrows(() => {
    let str = '{"src":"client","dst":"ggs","typ":"info","cat":"ping","id":"123","body":{"test":"test"}}';
    parse(str);
  }, 'This is a valid message and it should not throw an error');
});
