module.exports = class SocketExchangeQueue {
  constructor(global_state) {
    this.global_state = global_state;
    this.queue = [];
    this.sending = false;

    this.address = null;
    this.#updateClassSettings();

    this.error_count = 0;
    this.timeout_count = 0;
    this.parse_error_count = 0;
    this.success_count = 0;

    setInterval(this.#sendNext.bind(this), MESSAGE_DELAY);
  }

  addMessage(message, callback = (error, response) => {}) {
    this.queue.push({ message, callback });

    if (!this.sending) {
      this.#sendNext();
    }
  }

  getStats() {
    return {
      queue_length: this.queue.length,
      error_count: this.error_count,
      timeout_count: this.timeout_count,
      parse_error_count: this.parse_error_count,
      success_count: this.success_count,
    };
  }

  #updateClassSettings() {
    this.address = this.global_state.ggs_db.get(
      "settings",
      "gfs_connection",
      "gfs_socket_address"
    );
  }

  #sendMessage(message, callback) {
    // console.log("Sending message to FSA: " + JSON.stringify(message));
    let socket = new net.Socket();
    socket.setTimeout(FSA_TIMEOUT);
    socket.connect(FSA_PORT, this.address, () => {
      socket.write(JSON.stringify(message));
    });
    socket.on("data", (data) => {
      // console.log("FSA response: " + data);
      // this.#kickConnectionTimeout();
      try {
        let response_message = parse(data.toString());
        callback(null, response_message);
        this.success_count++;
      } catch (e) {
        callback("Failed to parse FSA response: " + e, null);
        this.parse_error_count++;
        return;
      }
    });
    socket.on("timeout", () => {
      callback("FSA connection timed out for resource: " + resource, null);
      socket.destroy();
      this.timeout_count++;
    });
    socket.on("error", (error) => {
      callback("FSA connection error: " + error, null);
      socket.destroy();
      this.error_count++;
    });
    socket.on("close", () => {
      // console.log("FSA connection closed.");
    });
  }

  #sendNext() {
    if (this.queue.length === 0) {
      this.sending = false;
      return;
    }
    // console.log("Sending next message.");
    let { message, callback } = this.queue.shift();
    this.sending = true;
    this.#sendMessage(message, callback);
  }
};
