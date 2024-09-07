/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   flight_phase_detection.hpp
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-09-07
 * @copyright  2024 (license to be defined)
 */

#pragma once

#include "shared_data.hpp"

class FlightPhaseDetection {
public:
  /// @brief A stucture that contains the probabilities of being in each phase.
  /// The probabilities range from 0 to 255, where 0 is 0% and 255 is 100%.
  struct PhaseProbability {
    uint8_t launch = 0.0;
    uint8_t ascent = 0.0;
    uint8_t descent = 0.0;
    uint8_t recovery = 0.0;
  };

  FlightPhaseDetection(data::SharedData &shared_data)
      : shared_data_(shared_data) {
  }
  ~FlightPhaseDetection() = default;

  void detectFlightPhase() {
  }

private:
  struct FlightPhaseParameters {
    double gps_distance_from_launch_m = 0.0;
    double gps_distance_from_ground_m = 0.0;
    double gps_altitude_m = 0.0;
    double gps_horizontal_speed_mps_10s = 0.0;
    double gps_vertical_speed_mps_60s = 0.0;
  };

  void calculateLaunchPhaseProbability() {
  }

  void calculateAscentPhaseProbability() {
  }

  void calculateDescentPhaseProbability() {
  }

  void calculateRecoveryPhaseProbability() {
  }

  PhaseProbability phase_probability_{};

  data::SharedData &shared_data_;
};