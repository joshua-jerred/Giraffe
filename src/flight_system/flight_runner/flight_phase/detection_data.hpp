/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   detection_data.hpp
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-09-07
 * @copyright  2024 (license to be defined)
 */

#pragma once

#include <algorithm>
#include <array>
#include <variant>

#include "giraffe_assert.hpp"
#include "shared_data.hpp"

/**
 * @brief A class that contains and updates the parameters used to detect the
 * flight phase.
 */
class DetectionData {
public:
  /// @brief Default constructor.
  DetectionData(data::SharedData &shared_data);
  /// @brief Default destructor.
  ~DetectionData() = default;
  /// @brief Delete the copy constructor.
  DetectionData(const DetectionData &) = delete;
  /// @brief Delete the copy assignment operator.
  DetectionData &operator=(const DetectionData &) = delete;
  /// @brief Delete the move constructor.
  DetectionData(DetectionData &&) = delete;

  /// @brief A structure that represents the parameters that are used for flight
  /// phase detection.
  struct Parameter {
    /// @brief Parameter IDs for phase detection.
    enum class Id : uint8_t {
      /// @brief The mission clock in seconds, if the value can be negative or
      /// positive. A negative value is in the future.
      MISSION_CLOCK = 0,
      /// @brief True if the mission clock is running, false when it's paused.
      MISSION_CLOCK_RUNNING,
      /// @brief The uptime of the flight system in seconds.
      UP_TIME,
      /// @brief The altitude in meters as reported by the GPS.
      GPS_ALTITUDE,
      /// @brief The current vertical speed in meters per second.
      GPS_VERTICAL_SPEED,
      /// @brief A 1 minute average of the vertical speed in meters per second.
      GPS_VERTICAL_SPEED_1MIN,
      /// @brief The current horizontal speed in meters per second.
      /// @todo Filter this to 10 seconds.
      GPS_HORIZONTAL_SPEED,
      /// @brief A 1 minute average of the horizontal speed in meters per
      /// second.
      GPS_HORIZONTAL_SPEED_1MIN,
      /// @brief The altitude in meters as reported by the barometer.
      PRESSURE_ALTITUDE,
      /// @brief Vertical speed as reported by the barometer in meters per
      /// second.
      PRESSURE_VERTICAL_SPEED,
      /// @brief Vertical speed as reported by the barometer in meters per
      /// second over the past minute.
      PRESSURE_VERTICAL_SPEED_1MIN,
      /// @brief The distance from the ground in meters. Can be based on
      /// pressure and/or GPS data.
      DISTANCE_FROM_GROUND,
      /// @brief The distance traveled in meters.
      DISTANCE_TRAVELED,
      /// @brief The distance from the launch point in meters. Two dimensional.
      DISTANCE_FROM_LAUNCH,
      /// @brief The number of parameters.
      NUM_PARAMETERS
    };

    /// @brief Validation modes for parameters.
    enum class Validation { NONE, IN_RANGE };

    /// @brief Filter modes for parameters.
    enum class InvalidateFilter {
      /// @brief The parameter will always be updated even if it's marked
      /// invalid. This is the default.
      ALWAYS_UPDATE = 0,
      /// @brief The parameter will not be updated, or set to zero if it's
      /// marked invalid.
      DO_NOT_UPDATE,
      /// @brief The parameter will be set to zero if it's marked invalid.
      ZERO_IF_INVALID,
      /// @brief The parameter will be updated if it's marked invalid, but the
      /// value will be clamped to the minimum or maximum.
      CLAMP_IN_RANGE,
    };

    Parameter() = default;
    Parameter(
        Parameter::Id id, Validation validation = Validation::NONE,
        InvalidateFilter invalidate_filter = InvalidateFilter::ALWAYS_UPDATE,
        double minimum = 0.0, double maximum = 0.0);

    ~Parameter() = default;

    /// @brief True if the data in this parameter is valid.
    bool is_valid = false;

    /// @brief The value of the parameter.
    double value = 0.0;

    /// @brief A digital value for the parameter.
    bool binary_value = false;

    /// @brief The ID of the parameter.
    Id id = Id::NUM_PARAMETERS;

    /// @brief The validation mode for the parameter. Must be set in the
    /// constructor.
    const Validation validation = Validation::NONE;

    /// @brief The filter mode for the parameter for when it's marked invalid.
    const InvalidateFilter invalidate_filter = InvalidateFilter::ALWAYS_UPDATE;

    /// @brief The minimum value for the parameter.
    const double min = 0.0;

    /// @brief The maximum value for the parameter.
    const double max = 0.0;
  };

  /// @brief Get a parameter by its ID.
  /// @param id The ID of the parameter to get.
  Parameter getParameter(Parameter::Id id) const {
    giraffe_assert(id != Parameter::Id::NUM_PARAMETERS);
    return parameters_.at(static_cast<uint8_t>(id));
  }

