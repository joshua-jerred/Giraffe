/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   i2c_extension_adapter.hpp
 * @brief  Adapter for I2C Extensions
 * @warning This is currently not implemented!
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-05
 * @copyright  2023 (license to be defined)
 */

#ifndef I2C_EXTENSION_ADAPTER_HPP_
#define I2C_EXTENSION_ADAPTER_HPP_

#include <BoosterSeat/sleep.hpp>
#include <BoosterSeat/timer.hpp>

#include "extension_base.hpp"
#include "i2c_interface.hpp"

namespace extension {

inline constexpr uint32_t kDefaultI2cHandshakeTimeoutMs = 1000;
inline constexpr uint32_t kDefaultI2cHandshakeRetryDelayMs = 100;

/**
 * @brief Adapter for I2C Extensions
 * @warning This is currently not implemented!
 */
class I2cExtensionAdapter {
protected:
  I2cExtensionAdapter(
      ExtensionResources &resources, cfg::ExtensionMetadata &metadata,
      uint8_t i2c_address,
      uint32_t i2c_handshake_timeout_ms = kDefaultI2cHandshakeTimeoutMs,
      uint32_t i2c_handshake_retry_delay_ms = kDefaultI2cHandshakeRetryDelayMs)
      : i2c_(resources.i2c_bus, i2c_address, resources.i2c_bus_lock),
        handshake_timer_(i2c_handshake_timeout_ms),
        retry_delay_ms_(i2c_handshake_retry_delay_ms) {
    (void)metadata;
  }
  virtual ~I2cExtensionAdapter() = default;

  virtual bool i2cHandshake() = 0;

  bool connectToI2cBus(DiagnosticId &error) {
    auto result = i2c_.connect();
    handshake_timer_.reset();
    if (result == I2cInterface::Result::SUCCESS) {
      while (!i2cHandshake()) {
        if (handshake_timer_.isDone()) {
          error = DiagnosticId::EXT_FAULT_i2cHandshakeTimeoutFault;
          return false;
        }
        bst::sleep(retry_delay_ms_);
      }
      return true;
    }

    if (result == I2cInterface::Result::BUS_OPEN_ERROR) {
      error = DiagnosticId::EXT_FAULT_i2cBusConnectFault;
    } else if (result == I2cInterface::Result::ADDRESS_SET_ERROR) {
      error = DiagnosticId::EXT_FAULT_i2cAddressSetFault;
    } else {
      error = DiagnosticId::EXT_FAULT_i2cUnknownFault;
    }

    return false;
  }

  I2cInterface i2c_;

private:
  bst::Timer handshake_timer_;
  uint32_t retry_delay_ms_;
};

} // namespace extension

#endif /* I2C_EXTENSION_ADAPTER_HPP_ */
