/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   flight_logic.cpp
 * @brief  The implementation of the flight logic within the flight runner.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-02-15
 * @copyright  2024 (license to be defined)
 */

#include "flight_runner.hpp"
#include "to_string.hpp"

//// ---------------------- FLIGHT LOGIC PLUGINS ---------------------- ////

void FlightRunner::fl_preLaunchLogic() {
  // This is the pre-launch logic
}

//// ------------------------------------------------------------------ ////

void FlightRunner::flightLogic() {
  detectFlightPhase();
}

void FlightRunner::detectFlightPhase() {
  const auto calculated = shared_data_.blocks.calculated_data.get();
  const auto location = shared_data_.blocks.location_data.get();

  const bool gps_valid = location.last_valid_gps_fix == data::GpsFix::FIX_3D;

  // predicted probabilities of being in each phase
  double launch = 0.0;
  double ascent = 0.0;
  double descent = 0.0;
  double recovery = 0.0;

  // parameters
  double gps_distance_from_launch_m = calculated.distance_from_launch_m;
  double gps_distance_from_ground_m = calculated.distance_from_ground_m;
  double gps_altitude_m = location.last_valid_gps_frame.altitude;
  double gps_horizontal_speed_mps_10s = calculated.average_horiz_speed_mps_1min;
  double gps_vertical_speed_mps_10s = 0.0;
  double gps_vertical_speed_mps_60s = calculated.average_vert_speed_mps_1min;

  { // launch
    launch += 40.0 * (gps_distance_from_launch_m < 10.0);
    launch += 40.0 * (gps_distance_from_ground_m < 5.0);
    launch += 5.0 * (gps_horizontal_speed_mps_10s < 1.0);
    launch += 10.0 * (gps_vertical_speed_mps_10s < 0.5);
    launch += 5.0 * (gps_vertical_speed_mps_60s < 0.5);
  }
  { // ascent
    ascent += 20.0 * (gps_distance_from_launch_m > 10.0);
    ascent += 10.0 * (gps_distance_from_ground_m > 5.0);
    ascent += 5.0 * (gps_horizontal_speed_mps_10s > 0.5);
    ascent += 10.0 * (gps_vertical_speed_mps_10s > 0.5);
    ascent += 50.0 * (gps_vertical_speed_mps_60s > 0.5);
  }
  { // descent
    descent += 5.0 * (gps_distance_from_launch_m > 10.0);
    descent += 15.0 * (gps_distance_from_ground_m > 1500.0);
    descent += 10.0 * (gps_horizontal_speed_mps_10s > 0.5);
    descent += 30.0 * (gps_vertical_speed_mps_10s < -1.0);
    descent += 40.0 * (gps_vertical_speed_mps_60s < -1.0);
  }
  { // recovery
    recovery += 10.0 * (gps_distance_from_launch_m > 10.0);
    recovery += 10.0 * (gps_distance_from_ground_m > 5.0);
    recovery += 20.0 * (gps_horizontal_speed_mps_10s < 1.0);
    recovery += 30.0 * (gps_vertical_speed_mps_10s > -0.5 &&
                        gps_vertical_speed_mps_10s < 0.5);
    recovery += 40.0 * (gps_vertical_speed_mps_60s < -0.5 &&
                        gps_vertical_speed_mps_60s > 0.5);
  }
  shared_data_.flight_data.setPhasePrediction(launch, ascent, descent,
                                              recovery);

  // bool gps_valid = false;
  // bool ascending = false;
  // bool descending = false;

  switch (shared_data_.flight_data.getFlightPhase()) {

    // -- CURRENTLY UNKNOWN --
  case FlightPhase::UNKNOWN:
    // If the flight phase is unknown, (start-up only) we need check if we were
    // previously in a flight phase.
    switch (flight_runner_data_.getFlightPhase()) {
    case FlightPhase::UNKNOWN:
      // Previously unknown/never set.
      setFlightPhase(FlightPhase::LAUNCH);
      break;
    case FlightPhase::PRE_LAUNCH:
      setFlightPhase(FlightPhase::PRE_LAUNCH);
      break;
    case FlightPhase::LAUNCH:
      setFlightPhase(FlightPhase::LAUNCH);
      break;
    case FlightPhase::ASCENT:
      setFlightPhase(FlightPhase::ASCENT);
      break;
    case FlightPhase::DESCENT:
      setFlightPhase(FlightPhase::DESCENT);
      break;
    case FlightPhase::RECOVERY:
      setFlightPhase(FlightPhase::RECOVERY);
      break;
    default:
      giraffe_assert(false);
      break;
    }
    shared_data_.streams.log.info(
        node::Identification::FLIGHT_RUNNER,
        "startup flight phase set to: " +
            util::to_string(shared_data_.flight_data.getFlightPhase()));
    break;

    // -- CURRENTLY IN PRE-LAUNCH PHASE --
  case FlightPhase::PRE_LAUNCH:
    break;

    // -- CURRENTLY IN LAUNCH PHASE --
  case FlightPhase::LAUNCH:
    if (gps_valid && (ascent - launch > 50.0)) {
      setFlightPhase(FlightPhase::ASCENT);
    }
    break;

    // -- CURRENTLY IN ASCENT PHASE --
  case FlightPhase::ASCENT:
    if (gps_valid && (descent - ascent > 50.0)) {
      setFlightPhase(FlightPhase::DESCENT);
    }
    break;

    // -- CURRENTLY IN DESCENT PHASE --
  case FlightPhase::DESCENT:
    break;

    // -- CURRENTLY IN RECOVERY PHASE --
  case FlightPhase::RECOVERY:
    break;
  default:
    giraffe_assert(false);
    break;
  }
}

void FlightRunner::setFlightPhase(FlightPhase new_phase) {
  const auto old_phase = shared_data_.flight_data.getFlightPhase();
  if (old_phase == new_phase) {
    return;
  }

  if (old_phase == FlightPhase::PRE_LAUNCH &&
      new_phase == FlightPhase::LAUNCH && !setLaunchPosition()) {
    // If the launch position is not set, we cannot transition to launch.
    // Error message is already logged.
    return;
  }

  // Report to shared data
  shared_data_.flight_data.flight_phase = new_phase;
  // Save to file
  flight_runner_data_.setFlightPhase(new_phase);

  shared_data_.streams.data.addData(
      node::Identification::FLIGHT_RUNNER,
      data::DataId::FLIGHT_RUNNER_flightPhaseChange,
      util::to_string(new_phase));

#if RUN_IN_SIMULATOR == 1
  if (new_phase == FlightPhase::LAUNCH) {
    p_simulator_->launch();
  }
#endif
}