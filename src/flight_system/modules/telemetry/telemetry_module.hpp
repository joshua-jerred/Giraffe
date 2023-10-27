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
};

} // namespace modules

#endif /* TELEMETRY_MODULE_HPP_ */