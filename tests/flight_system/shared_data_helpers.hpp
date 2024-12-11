/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   fake_shared_data.hpp
 * @brief  Helper functions for setting shared data to known states when
 * testing.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-09-10
 * @copyright  2024 (license to be defined)
 */

#pragma once

#include "shared_data.hpp"
#include "to_string.hpp"

class FakeSharedData : public data::SharedData {
public:
  FakeSharedData() : SharedData() {
  }

  bool hasLogItems() {
    return streams.log.getNumPackets() > 0;
  }

  /// @brief Check if the next log item in the log stream has the given id.
  /// Clears packets until it finds one.
  /// @param id - The id to check for.
  /// @return \c true if the first log item has the given id. \c false
  /// otherwise.
  bool hasLogItem(DiagnosticId id) {
    data::LogPacket packet;
    while (streams.log.getPacket(packet)) {
      if (packet.id == id) {
        return true;
      } else {
        return false;
      }
    }
    return false;
  };

  bool hasDataItems() {
    return streams.data.getNumPackets() > 0;
  }

  bool hasDataItem(data::DataId id) {
    data::DataPacket packet;
    while (streams.data.getPacket(packet)) {
      if (packet.identifier == id) {
        return true;
      }
    }
    return false;
  }

  std::string dumpLog() {
    std::string log;
    data::LogPacket packet;
    while (streams.log.getPacket(packet)) {
      log += util::to_string(packet) + "\n";
    }
    return log;
  }
};

inline void setLocationDataValid(data::SharedData &shared) {
  const data::blocks::LocationData LOC_DATA{
      .have_gps_source = true,
      .current_gps_fix = data::GpsFix::FIX_3D,
      .last_valid_gps_fix = data::GpsFix::FIX_3D,
      .last_valid_gps_frame =
          data::GpsFrame{
              .gps_utc_time = bst::clck::TimePoint(),
              .is_valid = true,
              .fix = data::GpsFix::FIX_3D,
              .num_satellites = 10,
              .horz_accuracy = 0.1,
              .altitude = 0.0,
              .vert_accuracy = 0.0,
              .vertical_speed = 0.0,
              .horizontal_speed = 0.0,
              .speed_accuracy = 0.0,
              .heading_accuracy = 2.0,
          },
  };

  shared.blocks.location_data.set(LOC_DATA);
}

inline void setLocationDataInvalid(data::SharedData &shared) {
  const data::blocks::LocationData LOC_DATA{
      .have_gps_source = false,
      .current_gps_fix = data::GpsFix::NO_FIX,
      .last_valid_gps_fix = data::GpsFix::NO_FIX,
      .last_valid_gps_frame =
          data::GpsFrame{
              .gps_utc_time = bst::clck::TimePoint(),
              .is_valid = false,
              .fix = data::GpsFix::NO_FIX,
              .num_satellites = 10,
              .horz_accuracy = 0.1,
              .altitude = 0.0,
              .vert_accuracy = 0.0,
              .vertical_speed = 0.0,
              .horizontal_speed = 0.0,
              .speed_accuracy = 0.0,
              .heading_accuracy = 11.0,
          },
  };

  shared.blocks.location_data.set(LOC_DATA);
}

inline void setLocationDataLaunch(data::SharedData &shared) {
  const data::blocks::LocationData LOC_DATA{
      .have_gps_source = true,
      .current_gps_fix = data::GpsFix::FIX_3D,
      .last_valid_gps_fix = data::GpsFix::FIX_3D,
      .last_valid_gps_frame =
          data::GpsFrame{
              .gps_utc_time = bst::clck::TimePoint(),
              .is_valid = true,
              .fix = data::GpsFix::FIX_3D,
              .num_satellites = 10,
              .horz_accuracy = 0.1,
              .altitude = 0.0,
              .vert_accuracy = 0.0,
              .vertical_speed = 0.0,
              .horizontal_speed = 0.0,
              .speed_accuracy = 0.0,
              .heading_accuracy = 2.0,
          },
  };

  shared.blocks.location_data.set(LOC_DATA);
}

