/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   gdl_config.hpp
 * @brief  Giraffe Data Link Configuration
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-18
 * @copyright  2024 (license to be defined)
 */

#ifndef GDL_CONFIG_HPP_
#define GDL_CONFIG_HPP_

#include <BoosterSeat/time.hpp>

#include <atomic>
#include <cstdint>
#include <mutex>
#include <string>

namespace giraffe::gdl {

/// @brief Data Link Configuration
class Config {
public:
  /**
   * @brief Constructor for GDL Configuration
   * @param is_controller - Is this GDL instance the controller or the remote?
   */
  Config(bool is_controller = true) : is_controller_(is_controller) {
  }

  ~Config() = default;

  void setCallSign(const std::string &call_sign) {
    std::lock_guard<std::mutex> lock(call_sign_lock_);
    call_sign_ = call_sign;
  }

  std::string getCallSign() {
    std::lock_guard<std::mutex> lock(call_sign_lock_);
    return call_sign_;
  }

  void setSSID(uint8_t ssid) {
    ssid_ = ssid;
  }

  uint8_t getSSID() const {
    return ssid_;
  }

  void setRemoteCallSign(const std::string &remote_call_sign) {
    std::lock_guard<std::mutex> lock(call_sign_lock_);
    remote_call_sign_ = remote_call_sign;
  }

  std::string getRemoteCallSign() {
    std::lock_guard<std::mutex> lock(call_sign_lock_);
    return remote_call_sign_;
  }

  void setRemoteSSID(uint8_t remote_ssid) {
    remote_ssid_ = remote_ssid;
  }

  void setAprsTelemetrySenderEnabled(bool enabled) {
    aprs_telemetry_sender_enabled_ = enabled;
  }

  void setAprsTelemetryDataIntervalMs(uint32_t interval_ms) {
    aprs_telemetry_data_interval_ms_ = interval_ms;
  }

  uint8_t getRemoteSSID() const {
    return remote_ssid_;
  }

  bool isController() const {
    return is_controller_;
  }

  void setProactiveKeepAlive(bool proactive_keep_alive) {
    proactive_keep_alive_ = proactive_keep_alive;
  }

  bool getProactiveKeepAlive() const {
    return proactive_keep_alive_;
  }

  bool isAprsTelemetrySenderEnabled() const {
    return aprs_telemetry_sender_enabled_;
  }

  uint32_t getAprsTelemetryDataIntervalMs() const {
    return aprs_telemetry_data_interval_ms_;
  }

private:
  bool is_controller_ = true;

  std::mutex call_sign_lock_{};

  std::string call_sign_ = "NOCALL";
  std::atomic<uint8_t> ssid_ = 0;
  std::string remote_call_sign_ = "NOCALL";
  std::atomic<uint8_t> remote_ssid_ = 0;

  std::atomic<bool> proactive_keep_alive_{false};

  // --- APRS Telemetry Sender Configuration ---
  /// @brief Set to true to enable the APRS telemetry sender.
  std::atomic<bool> aprs_telemetry_sender_enabled_{false};
  /// @brief The interval at which to send telemetry *data* packets. Default
  /// is 5 minutes.
  std::atomic<uint32_t> aprs_telemetry_data_interval_ms_{5 * 60 * 1000};
};

struct Statistics {
  // DataLink layer
  size_t exchange_queue_size = 0;
  size_t broadcast_queue_size = 0;
  size_t received_queue_size = 0;
  /// @brief The number of APRS telemetry data packets that have been added to
  /// the broadcast queue.
  uint32_t aprs_telemetry_data_packets_added = 0;

  // Transport layer
  bool uplink_connected = false;
  bool downlink_connected = false;
  bst::Time last_message_received{};
  uint32_t position_packets_received = 0;
  uint32_t telemetry_packets_received = 0;

  // Network layer
  uint32_t total_packets_sent = 0;
  uint32_t total_packets_received = 0;
  uint32_t total_messages_dropped = 0;
  double volume = 0.0;
  double signal_to_noise_ratio = 0.0;

  // Physical layer
  uint32_t physical_layer_rssi = 0;
  uint32_t physical_layer_state = 0;

  /**
   * @brief Audio processing latency in milliseconds
   */
  uint64_t network_layer_latency_ms = 0;
};

} // namespace giraffe::gdl

#endif /* GDL_CONFIG_HPP_ */