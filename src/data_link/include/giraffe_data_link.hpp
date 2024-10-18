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
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include <BoosterSeat/sleep.hpp>
#include <BoosterSeat/timer.hpp>
#include <SignalEasel/aprs.hpp>

#include "gdl_config_and_stats.hpp"
#include "gdl_message.hpp"
#include "gdl_telemetry_receiver.hpp"
#include "gdl_telemetry_sender.hpp"
#include "i_giraffe_data_link.hpp"

#include "layers/network_layer.hpp"
#include "layers/physical_layer.hpp"
#include "layers/transport_layer.hpp"

namespace giraffe::gdl {

class DataLink : public IDataLink {
public:
  /// @brief Status of a connection
  enum class Connection { DISABLED, DISCONNECTED, CONNECTED };

  /// @brief Data Link Status
  enum class Status { ERROR, DISABLED, STARTING, RUNNING, STOPPING };

  /// @brief Constructor for the Giraffe Data Link.
  /// @param config - The configuration for the GDL instance.
  /// @param p_physical_layer - The (optional) physical layer for the GDL. If
  /// not set, the physical layer needs to be set before the GDL can be enabled.
  DataLink(Config &config,
           std::shared_ptr<PhysicalLayer> p_physical_layer = nullptr);

  /// @brief Destructor for the Giraffe Data Link. Stops the background thread
  /// if it is running.
  ~DataLink();

  /// @brief Add a physical layer to the Data Link.
  /// @param p_physical_layer - A shared pointer to the physical layer.
  /// @return True if the physical layer was set, false otherwise. Will return
  /// false if the Data Link already has a physical layer set.
  bool setPhysicalLayer(std::shared_ptr<PhysicalLayer> p_physical_layer);

  /// @brief Enable the Data Link. Starts the background thread.
  /// @return True if the Data Link was enabled or is already enabled, false if
  /// the Data Link could not be enabled.
  bool enable();

  /// @brief Disable the Data Link. Stops the background thread.
  /// @return True if the Data Link was disabled or is already disabled, false
  /// if the Data Link could not be disabled.
  bool disable();

  /// @brief Check if the Data Link is enabled.
  bool isEnabled() const {
    return !gdl_thread_stop_flag_;
  }

  /// @brief Get the status of the Data Link.
  /// @return The status of the Data Link.
  DataLink::Status getStatus() const;

  /// @brief Send a message over the Data Link.
  /// @param message - The message to send.
  /// @return \c true if the message was added to the out queue, \c false
  /// otherwise.
  bool sendMessage(const Message &message);

  /// @brief Send a text message over the Data Link. (Exchange message)
  /// @param text - The text to send.
  /// @param message_id A unique identifier for the message.
  /// @return \c true if the message was added to the out queue, \c false
  /// otherwise.
  bool sendText(const std::string &text, uint32_t message_id);

  /// @brief Send telemetry data over the Data Link. (Broadcast message)
  /// @details This *overrides* the functionality of the APRS telemetry sender.
  /// This will immediately add the telemetry packet to the broadcast queue.
  /// This method is actually used by the telemetry sender to send telemetry.
  /// @param telemetry_data - The telemetry data to send.
  /// @return \c true if the message was added to the out queue, \c false
  /// otherwise.
  bool sendTelemetryData(TelemetryData &telemetry_data,
                         uint32_t message_id) override;

  /// @brief Send an APRS telemetry coefficient message over the Data Link.
  /// (Broadcast message)
  /// @details This *overrides* the functionality of the APRS telemetry sender.
  /// This will immediately add the telemetry packet to the broadcast queue.
  /// This method is actually used by the telemetry sender to send telemetry.
  /// @param telemetry_data - The telemetry data to send.
  /// @return \c true if the message was added to the out queue, \c false
  bool sendTelemetryCoefficients(TelemetryData &telemetry_data) override;

  /// @brief Send an APRS telemetry parameter names message over the Data Link.
  /// (Broadcast message)
  /// @details This *overrides* the functionality of the APRS telemetry sender.
  /// This will immediately add the telemetry packet to the broadcast queue.
  /// This method is actually used by the telemetry sender to send telemetry.
  /// @param telemetry_data - The telemetry data to send.
  /// @return \c true if the message was added to the out queue, \c false
  bool sendTelemetryParameterNames(TelemetryData &telemetry_data) override;

