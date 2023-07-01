/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   log_container.hpp
 * @brief  A simple logging system for debugging.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-30
 * @copyright  2023 (license to be defined)
 */

#ifndef LOG_CONTAINER_HPP_
#define LOG_CONTAINER_HPP_

#include <deque>
#include <mutex>

constexpr int kLogContainerSize = 20;

class LogContainer {
public:
  LogContainer() = default;
  ~LogContainer() = default;

  void add(std::string message) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    if (log_container_.size() >= kLogContainerSize) {
      log_container_.pop_front();
    }
    log_container_.push_back(message);
  }

  std::deque<std::string> get() {
    std::lock_guard<std::mutex> lock(log_mutex_);
    return log_container_;
  }

private:
  std::deque<std::string> log_container_{};
  std::mutex log_mutex_{};
};

#endif /* LOG_CONTAINER_HPP_ */