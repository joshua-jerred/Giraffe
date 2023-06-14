/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   simulated_extensions.hpp
 * @brief  This contains the simulated extensions for testing the
 * flight system.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-13
 * @copyright  2023 (license to be defined)
 */

#ifndef SIMULATED_EXTENSIONS_HPP_
#define SIMULATED_EXTENSIONS_HPP_

#include "extension.hpp"
#include <iostream>

namespace extension {
class SimTemperatureSensor : public Extension {
public:
  SimTemperatureSensor(ExtensionResources &resources,
                       cfg::ExtensionMetadata metadata)
      : Extension(resources, metadata) {
  }

  void startup() override {
  }

  void loop() override {
    data(data::DataId::ENVIRONMENTAL_temperature, temperature_);
    temperature_ += increasing_ ? 0.1 : -0.1;

    if (temperature_ > 30) {
      increasing_ = false;
    } else if (temperature_ < -30) {
      increasing_ = true;
    }
  }

  void shutdown() override {
  }

private:
  double temperature_ = 20.0;
  bool increasing_ = false;
};
}; // namespace extension

#endif /* SIMULATED_EXTENSIONS_HPP_ */