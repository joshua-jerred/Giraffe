class Logger {
  constructor(log_label) {
    this.log_label = log_label;
  }

  #log(level, message, ...args) {
    let processed_message = message;
    if (typeof message === "object") {
      processed_message = JSON.stringify(message, null, 2);
    } else if (typeof message !== "string") {
      processed_message = String(message);
    }
    if (args.length > 0) {
      processed_message +=
        " " + args.map((arg) => JSON.stringify(arg)).join(" ");
    }
    if (args.length > 0 && args[0] instanceof Error) {
      processed_message += " " + args[0].stack;
    }

    const timestamp = new Date().toISOString();
    console.log(
      `[${timestamp}] [${this.log_label}] [${level}] ${processed_message}`
    );
  }

  error(message) {
    this.#log("ERROR", message);
  }
  warning(message, ...args) {
    this.#log("WARNING", message, ...args);
  }
  info(message) {
    this.#log("INFO", message);
  }
  debug(message, ...args) {
    this.#log("DEBUG", message, ...args);
  }
}

module.exports = {
  Logger,
  debug: (source, message, ...args) => {
    const logger = new Logger(source);
    logger.debug(message, ...args);
  },
  info: (source, message, ...args) => {
    const logger = new Logger(source);
    logger.info(message, ...args);
  },
  warning: (source, message, ...args) => {
    const logger = new Logger(source);
    logger.warning(message, ...args);
  },
  error: (source, message, ...args) => {
    const logger = new Logger(source);
    logger.error(message, ...args);
  },
  createLogger: (source) => {
    return new Logger(source);
  },
};
