/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   bmi088.hpp
 * @brief  Extension for the Bosch BMI088 IMU.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-26
 * @copyright  2024 (license to be defined)
 */

#ifndef BMI088_HPP_
#define BMI088_HPP_

#include "extension_base.hpp"
#include "spi_interface.hpp"

#include <BoosterSeat/timer.hpp>

namespace extension {

/**
 * @brief The BMI088 extension.
 * @details This extension reads data from the Bosch BMI088 IMU. This extension
 * uses the SPI interface to communicate with the IMU. The extension metadata
 * must include the SPI device path for both the accelerometer and the
 * gyroscope, separated by a comma. "<accelerometer>/<gyro>" For example:
 * extra_args: "/dev/spidev0.0,/dev/spidev0.1"
 *
 * @todo Accelerometer data.
 */
class Bmi088 : public Extension {
public:
  Bmi088(ExtensionResources &resources, cfg::ExtensionMetadata metadata);
  ~Bmi088();

  void startup() override;
  void loop() override;

private:
  bool accelerometerHandshake();
  bool gyroHandshake();

  bool configureAccelerometer();
  bool configureGyro();

  bool readGyroData();
  bool readAccelData();

  std::string accel_address_{};
  std::string gyro_address_{};

  giraffe::SpiInterface spi_accel_;
  giraffe::SpiInterface spi_gyro_;

  bst::Timer read_timeout_;

  data::ImuFrame imu_data_{};
};

} // namespace extension

#endif /* BMI088_HPP_ */