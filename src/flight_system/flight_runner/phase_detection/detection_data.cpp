/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   detection_data.cpp
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-09-07
 * @copyright  2024 (license to be defined)
 */

#include "detection_data.hpp"

using Parameter = DetectionData::Parameter;
using Id = DetectionData::Parameter::Id;
using Validation = DetectionData::Parameter::Validation;

// clang-format off
inline std::array<
  Parameter, static_cast<uint8_t>(Parameter::Id::NUM_PARAMETERS)>
  PARAMETER_DATA = {
  Parameter{ Id::MISSION_CLOCK,
    Validation::IN_RANGE, -31536000.0, 31536000.0
  },
  Parameter{ Id::UP_TIME, 
   Validation::IN_RANGE, 1.0, 31536000.0
  },
  Parameter{ Id::GPS_ALTITUDE
  },
  Parameter{ Id::GPS_VERTICAL_SPEED
  },
  Parameter{ Id::GPS_VERTICAL_SPEED_MPS_1MIN
  },
  Parameter{ Id::GPS_HORIZONTAL_SPEED
  },
  Parameter{ Id::GPS_HORIZONTAL_SPEED_1MIN
  },
  Parameter{ Id::PRESSURE_ALTITUDE
  },
  Parameter{ Id::PRESSURE_VERTICAL_SPEED
  },
  Parameter{ Id::PRESSURE_VERTICAL_SPEED_1MIN
  },
  Parameter{ Id::DISTANCE_FROM_LAUNCH
  },
  Parameter{ Id::DISTANCE_FROM_GROUND
  },
  Parameter{ Id::DISTANCE_TRAVELED
  }
};
// clang-format on

DetectionData::Parameter::Parameter(Parameter::Id id, Validation validation,
                                    double minimum, double maximum)
    : id(id), validation(validation), min{minimum}, max{maximum} {};

DetectionData::DetectionData(
    data::FlightData &flight_data,
    data::blocks::Block<data::blocks::CalculatedData> &calculated_data,
    data::blocks::Block<data::blocks::LocationData> &location_data,
    data::blocks::Block<data::blocks::ImuData> &imu_data)
    : parameters_{PARAMETER_DATA}, flight_data_(flight_data),
      calculated_data_(calculated_data), location_data_(location_data),
      imu_data_(imu_data) {

  for (size_t i = 0; i < parameters_.size(); i++) {
    giraffe_assert(parameters_.at(i).id == static_cast<Id>(i));
  }
}