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

#include <BoosterSeat/time.hpp>

#include "samm8q.hpp"
#include "ubx_ids.hpp"
#include "ubx_protocol.hpp"

inline constexpr uint8_t kSamM8qI2cAddress = 0x42;
inline constexpr uint32_t kResetWaitTimerLength = 1000;
inline constexpr uint32_t kReadTimeout = 5000;

namespace extension {

SamM8qExtension::SamM8qExtension(ExtensionResources &resources,
                                 cfg::ExtensionMetadata metadata)
    : Extension(resources, metadata),
      i2c_(resources.i2c_bus, kSamM8qI2cAddress, resources.i2c_bus_lock),
      primary_watchdog_timer_((kReadTimeout + metadata.update_interval) * 4),
      read_watchdog_timer_(kReadTimeout + metadata.update_interval),
      reset_wait_timer_(kResetWaitTimerLength) {
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
    I2cInterface::Result connect_result;
    switch (start_state) {
    case StartupState::I2C_CONNECT:
      connect_result = i2c_.connect();
      if (connect_result == I2cInterface::Result::SUCCESS) {
        EXT_DEBUG("SAM-M8Q I2C connect success");
        start_state = StartupState::HANDSHAKE;
        break;
      }
      EXT_DEBUG("SAM-M8Q I2C connect failed");
      extSleep(kStartupRetryDelay);
      break;
    case StartupState::HANDSHAKE:
      if (handshake()) {
        start_state = StartupState::DONE;
        EXT_DEBUG("SAM-M8Q handshake success");
        break;
      }
      EXT_DEBUG("SAM-M8Q handshake failed");
      extSleep(kStartupRetryDelay);
      break;
    case StartupState::DONE: // Return, startup is complete
      EXT_DEBUG("SAM-M8Q startup complete");
      return;
    }

    if (stopRequested()) { // A stop was requested during startup
      return;
    }
  }

  // Startup timed out
  switch (start_state) {
  case StartupState::I2C_CONNECT:
    raiseFault(DiagnosticId::EXT_FAULT_samm8qI2CConnect);
    return;
  case StartupState::HANDSHAKE:
    raiseFault(DiagnosticId::EXT_FAULT_samm8qStartupHandshake);
    return;
  case StartupState::DONE:
    return;
  }
}

void SamM8qExtension::loop() {
  /**
   * @todo Check the primary watchdog timer
   */

  switch (state_) {
  case State::CONFIGURE:
    stateConfigure();
    break;
  case State::RESET:
    stateReset();
    break;
  case State::READ:
    stateRead();
    break;
  }
}

void SamM8qExtension::shutdown() {
  /**
   * @brief Reset?
   */
}

// ////////////////                       ////////////////
// ---------------- SAM-M8Q State Machine ----------------

void SamM8qExtension::transitionState(State new_state) {
  state_ = new_state;
  switch (new_state) {
  case State::CONFIGURE:
    EXT_DEBUG("SAM-M8Q state transition: CONFIGURE");
    break;
  case State::RESET:
    EXT_DEBUG("SAM-M8Q state transition: RESET");
    reset_state_ = ResetState::SEND_RESET;
    break;
  case State::READ:
    EXT_DEBUG("SAM-M8Q state transition: READ");
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
    ack = ubx::setProtocolDDC(i2c_, false);
    if (ack == ubx::ACK::ACK) {
      EXT_DEBUG("SAM-M8Q set protocol DDC success");
      break;
    }
    EXT_DEBUG("SAM-M8Q set protocol DDC failed");
    attempts++;
  }
  if (attempts == kCommandAttempts) {
    error(DiagnosticId::EXTENSION_samm8qConfigureSetProtocol);
    transitionState(State::RESET);
    return;
  }

  // Set the measurement rate
  attempts = 0;
  while (attempts < kCommandAttempts) {
    ack = ubx::setMeasurementRate(i2c_, 100);
    if (ack == ubx::ACK::ACK) {
      EXT_DEBUG("SAM-M8Q set measurement rate success");
      break;
    }
    EXT_DEBUG("SAM-M8Q set measurement rate failed");
    attempts++;
  }
  if (attempts == kCommandAttempts) {
    error(DiagnosticId::EXTENSION_samm8qConfigureSetMeasurementRate);
    transitionState(State::RESET);
    return;
  }

  // Change the dynamic model (high altitude), this provides for high altitude
  // altitude data, as long as we stay below 1G of acceleration, check for ACK
  attempts = 0;
  while (attempts < kCommandAttempts) {
    ack = ubx::setDynamicModel(i2c_, ubx::DYNAMIC_MODEL::AIRBORNE_1G);
    if (ack == ubx::ACK::ACK) {
      EXT_DEBUG("SAM-M8Q set dynamic model success");
      break;
    }
    EXT_DEBUG("SAM-M8Q set dynamic model failed");
    attempts++;
  }
  if (ack != ubx::ACK::ACK) {
    error(DiagnosticId::EXTENSION_samm8qConfigureSetDynamicModel);
    transitionState(State::RESET);
    return;
  }

  EXT_DEBUG("SAM-M8Q configuration complete");
  transitionState(State::READ);
  /** @todo Read the configuration data and verify */
}

