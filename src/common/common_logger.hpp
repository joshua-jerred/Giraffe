/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   common_logger.hpp
 * @brief  A common logger for Giraffe
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-23
 * @copyright  2024 (license to be defined)
 */

#ifndef GIRAFFE_COMMON_LOGGER_HPP_
#define GIRAFFE_COMMON_LOGGER_HPP_

#include <chrono>
#include <iostream>
#include <queue>
#include <string>

#include <fmt/chrono.h>
#include <fmt/core.h>

#include <BoosterSeat/filesystem.hpp>

#include "i_logger.hpp"

namespace giraffe {

enum class LoggerLevel : uint8_t { DEBUG = 0, INFO = 1, WARN = 2, ERROR = 3 };
std::string to_string(const LoggerLevel level);

struct LoggerEntry {
  LoggerLevel level;
  std::string msg;
  std::chrono::system_clock::time_point time;

  std::string getLevelString() const {
    return to_string(level);
  }
};

template <size_t LOG_SIZE> class CommonLogger : public ILogger {
public:
  CommonLogger(LoggerLevel level = LoggerLevel::INFO,
               bool print_to_stdout = false, std::string log_file_path = "")
      : level_to_log_(level), print_to_stdout_(print_to_stdout),
        log_to_file_(!log_file_path.empty()), log_file_path_(log_file_path) {
  }
  ~CommonLogger() = default;

  void debug(const std::string &msg) override {
    log(LoggerLevel::DEBUG, msg);
  }

  void info(const std::string &msg) override {
    log(LoggerLevel::INFO, msg);
  }

  void warn(const std::string &msg) override {
    log(LoggerLevel::WARN, msg);
  }

  void error(const std::string &msg) override {
    log(LoggerLevel::ERROR, msg);
  }

  void setLevel(const LoggerLevel level) {
    level_to_log_ = level;
  }

  LoggerLevel getLevel() const {
    return level_to_log_;
  }

  void setPrintToStdout(const bool print_to_stdout) {
    print_to_stdout_ = print_to_stdout;
  }

  bool getPrintToStdout() const {
    return print_to_stdout_;
  }

  bool popEntry(LoggerEntry &entry) {
    if (log_queue_.empty()) {
      return false;
    }
    entry = log_queue_.front();
    log_queue_.pop();
    return true;
  }

  size_t getLogQueueSize() const {
    return log_queue_.size();
  }

private:
  void log(const LoggerLevel level, const std::string &msg) {

    // don't log if the level of this item is lower than the level to log
    if (static_cast<std::underlying_type_t<LoggerLevel>>(level) <
        static_cast<std::underlying_type_t<LoggerLevel>>(level_to_log_)) {
      return;
    }

    LoggerEntry entry{level, msg, std::chrono::system_clock::now()};
    if (log_queue_.size() > LOG_SIZE) {
      log_queue_.pop();
    }
    log_queue_.push(entry);

    if (print_to_stdout_ || log_to_file_) {
      std::string log_msg =
          fmt::format("{} [{}] : {}\n", fmt::format("{:%H:%M:%S}", entry.time),
                      entry.getLevelString(), entry.msg);

      if (print_to_stdout_) {
        std::cout << log_msg << std::flush;
      }
      if (log_to_file_) {
        // write to file
        bst::filesystem::appendToFile(log_file_path_, log_msg, false);
      }
    }
  }

  LoggerLevel level_to_log_;
  bool print_to_stdout_;
  bool log_to_file_;
  std::string log_file_path_;
  std::queue<LoggerEntry> log_queue_{};
};

} // namespace giraffe

#endif /* GIRAFFE_COMMON_LOGGER_HPP_ */