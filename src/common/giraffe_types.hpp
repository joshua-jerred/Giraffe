/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   giraffe_types.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2024-12-20
/// @copyright  2024 (license to be defined)

#pragma once

#include <cstdint>

namespace giraffe {

/// @brief The applications of the Giraffe system.
/// @details This should not be assumed to always be in the same order, or be
/// complete.
enum class AppIdentifier : uint8_t {
  DAEMON = 0,       ///< giraffe_daemon
  COMMAND_LINE,     ///< gcli - giraffe_command_line
  FLIGHT_SYSTEM,    ///< gfs - giraffe_flight_system
  DATA_LINK_SERVER, ///< gdl_server - giraffe_data_link_server
  _COUNT,           ///< Number of Giraffe applications, must be last.
  UNKNOWN           ///< unknown application, must be last.
};

}; // namespace giraffe