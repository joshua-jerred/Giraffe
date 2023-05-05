const Message = require("../message");

class PathMessage extends Message {
  constructor(path) {
    super('client', 'ggs', "info", "path", null, { path: path});
  }
};

module.exports = {
  PathMessage,
};