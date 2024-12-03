/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   optional_logger.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2024-11-23
/// @copyright  2024 (license to be defined)

#pragma once

#include <functional>
#include <optional>
#include <string>

#include "i_logger.hpp"

namespace giraffe {

/// @brief Optional logger class
class OptionalLogger {
public:
  virtual ~OptionalLogger() = default;

  void setLogger(ILogger &logger) {
    logger_ = logger;
  }

protected:
  void logDebug(const std::string &msg) {
    if (logger_.has_value()) {
      logger_.value().get().debug(msg);
    }
  }

  void logInfo(const std::string &msg) {
    if (logger_.has_value()) {
      logger_.value().get().info(msg);
    }
  }

  void logWarn(const std::string &msg) {
    if (logger_.has_value()) {
      logger_.value().get().warn(msg);
    }
  }

  void logError(const std::string &msg) {
    if (logger_.has_value()) {
      logger_.value().get().error(msg);
    }
  }

private:
  std::optional<std::reference_wrapper<ILogger>> logger_ = std::nullopt;
};

} // namespace giraffe