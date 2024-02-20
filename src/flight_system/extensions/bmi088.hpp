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
#include "imu_extension_base.hpp"
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
class Bmi088 : public ImuExtensionBase {
public:
  Bmi088(ExtensionResources &resources, cfg::ExtensionMetadata metadata);
  ~Bmi088();

  void imuStartup() override;
  bool readImu() override;

  enum AccPowerConf : uint8_t {
    ACC_ACTIVE_MODE = 0x00,
    ACC_SUSPEND_MODE = 0x03
  };

  enum AccPowerCtrl : uint8_t { ACC_OFF = 0x00, ACC_ON = 0x04 };

  enum AccRange : uint8_t {
    RANGE_3G = 0x00,
    RANGE_6G = 0x01,
    RANGE_12G = 0x02,
    RANGE_24G = 0x03
  };

  /**
   * @brief Output data rate for the BMI088 accelerometer in Hz.
   */
  enum AccOutputDataRate : uint8_t {
    ODR_12_5 = 0x05,
    ODR_25 = 0x06,
    ODR_50 = 0x07,
    ODR_100 = 0x08,
    ODR_200 = 0x09,
    ODR_400 = 0x0A,
    ODR_800 = 0x0B,
    ODR_1600 = 0x0C
  };

  /**
   * @brief Bandwidth parameter for the BMI088 accelerometer. Shifted left by 4
   * bits as it's OR'd with the output data rate.
   */
  enum AccBwp : uint8_t {
    BWP_OSR4 = (0x00 << 4),
    BWP_OSR2 = (0x01 << 4),
    BWP_NORMAL = (0x02 << 4)
  };

private:
  bool accHandshake();
  void accSoftReset();
  void accSetRange(AccRange range);
  void accSetConf(AccOutputDataRate odr, AccBwp bwp);
  void accSetPower(bool on);
  bool configureAccelerometer();
  bool readAccelData();

  bool gyroHandshake();

  bool configureGyro();

  bool readGyroData();

  bool accWriteByte(uint8_t register_addr, uint8_t value);
  bool gyroWriteByte(uint8_t register_addr, uint8_t value);
  // bool accRead(uint8_t register_addr, uint8_t &data);

  std::string accel_address_{};
  std::string gyro_address_{};

  giraffe::SpiInterface spi_accel_;
  giraffe::SpiInterface spi_gyro_;

  AccRange acc_range_{AccRange::RANGE_6G};
  double acc_range_value_{6000};
};

} // namespace extension

#endif /* BMI088_HPP_ */