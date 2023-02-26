/**
 * @file sa868.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Driver for the SA868
 * @date 2023-02-25
 * @copyright Copyright (c) 2023
 * @version 0.4
 */

#include "extensions.h"

extension::SA868::SA868(DataStream *p_data_stream,
                        ExtensionMetadata extension_metadata)
    : Extension(p_data_stream, extension_metadata) {

}

extension::SA868::~SA868() {

}

int extension::SA868::runner() {
  setStatus(ExtensionStatus::STOPPED_ERROR_STATE);
  return 0;
}