  /// @brief Send an APRS telemetry units and labels message over the Data Link.
  /// (Broadcast message)
  /// @details This *overrides* the functionality of the APRS telemetry sender.
  /// This will immediately add the telemetry packet to the broadcast queue.
  /// This method is actually used by the telemetry sender to send telemetry.
  /// @param telemetry_data - The telemetry data to send.
  /// @return \c true if the message was added to the out queue, \c false
  bool sendTelemetryUnitsAndLabels(TelemetryData &telemetry_data) override;

  /// @brief Send an APRS telemetry bit sense message over the Data Link.
  /// (Broadcast message)
  /// @details This *overrides* the functionality of the APRS telemetry sender.
  /// This will immediately add the telemetry packet to the broadcast queue.
  /// This method is actually used by the telemetry sender to send telemetry.
  /// @param telemetry_data - The telemetry data to send.
  /// @return \c true if the message was added to the out queue, \c false
  bool sendTelemetryBitSense(TelemetryData &telemetry_data) override;

  /// @brief Broadcast a text message over the Data Link.
  /// @param text - The text to send.
  /// @param message_id A unique identifier for the message.
  /// @return \c true if the message was added to the out queue, \c false
  /// otherwise.
  bool broadcastText(const std::string &text, uint32_t message_id);

  /// @brief Receive a message from the Data Link if one is available.
  /// @param message[out] - The received message.
  /// @return \c true if there was a message to receive and message was set,
  /// \c false otherwise.
  bool receiveMessage(Message &message);

  /// @brief Check if a messages have been received and are available.
  /// @return \c true if messages are available, \c false otherwise.
  bool messageAvailable() const {
    return in_queue_.size() > 0;
  }

  /// @brief Get statistics about the Data Link.
  /// @return GDL statistics.
  Statistics getStatistics() override {
    std::lock_guard<std::mutex> lock(statistics_lock_);
    return statistics_;
  }

  /// @brief Update the telemetry data for the data link telemetry sender.
  /// @details This does not send the telemetry data, it just updates the
  /// telemetry sender so that it can send it when it is ready.
  /// @param telemetry_data - The telemetry data to add.
  void updateTelemetryData(const TelemetryData &telemetry_data) override {
    if (config_.isController()) {
      giraffe_assert(false); // The controller has no need to add telemetry data
    }

    telemetry_sender_.addNewData(telemetry_data);
  }

  AprsTelemetrySender::Status getTelemetrySenderStatus() const {
    return telemetry_sender_.getStatus();
  }

  std::string getTelemetrySenderStatusString() const {
    return AprsTelemetrySender::getStatusString(getTelemetrySenderStatus());
  }

  void sendTelemetryMetadataPackets() {
    telemetry_sender_.sendTelemetryMetadata();
  }

  Config &getConfig() override {
    return config_;
  }

private:
  bool isRunning() {
    return gdl_thread_.joinable();
  }

  void setStatus(Status status) {
    status_ = status;
  }

  /// @brief The function that runs in the background thread.
  void gdlThread();

  /// @brief Thread safe GDL configuration.
  Config &config_;

  AprsTelemetryReceiver telemetry_receiver_{config_};

  AprsTelemetrySender telemetry_sender_{config_};

  /// @brief Outgoing broadcast message queue.
  MessageQueue out_broadcast_queue_{};
  /// @brief Outgoing exchange message queue.
  MessageQueue out_exchange_queue_{};
  /// @brief Received messages queue.
  MessageQueue in_queue_{};

  /// @brief The background thread for the GDL.
  std::thread gdl_thread_{};
  /// @brief Flag to stop the background thread.
  std::atomic<bool> gdl_thread_stop_flag_{true};
  /// @brief The status of the GDL.
  std::atomic<Status> status_{Status::DISABLED};

  /// @brief Lock for the statistics.
  std::mutex statistics_lock_{};
  /// @brief The statistics for the GDL.
  Statistics statistics_{};

  /// @brief The physical layer for the GDL.
  /// @details This is a shared pointer. PhysicalLayer is a base class that can
  /// be extended to create a custom physical layer.
  std::shared_ptr<PhysicalLayer> p_physical_layer_;
  /// @brief The network layer for the GDL.
  NetworkLayer network_layer_{config_, p_physical_layer_};
  /// @brief The transport layer for the GDL.
  TransportLayer transport_layer_{config_, network_layer_};
};

} // namespace giraffe::gdl

#endif /* GIRAFFE_DATA_LINK_HPP_ */