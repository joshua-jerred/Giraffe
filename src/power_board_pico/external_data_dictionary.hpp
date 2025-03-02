/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   external_data_dictionary.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2025-03-02
/// @copyright  2025 (license to be defined)

#pragma once

#include <array>
#include <cstdint>

namespace power_board {

class DataDictionary {
public:
  enum class ENTRY_ID : uint16_t {
    UNKNOWN = 0,
    VERSION_INFO = 1,
    SYSTEM_STATUS = 2,
  };
};

} // namespace power_board