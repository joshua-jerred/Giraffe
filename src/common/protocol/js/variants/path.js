const { SetMessage } = require("../set");

class PathMessage extends SetMessage {
  constructor(path) {
    super("gwc", "ggs", "path", { path: path });
  }
}

module.exports = {
  PathMessage,
};
