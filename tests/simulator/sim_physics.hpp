/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   sim_physics.hpp
 * @brief  GFS Simulator Physics
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-02-13
 * @copyright  2024 (license to be defined)
 */

#ifndef GFS_SIM_PHYSICS_HPP_
#define GFS_SIM_PHYSICS_HPP_

#include "sim_parameters.hpp"

namespace gfs_sim {
class BalloonPhysics {
public:
  BalloonPhysics(SimState &state) : state_(state) {
  }

  void update(double delta_time_s) {
    altitude_ += vertical_speed_ * delta_time_s;
    vertical_speed_ += vertical_acceleration_ * delta_time_s;
    stateMachine();
  }

  double getAltitude() const {
    return altitude_;
  }

  double getVerticalSpeed() const {
    return vertical_speed_;
  }

  double getVerticalAcceleration() const {
    return vertical_acceleration_;
  }

  void setVerticalAcceleration(double vertical_acceleration) {
    vertical_acceleration_ = vertical_acceleration;
  }

  void verticalAccelerationVariation(double variance) {
    vertical_acceleration_ += bst::randomDouble(-variance, variance);
  }

  void stopVertical() {
    vertical_speed_ = 0.0;
    vertical_acceleration_ = 0.0;
  }

  void launch() {
    launch_signal_ = true;
  }

private:
  bool launch_signal_{false};

  double altitude_{K_START_ALTITUDE_METERS};
  double vertical_speed_{K_START_VERTICAL_SPEED_MPS};
  double vertical_acceleration_{K_START_VERTICAL_ACCELERATION_MPS2};

  SimState &state_;

  void statePreLaunch() {
    setVerticalAcceleration(0.0);
    if (launch_signal_) {
      state_ = SimState::ASCENT;
    }
  }

  void stateAscent() {
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
      std::cout << " -- POP -- " << std::endl;
      setVerticalAcceleration(K_POP_ACCELERATION_MPS2);
    }
  }

  void statePop() {
    if (getVerticalSpeed() > K_POP_DESCENT_RATE_METERS_PER_SECOND_TARGET) {
      verticalAccelerationVariation(K_POP_ACCELERATION_VARIANCE);
    } else {
      state_ = SimState::DESCENT;
    }
  }

  void stateDescent() {
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

  void stateLanding() {
    if (getAltitude() < K_LANDING_ALTITUDE_METERS + 0.3) {
      stopVertical();
      state_ = SimState::LANDED;
    }
  }

  void stateLanded() {
  }

  void stateMachine() {
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
};
} // namespace gfs_sim

#endif /* GFS_SIM_PHYSICS_HPP_ */