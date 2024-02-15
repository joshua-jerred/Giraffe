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

#include "extension_base.hpp"
#include "i2c_interface.hpp"

namespace extension {

class Bme280 : public Extension {
public:
  Bme280(ExtensionResources &resources, cfg::ExtensionMetadata metadata);

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

  /**
   * @brief Reads the 'ID' register on the BME280, which should return 0x60.
   * @return true - handshake successful
   * @return false - handshake failed
   */
  bool handshake();

  /**
   * @brief Configures the BME280. (Weather Monitoring config, Section
   * 3.5.1)
   * @details
   * Sensor Mode: Normal
   * Oversampling: 1x on all
   * IIR Filter: Off
   * Standby Time: 125ms
   *
   * @return true - configuration successful
   * @return false - configuration failed
   */
  bool configureSensor();

  /**
   * @brief Reads and stores the compensation data from the BME280.
   *
   * @return true - compensation data read successfully
   * @return false - failed to read compensation data
   */
  bool readCompensationData();

  /**
   * @brief Reads the raw environmental data from the BME280.
   *
   * The raw data (pressure, temperature, humidity) is stored in the
   * follow registers:
   *
   * Pressure: 0xF7, 0xF8, 0xF9
   * Temperature: 0xFA, 0xFB, 0xFC
   * Humidity: 0xFD, 0xFE
   *
   * You'll notice that all of these registers are sequential. This means that
   * all of the data can be read in a single I2C transaction.
   * (As recommended in section 4 of the datasheet)
   *
   * Pressure:
   * 0xF7 data[0] 7:0 MSB - 8 bits
   * 0xF8 data[1] 7:0 LSB - 8 bits
   * 0xF9 data[2] 7:4 XLSB - 4 bits
   * [19:12] [11:4] [3:0]
   *
   * Temperature:
   * 0xFA data[3] 7:0 MSB - 8 bits
   * 0xFB data[4] 7:0 LSB - 8 bits
   * 0xFC data[5] 7:4 XLSB - 4 bits
   * [19:12] [11:4] [3:0]
   *
   * Humidity:
   * 0xFD data[6] 7:0 MSB - 8 bits
   * 0xFE data[7] 7:0 LSB - 8 bits
   * [15:8] [7:0]
   *
   * @return true - environmental data read successfully
   * @return false - failed to read environmental data
   */
  bool readEnvironmentalData();

  /**
   * @brief Processes the raw environmental data into known units using the
   * compensation data.
   * @details This is directly copied from the BME280 datasheet, but with
   * different variable names.
   */
  bool processEnvironmentalData();

  /**
   * @brief Send the reset command to the sensor.
   */
  void resetSensor();

  I2cInterface i2c_;

  /**
   * @brief A timer that is used to read the sensor's compensation data at a
   * configured interval.
   */
  bst::Timer compensation_timer_;

  /**
   * @brief A timer used with loop() to verify that the sensor is functioning.
   */
  bst::Timer read_timer_;

  uint32_t raw_temperature_ = 0;
  uint32_t raw_pressure_ = 0;
  uint32_t raw_humidity_ = 0;

  double temperature_ = 0.0; // Celsius (°C)
  double pressure_ = 0.0;    // millibar (mbar)
  double humidity_ = 0.0;    // percent (0.0 - 100.0)

  CompensationData comp_data_{};
};

} // namespace extension

#endif /* BME280_HPP_ */