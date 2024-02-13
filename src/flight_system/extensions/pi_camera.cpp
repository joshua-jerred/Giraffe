/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   pi_camera.cpp
 * @brief  Raspberry Pi Camera Extension
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-02-13
 * @copyright  2024 (license to be defined)
 */

#include "pi_camera.hpp"

#include "flight_computer_info.hpp"

#include <BoosterSeat/process.hpp>

namespace extension {

PiCamera::PiCamera(ExtensionResources &resources,
                   cfg::ExtensionMetadata metadata)
    : Extension(resources, metadata) {
}

void PiCamera::startup() {
  // detect if the fc is a raspberry pi
  auto fc_info = giraffe::getFlightComputerInfo();
  if (!fc_info.isRaspberryPi()) {
    raiseFault(DiagnosticId::EXT_FAULT_piCameraNotOnPi);
    return;
  }

  // detect if the camera is connected
}

void PiCamera::loop() {
}

void PiCamera::shutdown() {
}

} // namespace extension