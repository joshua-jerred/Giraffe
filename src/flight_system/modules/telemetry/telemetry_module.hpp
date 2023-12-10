/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   telemetry_module.hpp
 * @brief  The telemetry module.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-10-26
 * @copyright  2023 (license to be defined)
 */

#ifndef TELEMETRY_MODULE_HPP_
#define TELEMETRY_MODULE_HPP_

#include "module.hpp"

#include <gdl_network_layer_aprs.hpp>
#include <giraffe_data_link.hpp>
namespace modules {

static modules::MetaData metadata("telemetry_module",
                                  node::Identification::TELEMETRY_MODULE);

class TelemetryModule : public Module {
public:
  TelemetryModule(data::SharedData &shared_data, cfg::Configuration &config)
      : Module(metadata, shared_data, config) {
  }
  ~TelemetryModule() override = default;

private:
  void startup() override;
  void loop() override;
  void shutdown() override;
  void processCommand(const cmd::Command &command) {
    (void)command;
  }

  signal_easel::aprs::Packet base_packet_{
      .source_address = "N0CALL",
      .source_ssid = 0,
      .destination_address = "APZMWV",
      .destination_ssid = 0,
      .symbol_table = signal_easel::aprs::Packet::SymbolTable::PRIMARY,
      .symbol = '/'};
  gdl::GdlConfig gdl_config_{};
  gdl::PhysicalLayer physical_layer_{};
  gdl::AprsNetworkLayer network_layer_{physical_layer_, base_packet_};
  gdl::TransportLayer transport_layer_{network_layer_, gdl_config_};
  gdl::GiraffeDataLink gdl_{gdl_config_, transport_layer_};
};

} // namespace modules

#endif /* TELEMETRY_MODULE_HPP_ */