  /// @brief Updates all of the parameters.
  void updateParameters() {
    {
      bool valid = true;
      int32_t mission_clock_seconds = 0.0;
      bool running = flight_data_.getMissionClockSeconds(mission_clock_seconds);

      updateParameter(Parameter::Id::MISSION_CLOCK, mission_clock_seconds,
                      valid);

      // Digital value
      updateParameter(Parameter::Id::MISSION_CLOCK_RUNNING, 0, valid, true,
                      running);

      const double uptime_seconds = flight_data_.getUptimeSeconds();
      updateParameter(Parameter::Id::UP_TIME, uptime_seconds);
    }
    {
      // GPS Data

      bool valid = true;
      auto location = location_data_.get();

      // There is no filtering on the GPS Fix.
      valid &= location.current_gps_fix == data::GpsFix::FIX_3D ||
               location.current_gps_fix == data::GpsFix::FIX_2D;
      valid &= location.have_gps_source;

      auto frame = location.last_valid_gps_frame;

      updateParameter(Parameter::Id::GPS_ALTITUDE, frame.altitude, valid);
      updateParameter(Parameter::Id::GPS_VERTICAL_SPEED, frame.vertical_speed,
                      valid);
      updateParameter(Parameter::Id::GPS_HORIZONTAL_SPEED,
                      frame.horizontal_speed, valid);
    }
    {
      // Calculated Data
      auto data = calculated_data_.get();

      updateParameter(Parameter::Id::GPS_VERTICAL_SPEED_1MIN,
                      data.average_vert_speed_mps_1min,
                      data.average_speed_valid);
      updateParameter(Parameter::Id::GPS_HORIZONTAL_SPEED_1MIN,
                      data.average_horiz_speed_mps_1min,
                      data.average_speed_valid);

      updateParameter(Parameter::Id::DISTANCE_FROM_GROUND,
                      data.distance_from_ground_m,
                      data.distance_from_ground_valid);
      updateParameter(Parameter::Id::DISTANCE_TRAVELED,
                      data.distance_traveled_m, data.distance_traveled_valid);
      updateParameter(Parameter::Id::DISTANCE_FROM_LAUNCH,
                      data.distance_from_launch_m,
                      data.distance_from_launch_valid);

      updateParameter(Parameter::Id::PRESSURE_ALTITUDE,
                      data.pressure_altitude_m, data.pressure_altitude_valid);
      updateParameter(Parameter::Id::PRESSURE_VERTICAL_SPEED,
                      data.pressure_vertical_speed,
                      data.pressure_vertical_speed_valid);
      updateParameter(Parameter::Id::PRESSURE_VERTICAL_SPEED_1MIN,
                      data.pressure_vertical_speed_mps_1min,
                      data.pressure_vertical_speed_1min_valid);
    }
  }

private:
  /// @brief Validates, filters, and updates a parameter's value.
  /// @param id - The ID of the parameter to update.
  /// @param newValue - The new value of the parameter.
  /// @param valid - (default true) Set to false to mark the parameter as
  /// invalid before validation.
  void updateParameter(Parameter::Id id, double newValue, bool valid = true,
                       bool digital = false, bool digitalValue = false) {
    giraffe_assert(id != Parameter::Id::NUM_PARAMETERS);

    // If already marked invalid or the validation fails, apply the invalid
    // filter and mark the parameter as invalid.
    if (!valid || !validateParameter(id, newValue)) {
      valid = false;
      newValue = applyInvalidFilter(id, newValue);
    }

    parameters_.at(static_cast<uint8_t>(id)).is_valid = valid;
    if (digital) {
      parameters_.at(static_cast<uint8_t>(id)).binary_value = digitalValue;
    } else {
      parameters_.at(static_cast<uint8_t>(id)).value = newValue;
    }
  }

  /// @brief Validates a parameter.
  /// @param id - The ID of the parameter to validate.
  /// @param value - The value of the parameter to validate.
  bool validateParameter(Parameter::Id id, double value) {
    auto param = parameters_.at(static_cast<uint8_t>(id));

    switch (param.validation) {
    case Parameter::Validation::NONE:
      return true;
    case Parameter::Validation::IN_RANGE:
      return value >= param.min && value <= param.max;
    default:
      giraffe_assert(false);
      return false;
    }

    giraffe_assert(false);
    return false;
  }

  /// @brief Applies a filter to a parameter. Does not update the parameter.
  /// or care if it's valid.
  /// @param id - The ID of the parameter to filter.
  /// @param value - The value to filter.
  /// @return The filtered value.
  double applyInvalidFilter(Parameter::Id id, double value) {
    auto param = parameters_.at(static_cast<uint8_t>(id));

    switch (param.invalidate_filter) {
    case Parameter::InvalidateFilter::ALWAYS_UPDATE:
      return value;
    case Parameter::InvalidateFilter::DO_NOT_UPDATE:
      return param.value;
    case Parameter::InvalidateFilter::ZERO_IF_INVALID:
      return 0.0;
    case Parameter::InvalidateFilter::CLAMP_IN_RANGE:
      return std::clamp(value, param.min, param.max);
    default:
      giraffe_assert(false);
      return 0.0;
    }

    giraffe_assert(false);
    return 0.0;
  }

  /// @brief An array that contains all of the parameters.
  std::array<Parameter, static_cast<uint8_t>(Parameter::Id::NUM_PARAMETERS)>
      parameters_{};

  data::FlightData &flight_data_;
  data::blocks::Block<data::blocks::CalculatedData> &calculated_data_;
  data::blocks::Block<data::blocks::LocationData> &location_data_;
  data::blocks::Block<data::blocks::ImuData> &imu_data_;
};