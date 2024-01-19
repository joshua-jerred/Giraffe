/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   physical_layer.cpp
 * @brief  GDL Physical Layer Implementation
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-18
 * @copyright  2024 (license to be defined)
 */

#include "gdl_config_and_stats.hpp"
#include "gdl_layers.hpp"

namespace giraffe::gdl {

PhysicalLayer::PhysicalLayer(Config &config) : config_(config) {
}

PhysicalLayer::~PhysicalLayer() {
}

void PhysicalLayer::enable() {
  state_ = State::IDLE;
}

void PhysicalLayer::disable() {
  state_ = State::DISABLED;
}

void PhysicalLayer::update() {
}

} // namespace giraffe::gdl