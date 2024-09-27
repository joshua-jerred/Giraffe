/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   telemetry_module.cpp
 * @brief  The telemetry module.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-10-26
 * @copyright  2023 (license to be defined)
 */

#include <BoosterSeat/filesystem.hpp>

#include "gdl_message.hpp"
#include "telemetry_module.hpp"

#include "radios/sa868.hpp"
#include "radios/software_radio.hpp"

using namespace giraffe;

void modules::TelemetryModule::startup() {
  gdl_config_.setCallSign(configuration_.telemetry.getCallSign());
  gdl_config_.setSSID(configuration_.telemetry_aprs.getSsid());
  gdl_config_.setRemoteCallSign(
      configuration_.telemetry_aprs.getDestinationAddress());
  gdl_config_.setRemoteSSID(configuration_.telemetry_aprs.getDestinationSsid());

  gdl_config_.setRemoteCallSign(
      configuration_.telemetry_aprs.getDestinationAddress());
  gdl_config_.setRemoteSSID(configuration_.telemetry_aprs.getDestinationSsid());

  aprs_position_packet_timer_.setTimeout(
      configuration_.telemetry_aprs.getPositionPacketInterval() * 1000);

  const auto radio_type = configuration_.telemetry.getRadioType();
  switch (radio_type) {
  case cfg::gEnum::RadioType::SOFTWARE:
    physical_layer_ = std::make_unique<radio::SoftwareRadio>(gdl_config_);
    break;
  case cfg::gEnum::RadioType::SA868:
    physical_layer_ = std::make_unique<radio::Sa868>(gdl_config_);
    break;
  default:
    error(DiagnosticId::TELEMETRY_radioConfiguration, "0");
    break;
  }

  if (!gdl_.setPhysicalLayer(physical_layer_)) {
    error(DiagnosticId::TELEMETRY_dataLinkError, "stPhy");
    return;
  }

  if (!gdl_.enable()) {
    error(DiagnosticId::TELEMETRY_dataLinkError, "enbl");
    return;
  }
}

void modules::TelemetryModule::loop() {
  if (gdl_.messageAvailable()) {
    std::cout << "Message available" << std::endl;
    processTelemetryMessageQueue();
  } else if (configuration_.telemetry_aprs.getPositionPackets()) {
    // if APRS position packets are enabled, handle them.
    sendAprsPositionPacket(); // This function will only send a packet if it is
                              // time to send one. (determined by the configured
                              // interval)
  }

  // update the telemetry module stats
  auto gdl_data = gdl_.getStatistics();

  data::blocks::TelemetryModuleStats stats{};
  stats.exchange_queue_size = gdl_data.exchange_queue_size;
  stats.broadcast_queue_size = gdl_data.broadcast_queue_size;
  stats.received_queue_size = gdl_data.received_queue_size;
  stats.downlink_connected = gdl_data.downlink_connected;
  stats.network_layer_latency_ms = gdl_data.network_layer_latency_ms;
  stats.volume = gdl_data.volume;
  // stats.rssi = gdl_data.rssi;
  stats.signal_to_noise_ratio = gdl_data.signal_to_noise_ratio;
  stats.total_packets_received = gdl_data.total_packets_received;
  stats.total_packets_sent = gdl_data.total_packets_sent;
  stats.total_messages_dropped = gdl_data.total_messages_dropped;
  stats.total_messages_sent = total_messages_sent_;
  stats.total_messages_received = total_messages_received_;
  stats.last_received_message = last_received_message_;

  shared_data_.blocks.telemetry_module_stats.set(stats);
}

void modules::TelemetryModule::shutdown() {
  gdl_.disable();
}

void modules::TelemetryModule::processTelemetryMessageQueue() {
  // Read the packet
  giraffe::gdl::Message msg;
  if (!gdl_.receiveMessage(msg)) {
    // @todo log error
    std::cout << "message should be available" << std::endl;
    return;
  }

  if (msg.getType() == giraffe::gdl::Message::Type::LOCATION) {
    // @todo log error
    std::cout << "message should not be a location packet" << std::endl;
    return;
  }

  last_received_message_ = msg.getData();

  data(data::DataId::TELEMETRY_dataLinkPacketReceived,
       msg.getData() + " [id: " + msg.getIdentifierString() + "]");

  total_messages_received_++;

  cmd::Command command;
  if (!cmd::parseCommandString(msg.getData(), command)) {
    /// @todo log an error and respond to the telemetry packet
    return;
  }
  shared_data_.streams.command.addCommand(
      node::Identification::TELEMETRY_MODULE, command);
}

