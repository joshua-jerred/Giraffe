/**
 * @file error_stream.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Implementation of the error stream.
 * @date 2023-05-15
 * @copyright Copyright (c) 2023
 */

#include "streams.h"

std::ostream& operator<<(std::ostream& o, const data::ErrorStreamPacket& e) {
  o << "\tError Code: " << e.code << "\t Info: " << e.info << std::endl;
  return o;
}