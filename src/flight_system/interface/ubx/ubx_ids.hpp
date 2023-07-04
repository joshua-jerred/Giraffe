/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   ubx_ids.hpp
 * @brief  Contains the UBX message/class IDs
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-04
 * @copyright  2023 (license to be defined)
 */

#ifndef UBX_IDS_HPP_
#define UBX_IDS_HPP_

#include <cstdint>

namespace ubx {
inline constexpr uint8_t kNavClass = 0x01;
inline constexpr uint8_t kNavPvt = 0x07;
} // namespace ubx

#endif /* UBX_IDS_HPP_ */