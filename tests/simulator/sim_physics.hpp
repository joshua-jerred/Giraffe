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

#include <BoosterSeat/random.hpp>

#include "sim_environmental.hpp"
#include "sim_parameters.hpp"

namespace gfs_sim {
class BalloonPhysics {
public:
  BalloonPhysics(SimState &state, SimEnvironmental &env)
      : state_(state), env_(env) {
  }

  void update(double delta_time_s);

  void launch() {
    launch_signal_ = true;
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

  double getHorizontalSpeed() const {
    return horizontal_speed_;
  }

  double getDirectionOfTravel() const {
    return direction_of_travel_;
  }

  double getTotalDistance() const {
    return total_distance_;
  }

private:
  void setVerticalAcceleration(double vertical_acceleration) {
    vertical_acceleration_ = vertical_acceleration;
  }

  void setHorizontalSpeed(double horizontal_speed) {
    horizontal_speed_ = horizontal_speed;
  }

  void setDirectionOfTravel(double direction_of_travel) {
    direction_of_travel_ = direction_of_travel;
  }

  void verticalAccelerationVariation(double variance) {
    vertical_acceleration_ += bst::randomDouble(-variance, variance);
  }

  void updateHorizontal(double delta_time_s);

  void updateCoordinates(double distance_m, double heading);

  void stopTravel() {
    vertical_speed_ = 0.0;
    vertical_acceleration_ = 0.0;
    horizontal_speed_ = 0.0;
  }

  void statePreLaunch();
  void stateAscent();
  void statePop();
  void stateDescent();
  void stateLanding();
  void stateLanded();
  void stateMachine();

  bool launch_signal_{false};

  double altitude_{K_START_ALTITUDE_METERS};
  double vertical_speed_{K_START_VERTICAL_SPEED_MPS};
  double vertical_acceleration_{K_START_VERTICAL_ACCELERATION_MPS2};

  double direction_of_travel_{0.0};
  double horizontal_speed_{K_START_HORIZONTAL_SPEED_MPS};
  double total_distance_{0.0};
  double distance_since_last_coordinate_{0.0};
  double latitude_{K_START_LATITUDE};
  double longitude_{K_START_LONGITUDE};

  SimState &state_;
  SimEnvironmental &env_;
};

} // namespace gfs_sim

#endif /* GFS_SIM_PHYSICS_HPP_ */