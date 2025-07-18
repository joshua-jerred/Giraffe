module.exports = class FsaConnection {
  constructor(global_state) {
    this.global_state = global_state;
  }

  start(res) {
    console.log("Starting FSA");
    console.log("-- stub --");
    res.json({ message: "Starting FSA" });
  }

  stop(res) {
    console.log("Stopping FSA");
    console.log("-- stub --");
    res.json({ message: "Stopping FSA" });
  }
};
