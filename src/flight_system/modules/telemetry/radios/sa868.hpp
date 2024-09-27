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

#include "radio.hpp"
namespace giraffe::radio {

class Sa868 : public Radio {
public:
  Sa868(giraffe::gdl::Config &config) : Radio(config) {
  }

  ~Sa868() override = default;

  void enable() override {
  }

  void disable() override {
  }

  void update() override {
  }
};

} // namespace giraffe::radio