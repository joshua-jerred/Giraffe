module.exports = class Timer {
  constructor(timeout_ms) {
    this.timeout_ms = timeout_ms;
    this.start_time = null;
    // this.timeout_id = null;

    this.restart();
  }

  restart() {
    this.start_time = Date.now();
    // this.timeout_id = setTimeout(() => {}, this.timeout_ms);
  }

  isExpired() {
    return Date.now() - this.start_time >= this.timeout_ms;
  }

  getRemainingTime() {
    return this.timeout_ms - (Date.now() - this.start_time);
  }

  getElapsedTime() {
    return Date.now() - this.start_time;
  }
};
