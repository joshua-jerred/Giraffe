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
#include <cstdint>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include <BoosterSeat/sleep.hpp>
#include <BoosterSeat/timer.hpp>
#include <SignalEasel/aprs.hpp>

#include "gdl_config_and_stats.hpp"
#include "gdl_message.hpp"

#include "layers/network_layer.hpp"
#include "layers/physical_layer.hpp"
#include "layers/transport_layer.hpp"

namespace giraffe::gdl {

/**
 * @brief The Giraffe Data Link core class/interface - Layer 4 (Application)
 */
class DataLink {
public:
  /**
   * @brief Status of the remote connection.
   */
  enum class Status { DISABLED, DISCONNECTED, CONNECTED };

  /**
   * @brief Constructor for the Giraffe Data Link.
   * @param config - The configuration for the GDL instance. Can be modified
   * externally. (thread safe)
   */
  DataLink(Config &config, PhysicalLayer &pyhsical_layer);

  /**
   * @brief Deconstruct the GDL instance, this will stop the GDL instance if it
   * is running.
   */
  ~DataLink();

  void enable();
  void disable();
  bool isEnabled() const {
    return !gdl_thread_stop_flag_;
  }
  DataLink::Status getStatus() const;

  bool sendMessage(const Message &message);
  bool sendText(const std::string &text, uint32_t message_id);

  bool receiveMessage(Message &message);
  bool messageAvailable() const {
    return in_queue_.size() > 0;
  }

  Statistics getStatistics() {
    std::lock_guard<std::mutex> lock(statistics_lock_);
    return statistics_;
  }

private:
  bool isRunning() {
    return status_ != Status::DISABLED;
  }

  /**
   * @brief The main thread function for GDL.
   */
  void gdlThread();
  Config &config_;

  MessageQueue out_broadcast_queue_{};
  MessageQueue out_exchange_queue_{};
  MessageQueue in_queue_{};

  std::thread gdl_thread_{};
  std::atomic<bool> gdl_thread_stop_flag_{true};
  std::atomic<Status> status_{Status::DISABLED};

  std::mutex statistics_lock_{};
  Statistics statistics_{};

  PhysicalLayer &physical_layer_;
  NetworkLayer network_layer_{config_, physical_layer_};
  TransportLayer transport_layer_{config_, network_layer_};
};

} // namespace giraffe::gdl

#endif /* GIRAFFE_DATA_LINK_HPP_ */