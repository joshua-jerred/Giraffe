/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   gdl_telemetry_sender.hpp
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-10-11
 * @copyright  2024 (license to be defined)
 */

#pragma once

#include <atomic>
#include <mutex>

#include <BoosterSeat/stopwatch.hpp>

#include <SignalEasel/aprs/telemetry_data.hpp>

#include "gdl_config_and_stats.hpp"
#include "gdl_message.hpp"
#include "i_giraffe_data_link.hpp"

namespace giraffe::gdl {

/// @brief Manages sending APRS telemetry data via the Data Link.
/// @warning This class is contained within the Data Link. It's updated from an
/// internal thread of the Data Link. Thought should be put into making sure
/// that this class is thread safe.
class AprsTelemetrySender {
public:
  using TelemetryData = signal_easel::aprs::telemetry::TelemetryData;

  /// @brief The status of the telemetry sender. Used for debugging/logging.
  enum class Status : uint8_t {
    UNKNOWN = 0,
    DISABLED,
    WAITING_FOR_DATA,
    SEND_DATA_DELAY,
    SENDING_DATA,
    METADATA_WAIT_FOR_INITIAL_DATA,
    SENDING_METADATA
  };

  /// @brief Convert the status enum to a string for debugging/logging.
  /// @param status - The status to convert.
  /// @return The status as a string.
  static constexpr std::string getStatusString(Status status) {
    switch (status) {
    case Status::UNKNOWN:
      return "Unknown";
    case Status::DISABLED:
      return "Disabled";
    case Status::WAITING_FOR_DATA:
      return "Waiting for Data";
    case Status::SEND_DATA_DELAY:
      return "Send Data Delay";
    case Status::SENDING_DATA:
      return "Sending Data Packet";
    case Status::METADATA_WAIT_FOR_INITIAL_DATA:
      return "Waiting for Initial Data to Send Metadata";
    case Status::SENDING_METADATA:
      return "Sending Metadata Packets";
    default:
      return "Unknown";
    }
  }

  /// @brief Constructor for the telemetry sender.
  /// @param gdl_config - The configuration for the GDL instance.
  AprsTelemetrySender(Config &gdl_config) : config_(gdl_config) {
    data_send_interval_stopwatch_.start();
  }

  /// @brief Default destructor.
  ~AprsTelemetrySender() = default;

  /// @brief Get the status of the telemetry sender. This is used for
  /// monitoring, debugging, and logging, etc. Not use internally.
  /// @return The current status.
  Status getStatus() const {
    return status_;
  }

  /// @brief Add new telemetry data to the telemetry sender. If the telemetry
  /// sender is enabled, the send interval has elapsed, and we are waiting for
  /// data, the telemetry data will be sent on the next update.
  /// @param telemetry_data - The telemetry data to send.
  void addNewData(const TelemetryData &telemetry_data) {
    std::lock_guard<std::mutex> lock(telemetry_data_lock_);
    telemetry_data_ = telemetry_data;
    initial_data_added_ = true;
    new_data_available_ = true;
  }

  /// @brief Ask the telemetry sender to send the parameter name, unit/label,
  /// coefficient, and bit sense messages.
  void sendTelemetryMetadata() {
    if (!config_.isAprsTelemetrySenderEnabled()) {
      /// @todo  Another place where an error could be reported.
      std::cout << "Telemetry sender is not enabled, can't send metadata"
                << std::endl;
      return;
    }

    // Don't interrupt the telemetry data sending process.
    if (metadata_send_state_ == MetadataSendState::NONE) {
      metadata_send_state_ = MetadataSendState::WAIT_FOR_INITIAL_DATA;
    }
  }

  /// @brief Periodic update function for the telemetry sender.
  void update(IDataLink &gdl) {
    if (!config_.isAprsTelemetrySenderEnabled()) {
      status_ = Status::DISABLED;
      return; // We're not enabled!
    }

    // Check to see if metadata was requested.
    if (metadata_send_state_ != MetadataSendState::NONE) {
      sendMetadataStateMachine(gdl);
      return;
    }

    if (!new_data_available_) {
      status_ = Status::WAITING_FOR_DATA;
      return; // We don't have new data to send
    }

    if (data_send_interval_stopwatch_.elapsedMilliseconds() <
        config_.getAprsTelemetryDataIntervalMs()) {
      status_ = Status::SEND_DATA_DELAY;
      return; // Not time to send yet
    }

    status_ = Status::SENDING_DATA;

    // Reset the interval stopwatch but don't stop it.
    constexpr bool STOP_ON_RESET = false;
    data_send_interval_stopwatch_.reset(STOP_ON_RESET);

    telemetry_data_lock_.lock();
    uint16_t sequence_number = getNextSequenceNumber();
    telemetry_data_.setSequenceNumber(sequence_number);
    bool sent = gdl.sendTelemetryData(telemetry_data_, sequence_number);
    telemetry_data_lock_.unlock();

    if (sent) {
      new_data_available_ = false;
      num_data_packets_added_to_queue_++;
    } else {
      /// @todo Really need a way to report errors in GDL...
      std::cout << "Failed to send telemetry data" << std::endl;
    }
  }

