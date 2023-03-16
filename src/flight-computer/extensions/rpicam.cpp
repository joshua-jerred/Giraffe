/**
 * @file rpicam.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief
 * @date 2023-02-25
 * @copyright Copyright (c) 2023
 * @version 0.4
 */

#include "configurables.h"
#include "extensions.h"

extension::RaspPiCamera::RaspPiCamera(DataStream *p_data_stream,
                                      ExtensionMetadata extension_metadata)
    : Extension(p_data_stream, extension_metadata) {}

extension::RaspPiCamera::~RaspPiCamera() {}

int extension::RaspPiCamera::runner() {
  setStatus(ExtensionStatus::RUNNING);
  while (!stop_flag_) {
    sendData("NEW_IMAGE", "test.png");
    std::this_thread::sleep_for(std::chrono::milliseconds(getUpdateInterval()));
  }
  setStatus(ExtensionStatus::STOPPED);
  return 0;
}