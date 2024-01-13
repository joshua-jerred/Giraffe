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

#include "telemetry_module.hpp"

void modules::TelemetryModule::startup() {
  base_packet_.source_address = configuration_.telemetry.getCallSign();
  base_packet_.source_ssid = configuration_.telemetry_aprs.getSsid();
  base_packet_.destination_address =
      configuration_.telemetry_aprs.getDestinationAddress();
  base_packet_.destination_ssid =
      configuration_.telemetry_aprs.getDestinationSsid();

  aprs_position_packet_timer_.setTimeout(
      configuration_.telemetry_aprs.getPositionPacketInterval() * 1000);

  /// @todo Symbol table option
  // base_packet_.symbol_table = configuration_.telemetry_aprs.getSymbolTable();

  /// @warning this could be a problem
  base_packet_.symbol = configuration_.telemetry_aprs.getSymbol().at(0);
  gdl_.start();
}

void modules::TelemetryModule::loop() {
  if (gdl_.getReceiveQueueSize() > 0) {
    processTelemetryMessageQueue();
  } else if (configuration_.telemetry_aprs.getPositionPackets()) {
    // if APRS position packets are enabled, handle them.
    sendAprsPositionPacket(); // This function will only send a packet if it is
                              // time to send one. (determined by the configured
                              // interval)
  }

  // update the telemetry module stats
  auto gdl_data = gdl_.getGdlStatus();

  data::blocks::TelemetryModuleStats stats{};
  stats.exchange_queue_size = gdl_data.exchange_queue_size;
  stats.broadcast_queue_size = gdl_data.broadcast_queue_size;
  stats.received_queue_size = gdl_data.received_queue_size;
  stats.aprs_tx_queue_size = gdl_.getAprsGpsTxQueueSize();
  stats.aprs_rx_queue_size = gdl_.getAprsGpsRxQueueSize();
  stats.network_layer_latency_ms = gdl_data.network_layer_latency_ms;
  stats.volume = gdl_data.volume;
  // stats.rssi = gdl_data.rssi;
  stats.signal_to_noise_ratio = gdl_data.signal_to_noise_ratio;
  stats.total_packets_received = total_packets_received_;
  stats.total_packets_sent = total_packets_sent_;
  stats.last_received_message = last_received_message_;

  shared_data_.blocks.telemetry_module_stats.set(stats);
}

void modules::TelemetryModule::shutdown() {
  gdl_.stop();
}

void modules::TelemetryModule::processTelemetryMessageQueue() {
  // Read the packet
  gdl::Message msg;
  bool res = gdl_.getReceivedMessage(msg);
  if (!res) {
    // @todo log error
    return;
  }
  last_received_message_ = msg.data;
  data(data::DataId::TELEMETRY_dataLinkPacketReceived,
       msg.data + " [id: " + msg.id + "]");

  total_packets_received_++;

  cmd::Command command;
  if (!cmd::parseCommandString(msg.data, command)) {
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
  auto loc = position.last_gps_frame;
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
  signal_easel::aprs::PositionPacket packet{};
  /**
   * @todo Fix this time code!
   * @warning Time code in not implemented for APRS position packets.
   */
  packet.time_code = "000000";
  packet.latitude = loc.latitude;
  packet.longitude = loc.longitude;
  packet.altitude = loc.altitude;
  packet.speed = loc.horizontal_speed;
  packet.course = loc.heading_of_motion;

  // Send the packet
  if (!gdl_.broadcastAprsLocation(packet)) {
    /// @todo log an error
    return;
  }

  aprs_position_packet_timer_.reset();
}

void modules::TelemetryModule::processCommand(const cmd::Command &command) {
  int int_buffer = 0;
  double double_buffer = 0.0;
  switch (command.command_id) {
  case cmd::CommandId::TELEMETRY_MODULE_sendNumActiveErrors:
    int_buffer = shared_data_.frames.error_frame.numActiveErrors();
    gdl_.exchangeMessage("cmd/tlm/nae/ : " + std::to_string(int_buffer));
    break;
  case cmd::CommandId::TELEMETRY_MODULE_sendRssi:
    std::cout << "TELEMETRY_MODULE_sendRssi" << std::endl;
    double_buffer = shared_data_.blocks.telemetry_module_stats.get().rssi;
    gdl_.exchangeMessage("cmd/tlm/rsi/ : " + std::to_string(double_buffer));
    break;
  case cmd::CommandId::TELEMETRY_MODULE_sendSnr:
    std::cout << "TELEMETRY_MODULE_sendSnr" << std::endl;
    double_buffer =
        shared_data_.blocks.telemetry_module_stats.get().signal_to_noise_ratio;
    gdl_.exchangeMessage("cmd/tlm/snr/ : " + std::to_string(double_buffer));
    break;
  default:
    error(DiagnosticId::TELEMETRY_invalidCommand);
    break;
  }
}