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

void SamM8qExtension::stateConfigure() {
  if (transition_in_) {
    transition_in_ = false;
  }
}

void SamM8qExtension::stateReset() {
  if (transition_in_) {
    transition_in_ = false;
  }
}

void SamM8qExtension::stateRead() {
  if (transition_in_) {
    transition_in_ = false;
  }

  if (!ubx::pollMessage(i2c_, msg, ubx::kNavClass, ubx::kNavPvt, 92 + 8,
                        2000) ||
      !ubx::parsePVT(msg, nav_data)) { // Bad Data
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    ubx::flushStream(i2c_);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  GPSFrame gps_frame;

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

  sendData(gps_frame);
}

// ////////////////                          ////////////////
// ---------------- SAM-M8Q Helper Functions ----------------

bool SAMM8Q::configure() {
  ubx::ACK ack;

  if (ubx::getStreamSize(i2c_) !=
      0) { // Restart the device if it is already running
    ubx::sendResetCommand(i2c_);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    if (i2c_.readByteFromReg(0xFF) != 0) {
      if (i2c_.status() != interface::I2C_STATUS::OK) {
        error("UBX_RESTART");
        return false;
      }
    }
  }

  // Configure the device to be UBX only, check for ACK
  ack = ubx::setProtocolDDC(i2c_, false);
  if (ack != ubx::ACK::ACK) {
    error("CFG_PRT");
    return false;
  }

  //// Set the frequency of the messages, check for ACK (1 for every solution)
  // ack = ubx::setMessageRate(i2c_, kNavClass, kNavPvt, 1);
  // if (ack != ubx::ACK::ACK) {
  //     error("CFG_MSG");
  //     return false;
  // }

  // Set the measurement rate
  ack = ubx::setMeasurementRate(i2c_, 100);
  if (ack != ubx::ACK::ACK) {
    error("CFG_RATE");
    return false;
  }

  // Change the dynamic model (high altitude), this provides for high altitude
  // altitude data, as long as we stay below 1G of acceleration, check for ACK
  ack = ubx::setDynamicModel(i2c_, ubx::DYNAMIC_MODEL::AIRBORNE_1G);
  if (ack != ubx::ACK::ACK) {
    error("CFG_NAV5");
    return false;
  }

  /** @todo Read the configuration data and verify */
  return true;
}

} // namespace extension