  /// @brief Get the number of data packets that have been added to the queue.
  /// @return The number of data packets added to the queue.
  uint32_t getNumDataPacketsAddedToQueue() const {
    return num_data_packets_added_to_queue_;
  }

private:
  /// @brief Get the next sequence number for the telemetry data packet.
  /// @return The next sequence number.
  uint16_t getNextSequenceNumber() {
    // Reset the sequence number if reached the maximum.
    if (sequence_number_ >= 999) {
      sequence_number_ = 0;
    }
    return sequence_number_++;
  }

  /// @brief State list for sending telemetry metadata.
  enum class MetadataSendState {
    NONE,
    WAIT_FOR_INITIAL_DATA,
    PARAMETER_NAMES,
    UNITS_LABELS,
    COEFFICIENTS,
    BIT_SENSE
  };
  /// @brief The current state of the telemetry metadata sending state machine.
  MetadataSendState metadata_send_state_ = MetadataSendState::NONE;

  /// @brief Handles sending telemetry metadata. Implemented with a simple state
  /// machine.
  void sendMetadataStateMachine(IDataLink &gdl) {
    std::lock_guard<std::mutex> lock(telemetry_data_lock_);

    status_ = Status::SENDING_METADATA;

    switch (metadata_send_state_) {
    case MetadataSendState::WAIT_FOR_INITIAL_DATA:
      if (initial_data_added_) {
        metadata_send_state_ = MetadataSendState::PARAMETER_NAMES;
      } else {
        status_ = Status::METADATA_WAIT_FOR_INITIAL_DATA;
      }
      break;
    case MetadataSendState::PARAMETER_NAMES:
      gdl.sendTelemetryParameterNames(telemetry_data_);
      metadata_send_state_ = MetadataSendState::UNITS_LABELS;
      break;
    case MetadataSendState::UNITS_LABELS:
      gdl.sendTelemetryUnitsAndLabels(telemetry_data_);
      metadata_send_state_ = MetadataSendState::COEFFICIENTS;
      break;
    case MetadataSendState::COEFFICIENTS:
      gdl.sendTelemetryCoefficients(telemetry_data_);
      metadata_send_state_ = MetadataSendState::BIT_SENSE;
      break;
    case MetadataSendState::BIT_SENSE:
      gdl.sendTelemetryBitSense(telemetry_data_);
      metadata_send_state_ = MetadataSendState::NONE;
      break;
    case MetadataSendState::NONE:
      break;
    }
  }

  /// @brief This flag indicates that a set of telemetry data was added to the
  /// sender. This must be true before telemetry metadata packets can be sent.
  std::atomic<bool> initial_data_added_ = false;

  /// @brief A flag to indicate that new data is available to send. If there is
  /// no new data, there is no reason to send a telemetry data packet.
  std::atomic<bool> new_data_available_ = false;

  /// @brief Mutex to protect the local telemetry data.
  std::mutex telemetry_data_lock_{};

  /// @brief The telemetry data to send.
  TelemetryData telemetry_data_{};

  /// @brief The sequence number for the telemetry data packet. Incremented for
  /// each packet sent. Range is 0-999.
  uint16_t sequence_number_ = 0;

  /// @brief Used to determine when to send telemetry data.
  /// @details A bst::Stopwatch was used instead of a bst::Timer because the
  /// configuration for the interval can be changed on the fly. The timer would
  /// need to expire before the new interval would take effect. If the timer
  /// was set to a very long interval this would be very annoying.
  bst::Stopwatch data_send_interval_stopwatch_;

  /// @brief General GDL configuration.
  Config &config_;

  // --- Debugging/Monitoring Data ---
  /// @brief The status of the telemetry sender. Used for debugging/logging.
  std::atomic<Status> status_ = Status::UNKNOWN;

  /// @brief The total number of data packets added to the queue. Reported in
  /// upper level GDL statistics.
  std::atomic<uint32_t> num_data_packets_added_to_queue_ = 0;
};

} // namespace giraffe::gdl