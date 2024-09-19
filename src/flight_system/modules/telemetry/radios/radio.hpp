/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   radio.hpp
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-09-18
 * @copyright  2024 (license to be defined)
 */

#pragma once

#include <cstdint>

#include "layers/physical_layer.hpp"

namespace giraffe::radio {

class Radio : public giraffe::gdl::PhysicalLayer {
public:
  using State = giraffe::gdl::PhysicalLayer::State;

  Radio(giraffe::gdl::Config &config) : PhysicalLayer(config) {
  }

  virtual ~Radio() = default;

  void enable() override {
    setState(State::IDLE);
  }

  void disable() override {
    setState(State::DISABLED);
  }

  void update() override {
  }

  bool startTransmit() override {
    setState(State::TRANSMITTING);
    return true;
  }

  bool stopTransmit() override {
    setState(State::IDLE);
    return true;
  }

  double getRssi() const override {
    return 0.1;
  }

  bool changeMode(Mode mode) override {
    setMode(mode);
    return true;
  }
};

} // namespace giraffe::radio