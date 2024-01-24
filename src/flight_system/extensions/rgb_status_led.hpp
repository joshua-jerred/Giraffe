/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   status_led.hpp
 * @brief  RGB Status LED extension header file.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-24
 * @copyright  2024 (license to be defined)
 */

#ifndef RGB_STATUS_LED_HPP_
#define RGB_STATUS_LED_HPP_

#include "extension_base.hpp"

namespace extension {

class RgbStatusLed : public Extension {
public:
  RgbStatusLed(ExtensionResources &resources, cfg::ExtensionMetadata metadata);

  void startup() override;
  void loop() override;

private:
};

} // namespace extension

#endif /* RGB_STATUS_LED_HPP_ */