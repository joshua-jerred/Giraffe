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

  /// @todo Symbol table option
  // base_packet_.symbol_table = configuration_.telemetry_aprs.getSymbolTable();

  /// @warning this could be a problem
  base_packet_.symbol = configuration_.telemetry_aprs.getSymbol().at(0);
  gdl_.start();
}

void modules::TelemetryModule::loop() {
  if (gdl_.getReceiveQueueSize() > 0) {
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

  // update the telemetry module stats
  auto gdl_data = gdl_.getGdlStatus();

  data::blocks::TelemetryModuleStats stats{};
  stats.exchange_queue_size = gdl_data.exchange_queue_size;
  stats.broadcast_queue_size = gdl_data.broadcast_queue_size;
  stats.received_queue_size = gdl_data.received_queue_size;
  stats.network_layer_latency_ms = gdl_data.network_layer_latency_ms;
  stats.volume = gdl_data.volume;
  stats.signal_to_noise_ratio = gdl_data.signal_to_noise_ratio;
  stats.total_packets_received = total_packets_received_;
  stats.total_packets_sent = total_packets_sent_;
  stats.last_received_message = last_received_message_;

  shared_data_.blocks.telemetry_module_stats.set(stats);
}

void modules::TelemetryModule::shutdown() {
  gdl_.stop();
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