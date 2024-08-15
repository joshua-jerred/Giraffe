/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   software_physical_layer.hpp
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-08-14
 * @copyright  2024 (license to be defined)
 */

#pragma once

#include "layers/physical_layer.hpp"

namespace giraffe::gdl {

class SoftwarePhysicalLayer : public PhysicalLayer {
public:
  SoftwarePhysicalLayer(const Config &config) : PhysicalLayer(config) {
  }
  ~SoftwarePhysicalLayer() override = default;

  void enable() override {
  }

  void disable() override {
  }

  void update() override {
  }
};

inline std::shared_ptr<SoftwarePhysicalLayer>
createSwPhysicalLayer(const Config &config) {
  return std::make_shared<SoftwarePhysicalLayer>(config);
}

} // namespace giraffe::gdl
