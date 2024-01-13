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
#include "gdl_status.hpp"
#include "gdl_transport_layer.hpp"

namespace gdl {
/**
 * @brief The Giraffe Data Link core class/interface - Layer 4 (Application)
 */
class GiraffeDataLink {
public:
  /**
   * @brief The status of the GDL thread.
   */
  enum class Status { ERROR, STOPPED, STARTING, RUNNING, STOPPING };

  /**
   * @brief Status of the remote connection.
   */
  enum class ConnectionStatus { DISCONNECTED, CONNECTED };

  /**
   * @brief Create a new Giraffe Data Link instance
   * @param config - The configuration for the GDL instance
   */
  GiraffeDataLink(GdlConfig config, TransportLayer transport_layer);

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
  ConnectionStatus getUplinkStatus() const {
    return uplink_status_;
  }

  ConnectionStatus getDownlinkStatus() const {
    return downlink_status_;
  }

  bool exchangeMessage(std::string message);

  bool broadcastMessage(std::string message);

  /**
   * @brief Get a message from the receive queue.
   *
   * @param message (out) - The message to get from the queue.
   * @return true - If a message was available.
   * @return false - If a message was not available.
   */
  bool getReceivedMessage(Message &message);

  int getExchangeQueueSize() const;
  int getBroadcastQueueSize() const;
  int getReceiveQueueSize() const;

  GdlStatus getGdlStatus() {
    std::lock_guard<std::mutex> lock(gdl_status_lock_);
    return gdl_status_;
  }

private:
  // PhysicalLayer physical_layer_;
  // NetworkLayer network_layer_;
  TransportLayer transport_layer_;

  std::string getNextMessageId();

  struct MessageQueues {
    MessageQueues(int exchange_queue_size, int broadcast_queue_size,
                  int receive_queue_size)
        : exchange(exchange_queue_size), broadcast(broadcast_queue_size),
          received(receive_queue_size) {
    }

    MessageQueue exchange;
    MessageQueue broadcast;
    MessageQueue received;
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
  std::atomic<ConnectionStatus> uplink_status_{ConnectionStatus::DISCONNECTED};
  std::atomic<ConnectionStatus> downlink_status_{
      ConnectionStatus::DISCONNECTED};

  std::mutex gdl_status_lock_{};
  GdlStatus gdl_status_{};
  uint16_t message_id_ = 0;
};

} // namespace gdl

#endif /* GIRAFFE_DATA_LINK_HPP_ */