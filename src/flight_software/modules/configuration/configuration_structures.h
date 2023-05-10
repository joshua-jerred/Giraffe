/**
 * @file config_structure.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief
 * @date 2023-05-09
 * @copyright Copyright (c) 2023
 */

#ifndef CONFIGURATION_STRUCTURES_H_
#define CONFIGURATION_STRUCTURES_H_

#include <string>
#include <unordered_map>
#include <vector>

namespace cfg {

struct General {
  enum class MainBoard {
    OTHER,
    PI_ZERO_W_2,
    PI_4,
  };

  std::string project_name;
  MainBoard main_board_type;
};

// namespace flight_mode { // cfg::flight_modes

// struct Procedure {
// };

// enum class Type {
//   TESTING,
//   ASCENT,
//   DESCENT,
//   RECOVERY,
//   FAILSAFE,
//   OTHER
// };

// struct Services {
//   bool data_logging;

//   bool telem_data_log;
//   bool telem_data_packet;
//   bool telem_sstv;
//   bool telem_aprs_location;
//   bool telem_aprs_telemetry;

//   bool image_capture;
//   bool health_check;
// };

// bool enabled;
// Type type;
// std::string name;

// };  // namespace flight_mode
};  // namespace cfg
#endif