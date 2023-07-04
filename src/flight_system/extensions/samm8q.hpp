/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   samm8q.hpp
 * @brief  SAM-M8Q GPS extension (I2C).
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-04
 * @copyright  2023 (license to be defined)
 */

#ifndef SAMM8Q_HPP_
#define SAMM8Q_HPP_

#include "extension.hpp"
#include "i2c_interface.hpp"
#include "positional.hpp"

namespace extension {
class SamM8qExtension : public Extension {
public:
  SamM8qExtension(ExtensionResources &resources,
                  cfg::ExtensionMetadata metadata);

  void startup() override;
  void loop() override;
  void shutdown() override;

private:
  bool handshake();
  bool configure();

  I2cInterface i2c_;
  data::GpsFrame gps_frame_;
  BoosterSeat::Timer read_timer_;
};
} // namespace extension

#endif /* SAMM8Q_HPP_ */