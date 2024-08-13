/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   imu_extension_base.hpp
 * @brief  Base IMU Extension
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-02-19
 * @copyright  2024 (license to be defined)
 */

#ifndef IMU_EXTENSION_BASE_HPP_
#define IMU_EXTENSION_BASE_HPP_

#include <BoosterSeat/rolling_average.hpp>
#include <BoosterSeat/string_utils.hpp>
#include <BoosterSeat/timer.hpp>

#include "extension_base.hpp"
#include "giraffe_assert.hpp"

namespace extension {
/**
 * @brief The IMU timeout multiplier based on the update interval.
 */
inline constexpr uint32_t K_IMU_WATCHDOG_MULTIPLIER = 10;

inline constexpr size_t K_DEFAULT_REPORT_INTERVAL = 5;

class ImuExtensionBase : public Extension {

public:
  ImuExtensionBase(ExtensionResources &resources,
                   cfg::ExtensionMetadata metadata)
      : Extension(resources, metadata),
        imu_watchdog_(metadata.update_interval * K_IMU_WATCHDOG_MULTIPLIER) {

    if (bst::containsSubstring(metadata.extra_args, "ri=")) {
      auto pos = metadata.extra_args.find("ri=");
      metadata.extra_args = metadata.extra_args.substr(pos + 3);

      try {
        data_report_interval_ = std::stoul(metadata.extra_args);

      } catch (const std::invalid_argument &e) {
        error(DiagnosticId::EXT_FAULT_imuInvalidReportInterval,
              metadata.extra_args);
        data_report_interval_ = K_DEFAULT_REPORT_INTERVAL;
      }
    } else {
      data_report_interval_ = K_DEFAULT_REPORT_INTERVAL;
    }

    constexpr size_t K_MIN_REPORT_INTERVAL = 1;
    constexpr size_t K_MAX_REPORT_INTERVAL = 100;

    if (data_report_interval_ < K_MIN_REPORT_INTERVAL ||
        data_report_interval_ > K_MAX_REPORT_INTERVAL) {
      error(DiagnosticId::EXT_FAULT_imuInvalidReportInterval,
            "no fault, defaulting to 10");
      data_report_interval_ = 10;
    }

    accel_x_.setWindowSize(data_report_interval_);
    accel_y_.setWindowSize(data_report_interval_);
    accel_z_.setWindowSize(data_report_interval_);
  }

protected:
  virtual void imuStartup(){};
  virtual bool readImu() {
    giraffe_assert(false);
    return false;
  };
  virtual void imuShutdown(){};

  void reportAccelData(const double x, const double y, const double z) {
    accel_x_.addValue(x);
    accel_y_.addValue(y);
    accel_z_.addValue(z);
  }

  void reportGyroData(const double x, const double y, const double z) {
    gyro_x_.addValue(x);
    gyro_y_.addValue(y);
    gyro_z_.addValue(z);
  }

private:
  void startup() override {
    imuStartup();
    if (isFaulted()) {
      return;
    }
    imu_watchdog_.reset();
  }

  void loop() override {
    if (imu_watchdog_.isDone()) {
      raiseFault(DiagnosticId::EXT_FAULT_imuWatchdogTimeout);
      return;
    }
    if (!readImu()) {
      return;
    }

    data_report_counter_++;

    if (data_report_counter_ >= data_report_interval_) {
      data::ImuFrame imu_data;
      imu_data.x_angular_velocity = gyro_x_.getAverage();
      imu_data.y_angular_velocity = gyro_y_.getAverage();
      imu_data.z_angular_velocity = gyro_z_.getAverage();
      imu_data.x_acceleration = accel_x_.getAverage();
      imu_data.y_acceleration = accel_y_.getAverage();
      imu_data.z_acceleration = accel_z_.getAverage();
      data(imu_data);
      data_report_counter_ = 0;
    }

    imu_watchdog_.reset();
  }

  void shutdown() override {
  }

  bst::Timer imu_watchdog_;
  uint32_t data_report_interval_;
  uint32_t data_report_counter_{0};

  bst::RollingAverage<double> accel_x_{10};
  bst::RollingAverage<double> accel_y_{10};
  bst::RollingAverage<double> accel_z_{10};

  bst::RollingAverage<double> gyro_x_{10};
  bst::RollingAverage<double> gyro_y_{10};
  bst::RollingAverage<double> gyro_z_{10};
};

} // namespace extension

#endif /* IMU_EXTENSION_BASE_HPP_ */