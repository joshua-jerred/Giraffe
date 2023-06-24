/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   message_queue.hpp
 * @brief  A queue for messages.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-24
 * @copyright  2023 (license to be defined)
 */

#ifndef MESSAGE_QUEUE_HPP_
#define MESSAGE_QUEUE_HPP_

#include <mutex>
#include <queue>

#include "gdl_configuration.hpp"
#include "gdl_message.hpp"

namespace gdl {

/**
 * @brief A queue for messages.
 */
class MessageQueue {
public:
  /**
   * @brief Construct a new Message Queue.
   * @param max_queue_size - The maximum size of the queue.
   */
  MessageQueue(const uint8_t max_queue_size) : max_size_{max_queue_size} {
  }

  /**
   * @brief Destroy the Message Queue object.
   */
  ~MessageQueue() = default;

  /**
   * @brief Push a message onto the queue.
   * @details This will lock the queue. If there is no space on the queue, the
   * message will not be pushed and false will be returned.
   *
   * @param message - The message to push.
   * @return true - If the message was pushed.
   * @return false - If the message was not pushed.
   */
  bool push(Message message);

  /**
   * @brief Pop a message from the queue.
   * @details This will lock the queue. If there is no message on the queue,
   * false will be returned.
   *
   * @param message - The message that was popped.
   * @return true - If a message was popped.
   * @return false - If a message was not popped.
   */
  bool pop(Message &message);

  /**
   * @brief Get the size of the queue.
   * @return int - The size of the queue.
   */
  uint8_t size() const;

private:
  const uint8_t max_size_;

  mutable std::mutex mutex_{};
  std::queue<Message> queue_{};
};

} // namespace gdl

#endif /* MESSAGE_QUEUE_HPP_ */
