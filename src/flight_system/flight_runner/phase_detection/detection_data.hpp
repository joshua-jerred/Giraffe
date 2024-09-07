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
  DetectionData(
      data::FlightData &flight_data,
      data::blocks::Block<data::blocks::CalculatedData> &calculated_data,
      data::blocks::Block<data::blocks::LocationData> &location_data,
      data::blocks::Block<data::blocks::ImuData> &imu_data);
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
      /// @brief The uptime of the flight system in seconds.
      UP_TIME,
      /// @brief The altitude in meters as reported by the GPS.
      GPS_ALTITUDE,
      /// @brief The current vertical speed in meters per second.
      GPS_VERTICAL_SPEED,
      /// @brief A 1 minute average of the vertical speed in meters per second.
      GPS_VERTICAL_SPEED_MPS_1MIN,
      /// @brief The current horizontal speed in meters per second.
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
      /// @brief The distance from the launch point in meters. Two dimensional.
      DISTANCE_FROM_LAUNCH,
      /// @brief The distance from the ground in meters. Can be based on
      /// pressure and/or GPS data.
      DISTANCE_FROM_GROUND,
      /// @brief The distance traveled in meters.
      DISTANCE_TRAVELED,
      /// @brief The number of parameters.
      NUM_PARAMETERS
    };

    /// @brief Validation modes for parameters.
    enum class Validation { NONE, IN_RANGE };

    Parameter() = default;
    Parameter(Parameter::Id id, Validation validation = Validation::NONE,
              double minimum = 0.0, double maximum = 0.0);

    ~Parameter() = default;

    /// @brief True if the data in this parameter is valid.
    bool is_valid = false;

    /// @brief The value of the parameter.
    double value = 0.0;

    /// @brief The ID of the parameter.
    Id id = Id::NUM_PARAMETERS;

    /// @brief The validation mode for the parameter. Must be set in the
    /// constructor.
    const Validation validation = Validation::NONE;

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

    const auto validateInRange = [](double value, double min, double max) {
      return value >= min && value <= max;
    };

    {
      bool valid = true;
      const double uptime_seconds = flight_data_.getUptimeSeconds();
      valid &= validateInRange(uptime_seconds, 0.0, 31536000);
      updateParameter(Parameter::Id::MISSION_CLOCK, valid, uptime_seconds);
    }
    {
      bool valid = true;
      int32_t mission_clock_seconds = 0.0;
      bool running = flight_data_.getMissionClockSeconds(mission_clock_seconds);
      valid &= validateInRange(mission_clock_seconds, -31536000, 31536000);
    }
  }

private:
  /// @brief Validates a parameter.
  /// @param id - The ID of the parameter to validate.
  /// @param value - The value of the parameter to validate.
  bool validateParameter(Parameter::Id id, double value) {
    (void)id;
    (void)value;
    return true;
  }

  void updateParameter(Parameter::Id id, bool valid, double value) {
    giraffe_assert(id != Parameter::Id::NUM_PARAMETERS);
    parameters_.at(static_cast<uint8_t>(id)).value = value;
    parameters_.at(static_cast<uint8_t>(id)).is_valid = valid;
  }

  /// @brief An array that contains all of the parameters.
  std::array<Parameter, static_cast<uint8_t>(Parameter::Id::NUM_PARAMETERS)>
      parameters_{};

  data::FlightData &flight_data_;
  data::blocks::Block<data::blocks::CalculatedData> &calculated_data_;
  data::blocks::Block<data::blocks::LocationData> &location_data_;
  data::blocks::Block<data::blocks::ImuData> &imu_data_;
};