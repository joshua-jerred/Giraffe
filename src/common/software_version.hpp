/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   software_version.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2024-12-20
/// @copyright  2024 (license to be defined)

#pragma once

#include <string>

namespace giraffe {

struct SoftwareVersion {
  uint32_t major = 0;
  uint32_t minor = 0;
  uint32_t patch = 0;

  /// @brief Indicates if the software was built within a clean git repository.
  bool clean = false;
};

} // namespace giraffe