/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   adc_interpolation.hpp
 * @brief  ADC Interpolation Utility
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-02-20
 * @copyright  2024 (license to be defined)
 */

#ifndef ADC_INTERPOLATION_HPP_
#define ADC_INTERPOLATION_HPP_

#include <unordered_map>

#include "configuration.hpp"
#include "logger.hpp"
#include "shared_data.hpp"

namespace data_middleware {

/**
 * @brief Data Module utility class for processing ADC data.
 */
class AdcInterpolation {
public:
  AdcInterpolation(data::SharedData &shared_data,
                   cfg::Configuration &configuration);
  ~AdcInterpolation() = default;

  /**
   * @brief Takes in a data packet containing raw ADC data and processes it
   * according to the ADC configuration.
   * @param data_packet - The data packet containing the ADC data.
   */
  void processDataPacket(const data::DataPacket &data_packet);

private:
  /**
   * @brief Takes in the raw ADC value and the ADC Value Configuration and
   * inserts a calculated percentage value into the adc data frame.
   * @param adc_cfg - The ADC configuration.
   * @param value - The raw ADC value.
   */
  void insertPercentageValue(const cfg::AdcConfig &adc_cfg, uint32_t value);

  /**
   * @brief Takes in the raw ADC value and the ADC Value Configuration and
   * inserts a calculated voltage divider value into the adc data frame.
   * @param adc_cfg - The ADC configuration.
   * @param value - The raw ADC value.
   */
  void insertVoltageDividerValue(const cfg::AdcConfig &adc_cfg, uint32_t value);

  /**
   * @brief Returns the ADC channel configuration for the given key. If one does
   * not exist, an error is logged and a basic configuration is added to the
   * map and returned.
   * @param key - The key to look up the ADC configuration.
   * extension_name:channel_number
   * @return const cfg::AdcConfig& - The ADC configuration.
   */
  const cfg::AdcConfig &getAdcConfig(const std::string &key);

  /**
   * @brief Maps extension_name:adc_channel to the ADC configuration.
   */
  std::unordered_map<std::string, cfg::AdcConfig> adc_map_;

  data::SharedData &shared_data_;
  cfg::Configuration &configuration_;
  giraffe::Logger logger_;
};

} // namespace data_middleware

#endif /* ADC_INTERPOLATION_HPP_ */