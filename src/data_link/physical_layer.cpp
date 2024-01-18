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

#include "giraffe_data_link.hpp"

namespace giraffe {

GiraffeDataLink::PhysicalLayer::PhysicalLayer() {
}

GiraffeDataLink::PhysicalLayer::~PhysicalLayer() {
}

void GiraffeDataLink::PhysicalLayer::enable() {
  state_ = State::IDLE;
}

void GiraffeDataLink::PhysicalLayer::disable() {
  state_ = State::DISABLED;
}

void GiraffeDataLink::PhysicalLayer::update() {
}

} // namespace giraffe