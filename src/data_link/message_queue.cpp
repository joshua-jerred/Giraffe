/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   message_queue.cpp
 * @brief  Contains definition of the MessageQueue class.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-24
 * @copyright  2023 (license to be defined)
 */

#include "message_queue.hpp"

namespace gdl {

bool MessageQueue::push(Message message) {
  std::lock_guard<std::mutex> lock{mutex_};

  if (queue_.size() >= max_size_) {
    return false;
  }

  queue_.push(message);
  return true;
}

bool MessageQueue::pop(Message &message) {
  std::lock_guard<std::mutex> lock{mutex_};

  if (queue_.empty()) {
    return false;
  }

  message = queue_.front();
  queue_.pop();
  return true;
}

uint8_t MessageQueue::size() const {
  std::lock_guard<std::mutex> lock{mutex_};
  return queue_.size();
}

} // namespace gdl