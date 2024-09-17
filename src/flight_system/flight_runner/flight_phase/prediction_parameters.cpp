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

#include "prediction_parameters.hpp"

using Parameter = PredictionParameters::Parameter;
using Id = PredictionParameters::Parameter::Id;
using Validation = PredictionParameters::Parameter::Validation;
using InvalidFilter = PredictionParameters::Parameter::InvalidateFilter;

/// @brief The parameters used for flight phase detection.
/// @todo All of these need to be configurable.
/// @todo Think about if any filtering needs to be done on the data or if it's
/// all already hit somewhere else.

std::array<Parameter, static_cast<uint8_t>(Parameter::Id::NUM_PARAMETERS)>
buildParametersArray() {
  return {
      // clang-format off
  Parameter( Id::MISSION_CLOCK,
    Validation::IN_RANGE, InvalidFilter::CLAMP_IN_RANGE,
    -31536000.0, 31536000.0
  ),
  Parameter( Id::UP_TIME, 
    Validation::IN_RANGE, InvalidFilter::CLAMP_IN_RANGE, 
    1.0, 31536000.0
  ),
  Parameter{ Id::GPS_ALTITUDE,
    Validation::IN_RANGE, InvalidFilter::DO_NOT_UPDATE, -450.0, 50000.0
  },
  Parameter{ Id::GPS_VERTICAL_SPEED
    /// @todo Add validation for vertical speed.
  },
  Parameter{ Id::GPS_VERTICAL_SPEED_1MIN
    /// @todo Add validation for vertical speed.
  },
  Parameter{ Id::GPS_HORIZONTAL_SPEED
    /// @todo Add validation for horizontal speed.
  },
  Parameter{ Id::GPS_HORIZONTAL_SPEED_1MIN
    /// @todo Add validation for horizontal speed.
  },
  Parameter{ Id::PRESSURE_ALTITUDE,
    Validation::IN_RANGE, InvalidFilter::CLAMP_IN_RANGE, -450.0, 50000.0
  },
  Parameter{ Id::PRESSURE_VERTICAL_SPEED,
  },
  Parameter{ Id::PRESSURE_VERTICAL_SPEED_1MIN,
      Validation::IN_RANGE, InvalidFilter::ZERO_IF_INVALID, -100.0, 100.0
  },
  Parameter{ Id::DISTANCE_FROM_GROUND,
    Validation::IN_RANGE, InvalidFilter::ALWAYS_UPDATE, -450.0, 50000.0
  },
  Parameter{ Id::DISTANCE_TRAVELED,
    Validation::IN_RANGE, InvalidFilter::ALWAYS_UPDATE, 0.0, 3500000.0
  },
  Parameter{ Id::DISTANCE_FROM_LAUNCH,
    Validation::IN_RANGE, InvalidFilter::ALWAYS_UPDATE, 0.0, 3500000.0
  },
};
  // clang-format on
}

PredictionParameters::Parameter::Parameter(Parameter::Id id,
                                           Validation validation,
                                           InvalidateFilter filter,
                                           double minimum, double maximum)
    : id(id), validation(validation),
      invalidate_filter{filter}, min{minimum}, max{maximum} {};

PredictionParameters::PredictionParameters(data::SharedData &shared_data)
    : parameters_{buildParametersArray()},
      flight_data_(shared_data.flight_data),
      calculated_data_(shared_data.blocks.calculated_data),
      location_data_(shared_data.blocks.location_data),
      imu_data_(shared_data.blocks.imu_data) {

  for (size_t i = 0; i < parameters_.size(); i++) {
    if (parameters_.at(i).id != static_cast<Id>(i)) {
      giraffe_assert(false);
    }
  }
}