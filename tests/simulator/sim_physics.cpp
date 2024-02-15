/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   sim_physics.cpp
 * @brief  GFS Simulator Physics Implementation
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-02-13
 * @copyright  2024 (license to be defined)
 */

#include <iostream>

#include <BoosterSeat/geo.hpp>

#include "sim_physics.hpp"

namespace gfs_sim {

void BalloonPhysics::update(double delta_time_s) {
  altitude_ += vertical_speed_ * delta_time_s;
  vertical_speed_ += vertical_acceleration_ * delta_time_s;

  if (state_ == SimState::ASCENT || state_ == SimState::POP ||
      state_ == SimState::DESCENT || state_ == SimState::LANDING) {
    updateHorizontal(delta_time_s);
  }

  stateMachine();
  env_.setAltitudeMeters(altitude_);

  data_.altitude_m = altitude_;
  data_.vert_velocity_mps = vertical_speed_;
  data_.vert_accel_mps2 = vertical_acceleration_;
  data_.horz_velocity_mps = horizontal_speed_;
  data_.heading_deg = direction_of_travel_;
  data_.latitude_deg = latitude_;
  data_.longitude_deg = longitude_;
}

void BalloonPhysics::updateHorizontal(double delta_time_s) {
  double horz_dist_moved = horizontal_speed_ * delta_time_s;
  total_distance_ += horz_dist_moved;
  distance_since_last_coordinate_ += horz_dist_moved;

  constexpr double K_COORDINATE_INTERVAL_METERS = 50.0;
  if (distance_since_last_coordinate_ > K_COORDINATE_INTERVAL_METERS) {
    updateCoordinates(distance_since_last_coordinate_, getDirectionOfTravel());
    distance_since_last_coordinate_ = 0.0;
  }

  double wind_spd = env_.getWindSpeedMPS();
  double wind_dir = env_.getWindDirectionDegrees();

  setHorizontalSpeed(wind_spd);
  setDirectionOfTravel(wind_dir);
}

void BalloonPhysics::updateCoordinates(double distance_m, double heading) {
  bst::geo::Point current_position(latitude_, longitude_);
  auto new_position =
      current_position.shootVector(heading, distance_m / 1000.0);
  latitude_ = new_position.latitude();
  longitude_ = new_position.longitude();
}

void BalloonPhysics::statePreLaunch() {
  setVerticalAcceleration(0.0);
  setHorizontalSpeed(0.0);
  if (launch_signal_) {
    state_ = SimState::ASCENT;
  }
}

void BalloonPhysics::stateAscent() {
  // speed up to the minimum ascent rate
  if (getVerticalSpeed() < K_ASCENT_RATE_METERS_PER_SECOND_MIN) {
    setVerticalAcceleration(K_ASCENT_ACCELERATION_MPS2);
  } else if (getVerticalSpeed() > K_ASCENT_RATE_METERS_PER_SECOND_MAX) {
    setVerticalAcceleration(-K_ASCENT_ACCELERATION_MPS2);
  } else {
    verticalAccelerationVariation(K_ASCENT_ACCELERATION_VARIANCE);
  }

  if (getAltitude() > K_POP_ALTITUDE_METERS) {
    state_ = SimState::POP;
    setVerticalAcceleration(K_POP_ACCELERATION_MPS2);
  }
}

void BalloonPhysics::statePop() {
  if (getVerticalSpeed() > K_POP_DESCENT_RATE_METERS_PER_SECOND_TARGET) {
    verticalAccelerationVariation(K_POP_ACCELERATION_VARIANCE);
  } else {
    state_ = SimState::DESCENT;
  }
}

void BalloonPhysics::stateDescent() {
  double descent_speed_ratio = getAltitude() / K_POP_ALTITUDE_METERS;
  descent_speed_ratio += 0.1;
  double target_descent_speed = K_DESCENT_RATE_METERS_PER_SECOND_MIN +
                                (K_DESCENT_RATE_METERS_PER_SECOND_MAX -
                                 K_DESCENT_RATE_METERS_PER_SECOND_MIN) *
                                    descent_speed_ratio * -1.0;
  if (target_descent_speed > K_DESCENT_RATE_METERS_PER_SECOND_MIN * -1.0) {
    target_descent_speed = -K_DESCENT_RATE_METERS_PER_SECOND_MIN;
  }
  // going too fast
  if (getVerticalSpeed() < target_descent_speed) {
    setVerticalAcceleration(K_DESCENT_DECELERATION_MPS2);
    // physics_.verticalAccelerationVariation(K_DESCENT_DECELERATION_VARIANCE);
  } else { // going too slow
    setVerticalAcceleration(-0.1);
    // physics_.verticalVariation(K_DESCENT_DECELERATION_VARIANCE);
  }

  if (getAltitude() <
      K_LANDING_ALTITUDE_METERS + K_DESCENT_TRANSITION_ALTITUDE_METERS) {
    state_ = SimState::LANDING;
  }
}

void BalloonPhysics::stateLanding() {
  if (getAltitude() < K_LANDING_ALTITUDE_METERS + 0.3) {
    stopTravel();
    state_ = SimState::LANDED;
  }
}

void BalloonPhysics::stateLanded() {
}

void BalloonPhysics::stateMachine() {
  switch (state_) {
  case SimState::PRE_LAUNCH:
    statePreLaunch();
    break;
  case SimState::ASCENT:
    stateAscent();
    break;
  case SimState::POP:
    statePop();
    break;
  case SimState::DESCENT:
    stateDescent();
    break;
  case SimState::LANDING:
    stateLanding();
    break;
  case SimState::LANDED:
    stateLanded();
    break;
  }
}

} // namespace gfs_sim