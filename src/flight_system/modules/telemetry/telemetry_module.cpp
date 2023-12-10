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

  // update the telemetry module stats
  auto gdl_data = gdl_.getGdlStatus();
  data::blocks::TelemetryModuleStats stats{};
  stats.exchange_queue_size = gdl_data.exchange_queue_size;
  stats.broadcast_queue_size = gdl_data.broadcast_queue_size;
  stats.received_queue_size = gdl_data.received_queue_size;
  stats.network_layer_latency_ms = gdl_data.network_layer_latency_ms;
  shared_data_.blocks.telemetry_module_stats.set(stats);
}

void modules::TelemetryModule::shutdown() {
  gdl_.stop();
}