inline void setLocationDataAscent(data::SharedData &shared) {
  const data::blocks::LocationData LOC_DATA{
      .have_gps_source = true,
      .current_gps_fix = data::GpsFix::FIX_3D,
      .last_valid_gps_fix = data::GpsFix::FIX_3D,
      .last_valid_gps_frame =
          data::GpsFrame{
              .gps_utc_time = bst::clck::TimePoint(),
              .is_valid = true,
              .fix = data::GpsFix::FIX_3D,
              .num_satellites = 10,
              .horz_accuracy = 0.1,
              .altitude = 1000.0,
              .vert_accuracy = 0.0,
              .vertical_speed = 2.0,
              .horizontal_speed = 4.0,
              .speed_accuracy = 0.0,
              .heading_accuracy = 2.0,
          },
  };

  shared.blocks.location_data.set(LOC_DATA);
}

inline void setLocationDataDescent(data::SharedData &shared) {
  const data::blocks::LocationData LOC_DATA{
      .have_gps_source = true,
      .current_gps_fix = data::GpsFix::FIX_3D,
      .last_valid_gps_fix = data::GpsFix::FIX_3D,
      .last_valid_gps_frame =
          data::GpsFrame{
              .gps_utc_time = bst::clck::TimePoint(),
              .is_valid = true,
              .fix = data::GpsFix::FIX_3D,
              .num_satellites = 10,
              .horz_accuracy = 0.1,
              .altitude = 1000.0,
              .vert_accuracy = 0.0,
              .vertical_speed = -2.0,
              .horizontal_speed = 4.0,
              .speed_accuracy = 0.0,
              .heading_accuracy = 2.0,
          },
  };

  shared.blocks.location_data.set(LOC_DATA);
}

inline void setLocationDataRecovery(data::SharedData &shared) {
  const data::blocks::LocationData LOC_DATA{
      .have_gps_source = true,
      .current_gps_fix = data::GpsFix::FIX_3D,
      .last_valid_gps_fix = data::GpsFix::FIX_3D,
      .last_valid_gps_frame =
          data::GpsFrame{
              .gps_utc_time = bst::clck::TimePoint(),
              .is_valid = true,
              .fix = data::GpsFix::FIX_3D,
              .num_satellites = 10,
              .horz_accuracy = 0.1,
              .altitude = 0.0,
              .vert_accuracy = 0.0,
              .vertical_speed = 0.1,
              .horizontal_speed = 4.0, // someone is walking away with it
              .speed_accuracy = 0.0,
              .heading_accuracy = 2.0,
          },
  };

  shared.blocks.location_data.set(LOC_DATA);
}

inline void setCalculatedDataValid(data::SharedData &shared) {
  data::blocks::CalculatedData CALC_DATA{
      .pressure_altitude_m = 0,
      .pressure_altitude_valid = true,
      .pressure_vertical_speed = 4,
      .pressure_vertical_speed_valid = true,
      .pressure_vertical_speed_mps_1min = 5,
      .pressure_vertical_speed_1min_valid = true,
      .distance_traveled_m = 0,
      .distance_traveled_valid = true,
      .distance_from_launch_m = 0,
      .distance_from_launch_valid = true,
      .distance_from_ground_m = 0,
      .distance_from_ground_valid = true,
      .average_speed_valid = true,
      .average_horiz_speed_mps_1min = 0,
      .average_vert_speed_mps_1min = 0,
      .max_horizontal_speed_mps = 0,
      .max_vertical_speed_mps = 0,
      .max_speed_valid = true,
  };

  shared.blocks.calculated_data.set(CALC_DATA);
}

inline void setCalculatedDataInvalid(data::SharedData &shared) {
  data::blocks::CalculatedData CALC_DATA{
      .pressure_altitude_m = 0,
      .pressure_altitude_valid = false,
      .pressure_vertical_speed = 4,
      .pressure_vertical_speed_valid = false,
      .pressure_vertical_speed_mps_1min = 5,
      .pressure_vertical_speed_1min_valid = false,
      .distance_traveled_m = 0,
      .distance_traveled_valid = false,
      .distance_from_launch_m = 0,
      .distance_from_launch_valid = false,
      .distance_from_ground_m = 0,
      .distance_from_ground_valid = false,
      .average_speed_valid = false,
      .average_horiz_speed_mps_1min = 0,
      .average_vert_speed_mps_1min = 0,
      .max_horizontal_speed_mps = 0,
      .max_vertical_speed_mps = 0,
      .max_speed_valid = false,
  };

  shared.blocks.calculated_data.set(CALC_DATA);
}

