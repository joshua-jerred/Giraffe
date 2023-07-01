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
#include "gdl_message_queue.hpp"

namespace gdl {
/**
 * @brief The Giraffe Data Link core class/interface - Layer 4 (Application)
 */
class GiraffeDataLink {
public:
  /**
   * @brief The status of the GDL instance.
   */
  enum class Status { ERROR, STOPPED, STARTING, RUNNING, STOPPING };

  enum class ConnectionStatus { DISCONNECTED, CONNECTED };

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
  GiraffeDataLink::Status getStatus() const;

  /**
   * @brief Get the connection status of the GDL instance.
   * @return GiraffeDataLink::ConnectionStatus - The connection status
   */
  ConnectionStatus getConnectionStatus() const;

  /**
   * @brief Add a message to the exchange queue.
   *
   * @param message - The message to add to the queue.
   * @return true - If the message was added to the queue.
   * @return false - If the message was not added to the queue.
   */
  bool addExchangeMessage(Message message);

  /**
   * @brief Add a message to the broadcast queue.
   *
   * @param message - The message to add to the queue.
   * @return true - If the message was added to the queue.
   * @return false - If the message was not added to the queue.
   */
  bool addBroadcastMessage(Message message);

private:
  struct MessageQueues {
    MessageQueues(int exchange_queue_size, int broadcast_queue_size,
                  int receive_queue_size)
        : exchange(exchange_queue_size), broadcast(broadcast_queue_size),
          receive(receive_queue_size) {
    }

    MessageQueue exchange;
    MessageQueue broadcast;
    MessageQueue receive;
  };

  /**
   * @brief The main thread function for GDL.
   */
  void gdlThread();

  /**
   * @brief The configuration for the GDL instance.
   */
  const GdlConfig config_;

  /**
   * @brief The queues for the GDL instance.
   */
  MessageQueues queues_;

  /**
   * @brief The thread that runs GDL.
   */
  std::thread gdl_thread_{};

  /**
   * @brief The status of the GDL instance.
   */
  std::atomic<Status> status_{Status::STOPPED};

  /**
   * @brief The status of the connection.
   */
  std::atomic<ConnectionStatus> connection_status_{
      ConnectionStatus::DISCONNECTED};
};

} // namespace gdl

#endif /* GIRAFFE_DATA_LINK_HPP_ */