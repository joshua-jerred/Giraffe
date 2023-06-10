const { SetMessage } = require("../set");

class PathMessage extends SetMessage {
  constructor(path) {
    super("gws", "ggs", "path", { path: path });
  }
}

module.exports = {
  PathMessage,
};
