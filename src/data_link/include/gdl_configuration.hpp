/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   gdl_configuration.hpp
 * @brief  Contains the configuration struct for GDL
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-24
 * @copyright  2023 (license to be defined)
 */

#ifndef GDL_CONFIGURATION_HPP_
#define GDL_CONFIGURATION_HPP_

#include <cstdint>

namespace gdl {

/**
 * @brief Configuration data for Giraffe Data Link
 */
struct GdlConfig {
  /**
   * @brief The size of the broadcast queue.
   */
  uint8_t broadcast_queue_size = 10;

  /**
   * @brief The size of the exchange queue.
   */
  uint8_t exchange_queue_size = 10;

  /**
   * @brief The size of the queue for received messages.
   */
  uint8_t received_queue_size = 10;

  /**
   * @brief The maximum number of retries for a message.
   * @details This will reset for each packet.
   */
  uint8_t max_retries = 3;

  /**
   * @brief The interval between retries in seconds.
   */
  uint16_t retry_interval = 10;

  /**
   * @brief The interval between connection status messages in seconds.
   * @details This is only used when idle.
   */
  uint16_t connection_status_interval = 30;

  /**
   * @brief The maximum size (payload) of a message in bytes.
   */
  uint16_t max_message_size = 1024;

  /**
   * @brief The default timeout for a message in seconds.
   */
  uint16_t default_timeout = 30;
};

} // namespace gdl

#endif /* GDL_CONFIGURATION_HPP_ */