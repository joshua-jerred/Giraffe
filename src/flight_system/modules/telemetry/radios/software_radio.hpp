/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   software_radio.hpp
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-09-18
 * @copyright  2024 (license to be defined)
 */

#pragma once

#include <iostream>
#include <string>

#include "radio.hpp"

namespace giraffe::radio {

class SoftwareRadio : public Radio {
public:
  SoftwareRadio(giraffe::gdl::Config &config) : Radio(config) {
  }

  ~SoftwareRadio() override = default;

  void enable() override {
    setState(State::IDLE);
    std::cout << "SoftwareRadio::enable" << std::endl;
  }

  void disable() override {
    std::cout << "SoftwareRadio::disable" << std::endl;
  }

  void update() override {
  }

  bool startTransmit() override {
    std::cout << "SoftwareRadio::startTransmit" << std::endl;
    return true;
  }

  bool stopTransmit() override {
    std::cout << "SoftwareRadio::stopTransmit" << std::endl;
    return true;
  }

  bool changeMode(Mode mode) override {
    setMode(mode);
    std::cout << "SoftwareRadio::changeMode | " +
                     std::to_string(static_cast<int>(mode))
              << std::endl;
    return true;
  }
};

} // namespace giraffe::radio