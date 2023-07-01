/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   to_string.hpp
 * @brief  Generic to_string functions for a variety of types.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-30
 * @copyright  2023 (license to be defined)
 */

#ifndef TO_STRING_HPP_
#define TO_STRING_HPP_

#include "streams.hpp"

namespace util {
std::string to_string(const data::LogPacket &packet);
std::string to_string(const data::LogPacket::Level &level);
}; // namespace util

#endif /* TO_STRING_HPP_ */