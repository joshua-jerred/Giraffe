/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   logger.hpp
 * @brief  A simple logger class so difference classes have access to the same
 * logger.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-13
 * @copyright  2024 (license to be defined)
 */

#ifndef GFS_LOGGER_HPP_
#define GFS_LOGGER_HPP_

#include "shared_data.hpp"

namespace giraffe {
class Logger {
public:
  Logger(data::LogStream &log_stream, node::Identification source,
         std::string secondary_source = "")
      : log_stream_(log_stream), source_(source),
        secondary_source_(secondary_source) {
  }

  void info(std::string message) {
    if (!secondary_source_.empty()) {
      message = secondary_source_ + ": " + message;
    }
    log_stream_.info(source_, message);
  }

  void debug(std::string message) {
    if (!secondary_source_.empty()) {
      message = secondary_source_ + ": " + message;
    }
    log_stream_.debug(source_, message);
  }

  void error(DiagnosticId error_id, std::string info = "") {
    if (!secondary_source_.empty()) {
      info = secondary_source_ + ": " + info;
    }
    log_stream_.error(source_, error_id, info);
  }

private:
  data::LogStream &log_stream_;
  node::Identification source_;
  std::string secondary_source_;
};

} // namespace giraffe

#endif /* GFS_LOGGER_HPP_ */