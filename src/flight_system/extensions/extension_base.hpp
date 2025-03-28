/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   extension_base.hpp
 * @brief  The new implementation of the extension system.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-12
 * @copyright  2023 (license to be defined)
 */

#ifndef EXTENSION_BASE_HPP_
#define EXTENSION_BASE_HPP_

#include <atomic>
#include <thread>

#include "sections/cfg_extensions.hpp"
#include "shared_data.hpp"
#include "status_led.hpp"

namespace extension {

/**
 * @brief This contains objects that are shared between all extensions.
 */
struct ExtensionResources {
  ExtensionResources(data::Streams &streams, cfg::gEnum::I2CBus i2c_bus,
                     giraffe::StatusLedState &status_led)
      : streams(streams), i2c_bus(i2c_bus), status_led(status_led) {
  }

  /**
   * @brief The global data/log streams.
   */
  data::Streams &streams;

  /**
   * @brief The mutex for the I2C bus.
   * @details The flight system currently supports only one I2C bus.
   * This is to prevent multiple extensions from trying to use the
   * bus at the same time.
   */
  std::mutex i2c_bus_lock{};

  /**
   * @brief The bus to use for I2C communication.
   */
  cfg::gEnum::I2CBus i2c_bus;

  /**
   * @brief The status LED.
   */
  giraffe::StatusLedState &status_led;
};

/**
 * @brief
 * @details Many of the things in here are nearly identical to
 * modules. However, there are few subtle differences.
 *
 * Modules are unique, you will only every have one "server" module.
 * Extensions are not. You may have multiple BMP280 sensors, for example.
 * As extensions are identified by name, the handling of the data
 * packets is a bit different. Along with that, extensions do not have
 * direct access to 'shared_data' as modules do. Instead, they all have
 * access to the data/log stream, but only through the data(), error(),
 * and info() functions.
 *
 * Extensions do not need to be written in a thread safe manner as they
 * are run in their own thread and their only interface is through
 * methods that are thread safe.
 */
class Extension {
public:
  Extension(ExtensionResources &resources, cfg::ExtensionMetadata metadata);
  Extension(const Extension &) = delete;            // no copy constructor
  Extension &operator=(const Extension &) = delete; // no copy assignment

  /**
   * @brief This will attempt to stop the extension if it is running, but with
   * a configured timeout. @see kExtensionDestroyTimeoutMs
   *
   * @details This really shouldn't be used, but it is here to silence a
   * warning and it's there if needed.
   */
  virtual ~Extension();

  /**
   * @brief Starts the extension.
   */
  void start();

  /**
   * @brief Sends a stop signal to the extension.
   */
  void stop();

  /**
   * @brief Only to be called when an extension is in the Error state. This
   * joins the thread and fixes the status.
   *
   */
  void reset();

  /**
   * @brief Get the Status of the extension.
   * @return node::Status The status of the extension.
   */
  node::Status getStatus() const {
    return status_;
  }

  /**
   * @brief Get the Fault Code when the status is ERROR
   * @return DiagnosticId The fault code.
   */
  DiagnosticId getFaultCode() const {
    return fault_code_;
  }

  /**
   * @brief Returns true if the extension is in a faulted state.
   * @return true if the extension is in a faulted state, false otherwise.
   */
  bool isFaulted() const {
    return fault_flag_;
  }

protected:
  virtual void startup(){};
  virtual void loop(){};
  virtual void shutdown(){};

  template <typename T>
  void data(data::DataId identifier, T value, int precision = 2);
  // void data(data::DataId identifier, std::string string_data);
  void data(data::GpsFrame frame);
  void data(data::ImuFrame frame);
  void dataWithSecondaryIdentifier(data::DataId identifier, uint32_t value,
                                   const std::string &secondary_identifier);
  void error(DiagnosticId, const std::string &info = "");
  void error(DiagnosticId log_id, int info);
  void info(std::string info);
  void debug(const std::string &message);

  /**
   * @brief Returns true if the extension has been requested to stop.
   * @details This should be used in the loop() function for extensions that
   * take a long time to complete a single loop iteration. This is helpful
   * for to stop extensions quickly upon request. Fast extensions do not need to
   * use this.
   *
   * @return true - The extension has been requested to stop.
   * @return false - The extension has not been requested to stop.
   */
  bool stopRequested() const;

  /**
   * @brief Used internally to raise a fault.
   * This will stop an extension.
   */
  void raiseFault(DiagnosticId ext_fault_code, std::string info = "");

  /**
   * @brief Sleep internally for a given number of milliseconds.
   * @details This has a limit of 1 second.
   *
   * @param milliseconds - The number of milliseconds to sleep.
   */
  void extSleep(uint32_t milliseconds);

  cfg::ExtensionMetadata metadata_;

private:
  void sleep();
  void runner();

  /**
   * @brief This is raised with a call to raiseFault().
   * @details This is used to stop the extension. Cleared on a call to
   * start().
   */
  std::atomic<bool> fault_flag_ = false;

  /**
   * @brief The fault code associated with the fault.
   * @details This is set when raiseFault() is called. Cleared on a call to
   * start().
   */
  std::atomic<DiagnosticId> fault_code_ = DiagnosticId::EXT_FAULT_none;

  std::atomic<node::Status> status_ = node::Status::STOPPED;
  std::atomic<bool> stop_flag_ = true;
  std::thread runner_thread_ = std::thread();

  ExtensionResources &interfaces_;
};
} // namespace extension

#endif /* EXTENSION_HPP_ */