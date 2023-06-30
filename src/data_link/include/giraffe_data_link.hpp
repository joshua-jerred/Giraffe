/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   giraffe_data_link.hpp
 * @brief  The external interface for the Giraffe Data Link
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-24
 * @copyright  2023 (license to be defined)
 */

#ifndef GIRAFFE_DATA_LINK_HPP_
#define GIRAFFE_DATA_LINK_HPP_

#include <atomic>
#include <functional>
#include <thread>

#include "gdl_configuration.hpp"
#include "gdl_message.hpp"

#include "message_queue.hpp"

namespace gdl {
/**
 * @brief The Giraffe Data Link core class/interface - Layer 4 (Application)
 */
class GiraffeDataLink {
public:
  /**
   * @brief The status of the GDL instance.
   * @details All values are uint8_t, stopped/error states start with 0x0,
   * running states start with 0xF.
   */
  enum class Status : uint8_t {
    ERROR = 0x00,
    STOPPED = 0x01,
    IDLE = 0xF1,
    RECEIVING = 0xF2,
    TRANSMITTING = 0xF3,
    STOPPING = 0xF4
  };

  /**
   * @brief Create a new Giraffe Data Link instance
   * @param config - The configuration for the GDL instance
   */
  GiraffeDataLink(GdlConfig config);

  /**
   * @brief Deconstruct the GDL instance, this will stop the GDL instance if it
   * is running.
   */
  ~GiraffeDataLink();

  /**
   * @brief Start the GDL instance.
   * @details If it's already running, this will do nothing.
   */
  void start();

  /**
   * @brief Stop the GDL instance.
   * @details If it's already stopped, this will do nothing.
   */
  void stop();

  /**
   * @brief Get the status of the GDL instance.
   * @return GiraffeDataLink::Status - The status of the GDL instance.
   */
  GiraffeDataLink::Status status() const;

  /**
   * @brief Add a message to the exchange queue.
   *
   * @param message - The message to add to the queue.
   * @return true - If the message was added to the queue.
   * @return false - If the message was not added to the queue.
   */
  bool exchange(Message message);

  /**
   * @brief Add a message to the broadcast queue.
   *
   * @param message - The message to add to the queue.
   * @return true - If the message was added to the queue.
   * @return false - If the message was not added to the queue.
   */
  bool broadcast(Message message);

private:
  const GdlConfig config_;
  std::thread gdl_thread{};

  /**
   * @brief The status of the GDL instance.
   */
  std::atomic<Status> status_{Status::STOPPED};
};

} // namespace gdl

#endif /* GIRAFFE_DATA_LINK_HPP_ */