/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   bme280.cpp
 * @brief  BME280 sensor extension
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-02
 * @copyright  2023 (license to be defined)
 */

#include "bme280.hpp"
#include "extension_error_counter.hpp"

namespace extension {

inline constexpr uint8_t kBme280I2cAddress = 0x76;
inline constexpr int kCompensationDataReadRate = 5000; // ms

Bme280Extension::Bme280Extension(ExtensionResources &resources,
                                 cfg::ExtensionMetadata metadata)
    : Extension(resources, metadata),
      i2c_(resources.i2c_bus, kBme280I2cAddress, resources.i2c_bus_lock),
      compensation_timer_(kCompensationDataReadRate) {
}

enum class StartupState {
  I2C_CONNECT,
  HANDSHAKE,
  CONFIGURE,
  READ_COMPENSATION_DATA,
  DONE
};

void Bme280Extension::startup() {
  constexpr uint32_t kStartupTimeout = 5000; // ms
  BoosterSeat::Timer startup_timer(kStartupTimeout);
  startup_timer.reset();

  StartupState state = StartupState::I2C_CONNECT;

  while (!startup_timer.isDone()) {
    switch (state) {
    case StartupState::I2C_CONNECT:
      auto connect_result = i2c_.connect();
      if (connect_result == I2cInterface::Result::SUCCESS) {
        state = StartupState::HANDSHAKE;
      }
      break;
    case StartupState::HANDSHAKE:
      if (handshake()) {
        state = StartupState::CONFIGURE;
      }
      break;
    case StartupState::CONFIGURE:
      if (configureSensor()) {
        state = StartupState::READ_COMPENSATION_DATA;
      }
      break;
    case StartupState::READ_COMPENSATION_DATA:
      if (readCompensationData()) {
        state = StartupState::DONE;
      }
      break;
    case StartupState::DONE: // Return, startup is complete
      return;
    }

    if (stopRequested()) { // A stop was requested during startup
      return;
    }
  }

  // Startup timed out
  switch (state) {
  case StartupState::I2C_CONNECT:
    raiseFault(DiagnosticId::EXT_FAULT_bme280StartupI2CConnect);
    break;
  case StartupState::HANDSHAKE:
    raiseFault(DiagnosticId::EXT_FAULT_bme280StartupHandshake);
    break;
  case StartupState::CONFIGURE:
    raiseFault(DiagnosticId::EXT_FAULT_bme280StartupConfigure);
    break;
  case StartupState::READ_COMPENSATION_DATA:
    raiseFault(DiagnosticId::EXT_FAULT_bme280StartupReadCompensation);
    break;
  case StartupState::DONE: // Return, startup is complete

    return;
  }
}

void Bme280Extension::loop() {
  if (read_timer_.isDone()) {
    raiseFault(DiagnosticId::EXT_FAULT_bme280ReadTimeout);
  }

  if (compensation_timer_.isDone()) {
    if (!readCompensationData()) {
      // TODO
      return;
    }
    if (!readEnvironmentalData()) {
      // TODO
      return;
    }
    if (!processEnvironmentalData()) {
      // TODO
      return;
    }
    error_counter_ = 0;
  }
}

} // namespace extension