/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   gdl_message.hpp
 * @brief  Message and Message Queue for Giraffe Data Link
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-18
 * @copyright  2024 (license to be defined)
 */

#ifndef GDL_MESSAGE_HPP_
#define GDL_MESSAGE_HPP_

#include <cstdint>
#include <mutex>
#include <queue>
#include <string>

#include <gdl_constants.hpp>

namespace giraffe::gdl {
class Message {
public:
  enum class Type : uint8_t { BROADCAST, EXCHANGE, LOCATION };

  struct Location {
    double latitude = 0;
    double longitude = 0;
    uint32_t altitude = 0;
    double speed = 0;
    int heading = 0;
  };

  Message() = default;

  bool setBroadcastMessage(std::string broadcast_message, uint32_t identifier) {
    if (broadcast_message.size() > GDL_MESSAGE_DATA_MAX_SIZE) {
      return false;
    }

    identifier_ = identifier;
    data_ = std::move(broadcast_message);
    type_ = Type::BROADCAST;
    return true;
  }

  bool setExchangeMessage(std::string exchange_message, uint32_t identifier) {
    if (exchange_message.size() > GDL_MESSAGE_DATA_MAX_SIZE) {
      return false;
    }

    identifier_ = identifier;
    data_ = std::move(exchange_message);
    type_ = Type::EXCHANGE;
    return true;
  }

  bool setLocationMessage(Location location, uint32_t identifier) {
    /// @todo validate values
    identifier_ = identifier;
    data_ = "";
    location_ = location;
    type_ = Type::LOCATION;
    return true;
  }

private:
  uint32_t identifier_ = 0;
  Type type_{Type::BROADCAST};
  std::string data_{};
  Location location_{};

  uint32_t retries_ = 0;
};

/**
 * @brief A queue for messages.
 */
class MessageQueue {
public:
  MessageQueue() = default;
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
  mutable std::mutex mutex_{};
  std::queue<Message> queue_{};
};
} // namespace giraffe::gdl

#endif /* GDL_MESSAGE_HPP_ */