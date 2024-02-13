/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   gfs_simulated_environment.hpp
 * @brief  A simulated environment for testing the flight system.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-02-13
 * @copyright  2024 (license to be defined)
 */

#ifndef GFS_SIMULATOR_HPP_
#define GFS_SIMULATOR_HPP_

#include <iostream>

#include <BoosterSeat/random.hpp>
#include <BoosterSeat/science.hpp>
#include <BoosterSeat/sleep.hpp>
#include <BoosterSeat/stopwatch.hpp>
#include <BoosterSeat/timer.hpp>

inline constexpr uint32_t K_UPDATE_INTERVAL_MS = 100;
inline constexpr uint32_t K_SLEEP_INTERVAL_MS = K_UPDATE_INTERVAL_MS / 4;
inline constexpr double K_DELTA_TIME_S = K_UPDATE_INTERVAL_MS / 1000.0;
inline constexpr uint32_t K_PRINT_INTERVAL_MS = 1000;
inline constexpr bool K_PRINT_SIM_DATA = true;

// -- Physics Parameters --
// Start Parameters
inline constexpr double K_START_ALTITUDE_METERS = 450.0;
inline constexpr double K_START_VERTICAL_SPEED_MPS = 6.0;
inline constexpr double K_START_VERTICAL_ACCELERATION_MPS2 = 0.00001;
// Pre-launch Parameters
inline constexpr double K_PRELAUNCH_LAUNCH_DELAY_S =
    0.0; // set to 0 if the start parameters are non-zero
// Ascent Parameters
inline constexpr double K_ASCENT_ACCELERATION_MPS2 = 0.3;
inline constexpr double K_ASCENT_ACCELERATION_VARIANCE = 0.1;
inline constexpr double K_ASCENT_RATE_METERS_PER_SECOND_MAX = 8.0;
inline constexpr double K_ASCENT_RATE_METERS_PER_SECOND_MIN = 2.5;
inline constexpr double K_POP_ALTITUDE_METERS = 500; // 30000.0;
// Pop Parameters
inline constexpr double K_POP_DESCENT_RATE_METERS_PER_SECOND_TARGET = -30.0;
inline constexpr double K_POP_ACCELERATION_MPS2 = -7.0;
inline constexpr double K_POP_ACCELERATION_VARIANCE = 0.5;
// Descent Parameters
inline constexpr double K_DESCENT_RATE_METERS_PER_SECOND_MAX = 6.0;
inline constexpr double K_DESCENT_RATE_METERS_PER_SECOND_MIN = 5.0;

class Physics {
public:
  void update(double delta_time_s) {
    altitude_ += vertical_speed_ * delta_time_s;
    vertical_speed_ += vertical_acceleration_ * delta_time_s;
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

  void verticalVariation(double variance) {
    vertical_speed_ += bst::randomDouble(-variance, variance);
  }

private:
  double altitude_{K_START_ALTITUDE_METERS};
  double vertical_speed_{K_START_VERTICAL_SPEED_MPS};
  double vertical_acceleration_{K_START_VERTICAL_ACCELERATION_MPS2};
};

class GfsSimulator {
public:
  GfsSimulator() {
  }
  ~GfsSimulator() {
  }

  void run() {
    flight_stopwatch_.start();
    while (true) {
      bst::sleep(K_SLEEP_INTERVAL_MS);
      elapsed_seconds_ = flight_stopwatch_.elapsed(bst::Resolution::SECONDS);
      if (state_machine_timer_.isDone()) {
        physics_.update(K_DELTA_TIME_S);
        stateMachine();
        state_machine_timer_.reset();
      }
      if (print_timer_.isDone()) {
        printData();
        print_timer_.reset();
      }
    }
  }

private:
  enum class State { PRE_LAUNCH, ASCENT, POP, DESCENT, LANDING, LANDED };

  void statePreLaunch() {
    physics_.setVerticalAcceleration(0.0);
    if (elapsed_seconds_ > K_PRELAUNCH_LAUNCH_DELAY_S) {
      state_ = State::ASCENT;
    }
  }

  void stateAscent() {
    // speed up to the minimum ascent rate
    if (physics_.getVerticalSpeed() < K_ASCENT_RATE_METERS_PER_SECOND_MIN) {
      physics_.setVerticalAcceleration(K_ASCENT_ACCELERATION_MPS2);
    } else if (physics_.getVerticalSpeed() >
               K_ASCENT_RATE_METERS_PER_SECOND_MAX) {
      physics_.setVerticalAcceleration(-K_ASCENT_ACCELERATION_MPS2);
    } else {
      physics_.setVerticalAcceleration(
          physics_.getVerticalAcceleration() +
          bst::randomDouble(-K_ASCENT_ACCELERATION_VARIANCE,
                            K_ASCENT_ACCELERATION_VARIANCE));
    }

    if (physics_.getAltitude() > K_POP_ALTITUDE_METERS) {
      state_ = State::POP;
      std::cout << " -- POP -- " << std::endl;
    }
  }

  void statePop() {
    if (physics_.getVerticalSpeed() >
        K_POP_DESCENT_RATE_METERS_PER_SECOND_TARGET) {
      physics_.setVerticalAcceleration(K_POP_ACCELERATION_MPS2);
    } else {
      state_ = State::DESCENT;
    }
  }

  void stateDescent() {
  }

  void stateLanding() {
  }

  void stateLanded() {
  }

  void printData() {
    switch (state_) {
    case State::PRE_LAUNCH:
      std::cout << "PRE_LAUNCH";
      break;
    case State::ASCENT:
      std::cout << "ASCENT";
      break;
    case State::POP:
      std::cout << "POP";
      break;
    case State::DESCENT:
      std::cout << "DESCENT";
      break;
    case State::LANDING:
      std::cout << "LANDING";
      break;
    case State::LANDED:
      std::cout << "LANDED";
      break;
    }
    std::cout << ", Time: " << elapsed_seconds_;
    std::cout << ", Alt: " << physics_.getAltitude() << "m";
    std::cout << ", VSpeed: " << physics_.getVerticalSpeed() << "m/s";
    std::cout << ", VAccel: " << physics_.getVerticalAcceleration() << "m/s^2";
    std::cout << std::endl;
  }

  State state_{State::PRE_LAUNCH};
  bst::Timer state_machine_timer_{K_UPDATE_INTERVAL_MS};
  bst::Timer print_timer_{K_PRINT_INTERVAL_MS};
  bst::Stopwatch flight_stopwatch_{};
  double elapsed_seconds_{0.0};

  Physics physics_{};

  void stateMachine() {
    switch (state_) {
    case State::PRE_LAUNCH:
      statePreLaunch();
      break;
    case State::ASCENT:
      stateAscent();
      break;
    case State::POP:
      statePop();
      break;
    case State::DESCENT:
      stateDescent();
      break;
    case State::LANDING:
      stateLanding();
      break;
    case State::LANDED:
      stateLanded();
      break;
    }
  }
};

#endif /* GFS_SIMULATOR_HPP_ */