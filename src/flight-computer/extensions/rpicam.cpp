/**
 * @file rpicam.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief 
 * @date 2023-02-25
 * @copyright Copyright (c) 2023
 * @version 0.4
 */

#include "extensions.h"

extension::RaspPiCamera::RaspPiCamera(DataStream *p_data_stream,
                        ExtensionMetadata extension_metadata)
    : Extension(p_data_stream, extension_metadata) {

}

extension::RaspPiCamera::~RaspPiCamera() {

}

int extension::RaspPiCamera::runner() {
  return 0;
}