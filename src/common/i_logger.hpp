/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   i_logger.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2024-11-23
/// @copyright  2024 (license to be defined)

#pragma once

#include <string>

namespace giraffe {

class ILogger {
public:
  virtual ~ILogger() = default;

  virtual void debug(const std::string &msg) = 0;
  virtual void info(const std::string &msg) = 0;
  virtual void warn(const std::string &msg) = 0;
  virtual void error(const std::string &msg) = 0;
};

class DummyLogger : public ILogger {
public:
  void debug(const std::string &msg) override {
    (void)msg;
  }
  void info(const std::string &msg) override {
    (void)msg;
  }
  void warn(const std::string &msg) override {
    (void)msg;
  }
  void error(const std::string &msg) override {
    (void)msg;
  }
};

} // namespace giraffe