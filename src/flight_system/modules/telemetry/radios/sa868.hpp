/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   datalink_radio.hpp
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-08-14
 * @copyright  2024 (license to be defined)
 */

#pragma once

#include "layers/physical_layer.hpp"

namespace radios {

class Sa868 : public giraffe::gdl::PhysicalLayer {
public:
  Sa868(giraffe::gdl::Config &config) : PhysicalLayer(config) {
  }

  ~Sa868() override = default;

  void enable() override {
    std::cout << "Sa868 enabled\n";
  }

  void disable() override {
    std::cout << "Sa868 disabled\n";
  }

  void update() override {
    std::cout << "Sa868 update\n";
  }
};

} // namespace radios