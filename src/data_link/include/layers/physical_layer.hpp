/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   pysical_layer.hpp
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-08-14
 * @copyright  2024 (license to be defined)
 */

#pragma once

#include "gdl_config_and_stats.hpp"

namespace giraffe::gdl {

/// @brief The physical layer of the Giraffe Data Link
class PhysicalLayer {
public:
  enum class State {
    ERROR,
    DISABLED,
    STARTING,
    IDLE,
    TRANSMITTING,
    RECEIVING,
    BUSY
  };

  PhysicalLayer(const Config &config) : config_(config) {
  }

  virtual ~PhysicalLayer() = default;

  virtual void enable() = 0;

  virtual void disable() = 0;

  virtual void update() = 0;

  PhysicalLayer::State getState() const {
    return state_;
  }

protected:
  const Config &config_;

  State state_ = State::DISABLED;
};

} // namespace giraffe::gdl