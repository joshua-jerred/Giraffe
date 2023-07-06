/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   ds18b20.hpp
 * @brief  DS18B20 extension header file.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-03
 * @copyright  2023 (license to be defined)
 */

#ifndef DS18B20_HPP_
#define DS18B20_HPP_

#include <BoosterSeat/timer.hpp>

#include "extension_base.hpp"
#include "one_wire_interface.hpp"

namespace extension {

class Ds18b20Extension : public Extension {
public:
  Ds18b20Extension(ExtensionResources &resources,
                   cfg::ExtensionMetadata metadata);

  void startup() override;
  void loop() override;

private:
  bool init(DiagnosticId &fault);
  bool readData(DiagnosticId &fault);

  BoosterSeat::Timer init_timer_;
  BoosterSeat::Timer read_timer_;

  OneWireInterface device_{};
  double temperature_ = 0.0; // degrees C

  DiagnosticId read_timeout_fault_ = DiagnosticId::EXT_FAULT_ds18b20ReadTimeout;
};

} // namespace extension

#endif /* DS18B20_HPP_ */