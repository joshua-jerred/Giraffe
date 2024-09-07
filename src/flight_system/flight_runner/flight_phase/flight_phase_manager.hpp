/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   flight_phase.hpp
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-09-07
 * @copyright  2024 (license to be defined)
 */

#pragma once

#include "flight_phase_predictor.hpp"

class FlightPhaseManager {
public:
  FlightPhaseManager() = default;
  ~FlightPhaseManager() = default;

  void update() {
    phase_predictor_.update();
  }

private:
  FlightPhasePredictor phase_predictor_;
};