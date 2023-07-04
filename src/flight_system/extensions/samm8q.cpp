/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   samm8q.cpp
 * @brief  The SAM-M8Q GPS extension (I2C).
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-04
 * @copyright  2023 (license to be defined)
 */

#include "samm8q.hpp"
#include "ubx_protocol.hpp"

inline constexpr uint8_t kSamM8qI2cAddress = 0x42;
inline constexpr uint32_t kResetWaitTimerLength = 1000;

namespace extension {

SamM8qExtension::SamM8qExtension(ExtensionResources &resources,
                                 cfg::ExtensionMetadata metadata)
    : Extension(resources, metadata),
      i2c_(resources.i2c_bus, kSamM8qI2cAddress, resources.i2c_bus_lock),
      read_timer_(kReadTimeout + metadata.update_interval) {
}

// ////////////////                                ////////////////
// ---------------- Extension Base Class Overrides ----------------

enum class StartupState { I2C_CONNECT, HANDSHAKE, DONE };
void SamM8qExtension::startup() {
  constexpr uint32_t kStartupTimeout = 5000;   // ms
  constexpr uint32_t kStartupRetryDelay = 400; // ms

  BoosterSeat::Timer startup_timer(kStartupTimeout);
  startup_timer.reset();

  StartupState start_state = StartupState::I2C_CONNECT;

  while (!startup_timer.isDone()) {
    switch (start_state) {
    case StartupState::I2C_CONNECT:
      connect_result = i2c_.connect();
      if (connect_result == I2cInterface::Result::SUCCESS) {
        state = StartupState::HANDSHAKE;
        break;
      }
      extSleep(kStartupRetryDelay);
      break;
    case StartupState::HANDSHAKE:
      if (handshake()) {
        state = StartupState::CONFIGURE;
        break;
      }
      extSleep(kStartupRetryDelay);
      break;
    case StartupState::DONE: // Return, startup is complete
      return;
    }

    if (stopRequested()) { // A stop was requested during startup
      return;
    }
  }

  // Startup timed out
  switch (start_state) {
  case StartupState::I2C_CONNECT:
    raiseFault(DiagnosticId::);
    break;
  case StartupState::HANDSHAKE:
    raiseFault(DiagnosticId::);
    break;
  case StartupState::DONE:
    return;
  }
}

void SamM8qExtension::loop() {
}

void SamM8qExtension::shutdown() {
}

// ////////////////                       ////////////////
// ---------------- SAM-M8Q State Machine ----------------

void SamM8qExtension::transitionState(State new_state) {
  switch (new_state) {
  case State::CONFIGURE:
    break;
  case State::RESET:
    reset_state_ = ResetState::SEND_RESET;
    reset_watchdog_timer_.reset();
    break;
  case State::READ:
    read_state_ = ReadState::WAIT;
    read_watchdog_timer_.reset();
    break;
  }
}

void SamM8qExtension::stateConfigure() {
  /**
   * @brief Allow each command to be sent up to 3 times before giving up.
   */
  constexpr uint32_t kCommandAttempts = 3;

  ubx::ACK ack;

  // Configure the device to be UBX only, check for ACK
  uint32_t attempts = 0;
  while (attempts < kCommandAttempts) {
    ack = ubx::setProtocolUBX(i2c_, false);
    if (ack == ubx::ACK::ACK) {
      break;
    }
    attempts++;
  }
  if (attempts == kCommandAttempts) {
    error(DiagnosticId::);
    transitionState(State::RESET);
    return;
  }

  // Set the measurement rate
  attempts = 0;
  while (attempts < kCommandAttempts) {
    ack = ubx::setMeasurementRate(i2c_, 100);
    if (ack == ubx::ACK::ACK) {
      break;
    }
    attempts++;
  }
  if (attempts == kCommandAttempts) {
    error(DiagnosticId::);
    transitionState(State::RESET);
    return;
  }

  // Change the dynamic model (high altitude), this provides for high altitude
  // altitude data, as long as we stay below 1G of acceleration, check for ACK
  attempts = 0;
  while (attempts < kCommandAttempts) {
    ack = ubx::setDynamicModel(i2c_, ubx::DYNAMIC_MODEL::AIRBORNE_1G);
    if (ack == ubx::ACK::ACK) {
      break;
    }
    attempts++;
  }
  if (ack != ubx::ACK::ACK) {
    error(DiagnosticId::);
    transitionState(State::RESET);
    return;
  }

  transitionState(State::READ);
  /** @todo Read the configuration data and verify */
}

void SamM8qExtension::stateReset() {
  inline constexpr uint32_t kMaxResetAttempts = 3;

  switch (reset_state_) {
  // Send a reset command, then wait for the sensor
  case ResetState::SEND_RESET:
    if (reset_attempts_ >= kMaxResetAttempts) {
      raiseFault(DiagnosticId::);
      return;
    }
    ubx::sendResetCommand(i2c_);
    reset_wait_timer_.reset();
    reset_state_ = ResetState::WAIT;
    reset_attempts_++;
    break;

  // Wait for the sensor to reset
  case ResetState::WAIT:
    if (reset_wait_timer_.isDone()) {
      reset_state_ = ResetState::HANDSHAKE;
    }
    break;

  // Try to handshake with the sensor
  case ResetState::HANDSHAKE:
    if (handshake()) {
      restart_attempts_ = 0;
      transitionState(State::CONFIGURE);
    } else {
      reset_state_ = ResetState::SEND_RESET;
    }
    break;
  }
}

void SamM8qExtension::stateRead() {
  if (read_watchdog_timer_.isDone()) {
    error(DiagnosticId::);
    transitionState(State::RESET);
    return;
  }

  // I don't remember what this section actually accomplishes...
  // Just labeled with 'Bad Data', no clue what I meant by that.
  constexpr uint32_t kBadDataSleepTime = 200;
  if (!ubx::pollMessage(i2c_, msg, ubx::kNavClass, ubx::kNavPvt, 92 + 8,
                        2000) ||
      !ubx::parsePVT(msg, nav_data)) {
    extSleep(kBadDataSleepTime);
    ubx::flushStream(i2c_);
    extSleep(kBadDataSleepTime);
  }

  data::GPSFrame gps_frame;

  gps_frame.time = std::to_string(nav_data.hour) + ":" +
                   std::to_string(nav_data.minute) + ":" +
                   std::to_string(nav_data.second);

  gps_frame.num_satellites = nav_data.num_satellites;
  gps_frame.latitude = nav_data.latitude;
  gps_frame.longitude = nav_data.longitude;
  gps_frame.horz_accuracy = nav_data.horz_accuracy;

  gps_frame.altitude = nav_data.altitude;
  gps_frame.vert_accuracy = nav_data.vert_accuracy;

  gps_frame.ground_speed = nav_data.ground_speed;
  gps_frame.speed_accuracy = nav_data.speed_accuracy;

  gps_frame.heading_of_motion = nav_data.heading_of_motion;
  gps_frame.heading_accuracy = nav_data.heading_accuracy;

  data(gps_frame);
}

// ////////////////                          ////////////////
// ---------------- SAM-M8Q Helper Functions ----------------

} // namespace extension
