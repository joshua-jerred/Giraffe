/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   rgb_status_led.cpp
 * @brief  RGB Status LED extension source file.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-24
 * @copyright  2024 (license to be defined)
 */

#include "rgb_status_led.hpp"

namespace extension {

RgbStatusLed::RgbStatusLed(ExtensionResources &resources,
                           cfg::ExtensionMetadata metadata)
    : Extension(resources, metadata) {
}

void RgbStatusLed::startup() {
}

void RgbStatusLed::loop() {
}

} // namespace extension