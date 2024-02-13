/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   detect_fc_type.hpp
 * @brief  A simple function to detect the flight computer type.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-02-13
 * @copyright  2024 (license to be defined)
 */

#ifndef DETECT_FC_TYPE_HPP_
#define DETECT_FC_TYPE_HPP_

namespace giraffe {

struct FCInfo {
  enum class Architecture { UNKNOWN, ARM, X86, X86_64 };
  enum class Type { UNKNOWN, OTHER, PI_ZERO, PI_ZERO_2 };

  Architecture architecture = Architecture::UNKNOWN;
  Type type = Type::UNKNOWN;

  bool isRaspberryPi() const {
    return type == Type::PI_ZERO || type == Type::PI_ZERO_2;
  }
};

FCInfo getFlightComputerInfo();

} // namespace giraffe

#endif /* DETECT_FC_TYPE_HPP_ */