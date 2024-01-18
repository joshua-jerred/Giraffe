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
#include "gdl_layers.hpp"
#include "gdl_message.hpp"

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
  DataLink(Config &config);

  /**
   * @brief Deconstruct the GDL instance, this will stop the GDL instance if it
   * is running.
   */
  ~DataLink();

  void enable();
  void disable();
  DataLink::Status getStatus() const;

  bool sendMessage(Message message);
  // bool receiveMessage(std::string &message);

  // bool broadcastMessage(std::string message);
  // bool receiveBroadcast(std::string &message);
  // bool broadcastLocation(double latitude, double longitude, uint32_t
  // altitude,
  //  double speed, int heading,
  //  std::string time_stamp = "");
  // bool receiveLocationBroadcast(double &latitude, double &longitude,
  // uint32_t &altitude, double &speed, int &heading,
  // std::string &time_stamp);

  Stats getGdlStats() {
    std::lock_guard<std::mutex> lock(gdl_status_lock_);
    return gdl_stats_;
  }

private:
  bool isRunning() {
    return status_ != Status::DISABLED;
  }

  /**
   * @brief The main thread function for GDL.
   */
  void gdlThread();

  /**
   * @brief The configuration for the GDL instance.
   */
  Config &config_;

  MessageQueue exchange_queue_{};
  MessageQueue broadcast_queue_{};
  MessageQueue received_queue_{};

  /**
   * @brief The thread that runs GDL.
   */
  std::thread gdl_thread_{};
  std::atomic<bool> gdl_thread_stop_flag_{true};

  /**
   * @brief The status of the GDL instance.
   */
  std::atomic<Status> status_{Status::DISABLED};

  std::mutex gdl_status_lock_{};
  Stats gdl_stats_{};

  TransportLayer transport_layer_{
      config_, NetworkLayer(config_, PhysicalLayer(config_))};
};

} // namespace giraffe::gdl

#endif /* GIRAFFE_DATA_LINK_HPP_ */