inline void setCalculatedDataLaunch(data::SharedData &shared) {
  data::blocks::CalculatedData CALC_DATA{
      .pressure_altitude_m = 0,
      .pressure_altitude_valid = true,
      .pressure_vertical_speed = 0,
      .pressure_vertical_speed_valid = true,
      .pressure_vertical_speed_mps_1min = 0,
      .pressure_vertical_speed_1min_valid = true,
      .distance_traveled_m = 0,
      .distance_traveled_valid = true,
      .distance_from_launch_m = 0,
      .distance_from_launch_valid = true,
      .distance_from_ground_m = 0,
      .distance_from_ground_valid = true,
      .average_speed_valid = true,
      .average_horiz_speed_mps_1min = 0,
      .average_vert_speed_mps_1min = 0,
      .max_horizontal_speed_mps = 0,
      .max_vertical_speed_mps = 0,
      .max_speed_valid = true,
  };

  shared.blocks.calculated_data.set(CALC_DATA);
}

inline void setCalculatedDataAscent(data::SharedData &shared) {
  data::blocks::CalculatedData CALC_DATA{
      .pressure_altitude_m = 2000,
      .pressure_altitude_valid = true,
      .pressure_vertical_speed = 4,
      .pressure_vertical_speed_valid = true,
      .pressure_vertical_speed_mps_1min = 5,
      .pressure_vertical_speed_1min_valid = true,
      .distance_traveled_m = 1000,
      .distance_traveled_valid = true,
      .distance_from_launch_m = 1000,
      .distance_from_launch_valid = true,
      .distance_from_ground_m = 2000,
      .distance_from_ground_valid = true,
      .average_speed_valid = true,
      .average_horiz_speed_mps_1min = 5,
      .average_vert_speed_mps_1min = 4,
      .max_horizontal_speed_mps = 0,
      .max_vertical_speed_mps = 0,
      .max_speed_valid = true,
  };

  shared.blocks.calculated_data.set(CALC_DATA);
}

inline void setCalculatedDataDescent(data::SharedData &shared) {
  data::blocks::CalculatedData CALC_DATA{
      .pressure_altitude_m = 2000,
      .pressure_altitude_valid = true,
      .pressure_vertical_speed = -4,
      .pressure_vertical_speed_valid = true,
      .pressure_vertical_speed_mps_1min = -5,
      .pressure_vertical_speed_1min_valid = true,
      .distance_traveled_m = 1000,
      .distance_traveled_valid = true,
      .distance_from_launch_m = 1000,
      .distance_from_launch_valid = true,
      .distance_from_ground_m = 2000,
      .distance_from_ground_valid = true,
      .average_speed_valid = true,
      .average_horiz_speed_mps_1min = 5,
      .average_vert_speed_mps_1min = -4,
      .max_horizontal_speed_mps = 0,
      .max_vertical_speed_mps = 0,
      .max_speed_valid = true,
  };

  shared.blocks.calculated_data.set(CALC_DATA);
}

inline void setCalculatedDataRecovery(data::SharedData &shared) {
  data::blocks::CalculatedData CALC_DATA{
      .pressure_altitude_m = 2000,
      .pressure_altitude_valid = true,
      .pressure_vertical_speed = 0,
      .pressure_vertical_speed_valid = true,
      .pressure_vertical_speed_mps_1min = 0,
      .pressure_vertical_speed_1min_valid = true,
      .distance_traveled_m = 1000,
      .distance_traveled_valid = true,
      .distance_from_launch_m = 1000,
      .distance_from_launch_valid = true,
      .distance_from_ground_m = 1000, // landing on higher terrain
      .distance_from_ground_valid = true,
      .average_speed_valid = true,
      .average_horiz_speed_mps_1min = 3, // someone is walking away with it
      .average_vert_speed_mps_1min = 0,
      .max_horizontal_speed_mps = 0,
      .max_vertical_speed_mps = 0,
      .max_speed_valid = true,
  };

  shared.blocks.calculated_data.set(CALC_DATA);
}
