/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   bme280.hpp
 * @brief  The BME280 sensor extension
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-02
 * @copyright  2023 (license to be defined)
 */

#ifndef BME280_HPP_
#define BME280_HPP_

#include <BoosterSeat/timer.hpp>

#include "extension.hpp"
#include "i2c_interface.hpp"

namespace extension {

class Bme280Extension : public Extension {
public:
  Bme280Extension(ExtensionResources &resources,
                  cfg::ExtensionMetadata metadata);

  void startup() override;
  void loop() override;

private:
  struct CompensationData {
    // Temperature compensation
    uint16_t dig_T1 = 0;
    int16_t dig_T2 = 0;
    int16_t dig_T3 = 0;

    // Pressure compensation
    uint16_t dig_P1 = 0;
    int16_t dig_P2 = 0;
    int16_t dig_P3 = 0;
    int16_t dig_P4 = 0;
    int16_t dig_P5 = 0;
    int16_t dig_P6 = 0;
    int16_t dig_P7 = 0;
    int16_t dig_P8 = 0;
    int16_t dig_P9 = 0;

    // Humidity compensation
    uint8_t dig_H1 = 0;
    int16_t dig_H2 = 0;
    uint8_t dig_H3 = 0;
    int16_t dig_H4 = 0;
    int16_t dig_H5 = 0;
    int8_t dig_H6 = 0;
  };

  bool handshake();

  /**
   * @brief Attempts to reset the sensor.
   * @details Includes the startup delay.
   *
   * @return true - if the reads/writes were successful
   * @return false - if unsuccessful
   */
  bool resetSensor();
  bool configureSensor();

  bool readCompensationData();
  bool readEnvironmentalData();
  bool processEnvironmentalData();

  I2cInterface i2c_;

  /**
   * @brief A timer that is used to read the sensor's compensation data at a
   * configured interval.
   */
  BoosterSeat::Timer compensation_timer_;

  /**
   * @brief A timer used with loop() to verify that the sensor is functioning.
   */
  BoosterSeat::Timer read_timer_;
};

} // namespace extension

#endif /* BME280_HPP_ */