void modules::TelemetryModule::sendAprsPositionPacket() {
  if (!aprs_position_packet_timer_.isDone()) {
    return;
  }

  // Get the current position
  auto position = shared_data_.blocks.location_data.get();
  if (!position.have_gps_source) {
    /// @todo log an error
    return;
  }

  /// @todo Maybe use the last valid GPS frame instead, but first there needs to
  /// be a time stamp as to when this one was generated. I have a fear of DST
  /// and time zones causing issues here.
  auto loc = position.last_valid_gps_frame;
  if (!loc.is_valid) {
    /// @todo log an error
    return;
  }

  if (loc.fix == data::GpsFix::NO_FIX || loc.fix == data::GpsFix::ERROR) {
    /// @todo log an error
    return;
  }

  if (loc.fix == data::GpsFix::FIX_2D) {
    /// @todo log a warning but maybe still send the packet?
  }

  // Create the APRS packet
  gdl::Message::Location gdl_loc{};
  /**
   * @todo Fix this time code!
   * @warning Time code in not implemented for APRS position packets.
   */
  gdl_loc.time_code = "000000";
  gdl_loc.latitude = loc.latitude;
  gdl_loc.longitude = loc.longitude;
  gdl_loc.altitude = loc.altitude;
  gdl_loc.speed = loc.horizontal_speed; // @todo Convert to mph
  gdl_loc.heading = loc.heading_of_motion;

  gdl::Message message{};
  message.setLocation(gdl_loc);
  message.setType(gdl::Message::Type::LOCATION);
  message.setIdentifier(getNextMessageId());

  // Send the packet
  if (!gdl_.sendMessage(message)) {
    error(DiagnosticId::TELEMETRY_dataLinkSendError, "APRS");
    return;
  }

  aprs_position_packet_timer_.reset();
}

void modules::TelemetryModule::sendSstvImage() {
  if (!configuration_.telemetry_sstv.getEnabled()) {
    error(DiagnosticId::TELEMETRY_modeNotEnabled, "SSTV");
    return;
  }

  auto camera_block = shared_data_.blocks.camera.get();
  if (!camera_block.have_camera_source) {
    error(DiagnosticId::TELEMETRY_sstvError, "cam_src");
    return;
  } else if (camera_block.num_images <= 0) {
    error(DiagnosticId::TELEMETRY_sstvError, "num_img");
    return;
  }

  // Get the image and verify that it exists
  const std::string &image_path = camera_block.last_valid_image_path;
  try {
    if (!bst::filesystem::doesFileExist(image_path)) {
      error(DiagnosticId::TELEMETRY_sstvError, "img_pth");
      return;
    }
  } catch (const std::exception &e) {
    error(DiagnosticId::TELEMETRY_sstvError, "img_pth_expt");
    return;
  }

  giraffe::gdl::Message::Image image{};
  image.path = image_path;
  giraffe::gdl::Message message{};
  message.setImageMessage(image, getNextMessageId());

  if (!gdl_.sendMessage(message)) {
    error(DiagnosticId::TELEMETRY_dataLinkSendError, "SSTV");
    return;
  }
}

void modules::TelemetryModule::reportDescent() {
  if (configuration_.telemetry.getDataLinkEnabled()) {
    /// @todo Capture the return values of sendText and broadcastText and set a
    /// error if they fail.

    // Attempt exchange
    gdl_.sendText("Descent Detected", getNextMessageId());
    // Also broadcast
    gdl_.broadcastText("Descent Detected", getNextMessageId());
    gdl::Message message{};
  }
  /// @todo report an error
}

void modules::TelemetryModule::processCommand(const cmd::Command &command) {
  int int_buffer = 0;
  double double_buffer = 0.0;
  switch (command.command_id) {
  case cmd::CommandId::TELEMETRY_MODULE_sendNumActiveErrors:
    int_buffer = shared_data_.frames.error_frame.numActiveErrors();
    gdl_.sendText("cmd/tlm/nae/ : " + std::to_string(int_buffer),
                  getNextMessageId());
    break;
  case cmd::CommandId::TELEMETRY_MODULE_sendRssi:
    std::cout << "TELEMETRY_MODULE_sendRssi" << std::endl;
    double_buffer = shared_data_.blocks.telemetry_module_stats.get().rssi;
    gdl_.sendText("cmd/tlm/rsi/ : " + std::to_string(double_buffer),
                  getNextMessageId());
    break;
  case cmd::CommandId::TELEMETRY_MODULE_sendSnr:
    std::cout << "TELEMETRY_MODULE_sendSnr" << std::endl;
    double_buffer =
        shared_data_.blocks.telemetry_module_stats.get().signal_to_noise_ratio;
    gdl_.sendText("cmd/tlm/snr/ : " + std::to_string(double_buffer),
                  getNextMessageId());
    break;
  case cmd::CommandId::TELEMETRY_MODULE_sendSstvImage:
    sendSstvImage();
    break;
  case cmd::CommandId::INTERNAL_reportDescent:
    reportDescent();
    break;
  default:
    error(DiagnosticId::TELEMETRY_invalidCommand);
    break;
  }
}