void SamM8qExtension::stateReset() {
  constexpr uint32_t kMaxResetAttempts = 3;

  switch (reset_state_) {
  // Send a reset command, then wait for the sensor
  case ResetState::SEND_RESET:
    if (reset_attempts_ >= kMaxResetAttempts) {
      raiseFault(DiagnosticId::EXT_FAULT_samm8qResetFailure);
      return;
    }
    EXT_DEBUG("SAM-M8Q sending reset command");
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
      reset_attempts_ = 0;
      transitionState(State::CONFIGURE);
      EXT_DEBUG("SAM-M8Q reset complete");
    } else {
      reset_state_ = ResetState::SEND_RESET;
      EXT_DEBUG("SAM-M8Q reset failed");
    }
    break;
  }
}

void SamM8qExtension::stateRead() {
  if (read_watchdog_timer_.isDone()) {
    error(DiagnosticId::EXTENSION_samm8qReadTimeout);
    EXT_DEBUG("SAM-M8Q read timeout - resetting");
    transitionState(State::RESET);
    return;
  }

  ubx::UBXMessage msg;
  ubx::NavData nav_data;

  // I don't remember what this section actually accomplishes...
  // Just labeled with 'Bad Data', no clue what I meant by that.
  constexpr uint32_t kBadDataSleepTime = 200;
  if (!ubx::pollMessage(i2c_, msg, ubx::kNavClassId, ubx::kNavPvt, 92 + 8,
                        2000) ||
      !ubx::parsePVT(msg, nav_data)) {
    extSleep(kBadDataSleepTime);
    ubx::flushStream(i2c_);
    extSleep(kBadDataSleepTime);
    EXT_DEBUG("SAM-M8Q bad data");
    return;
  }

  read_watchdog_timer_.reset();

  data::GpsFrame gps_frame;

  switch (nav_data.fix_type) {
  case ubx::FixType::NO_FIX:
    gps_frame.fix = data::GpsFix::NO_FIX;
    break;
  case ubx::FixType::FIX_2D:
    gps_frame.fix = data::GpsFix::FIX_2D;
    break;
  case ubx::FixType::FIX_3D:
    gps_frame.fix = data::GpsFix::FIX_3D;
    break;
  default:
    gps_frame.fix = data::GpsFix::ERROR;
  }

  gps_frame.is_valid = nav_data.valid;

  gps_frame.gps_utc_time = BoosterSeat::time::dateAndTimeToTimePoint(
      nav_data.year, nav_data.month, nav_data.day, nav_data.hour + 1,
      nav_data.minute, nav_data.second);

  gps_frame.num_satellites = nav_data.num_satellites;
  gps_frame.latitude = nav_data.latitude;
  gps_frame.longitude = nav_data.longitude;
  gps_frame.horz_accuracy = nav_data.horz_accuracy;

  gps_frame.altitude = nav_data.altitude;
  gps_frame.vert_accuracy = nav_data.vert_accuracy;

  gps_frame.horizontal_speed = nav_data.ground_speed;
  gps_frame.speed_accuracy = nav_data.speed_accuracy;

  gps_frame.heading_of_motion = nav_data.heading_of_motion;
  gps_frame.heading_accuracy = nav_data.heading_accuracy;

  data(gps_frame);

  EXT_DEBUG("SAM-M8Q read complete");
}

// ////////////////                          ////////////////
// ---------------- SAM-M8Q Helper Functions ----------------

bool SamM8qExtension::handshake() {
  constexpr uint8_t kStreamSizeLsbRegister = 0xFE;
  uint8_t byte_buffer;
  auto result = i2c_.readByteFromReg(byte_buffer, kStreamSizeLsbRegister);
  return result == I2cInterface::Result::SUCCESS;
}

} // namespace extension
