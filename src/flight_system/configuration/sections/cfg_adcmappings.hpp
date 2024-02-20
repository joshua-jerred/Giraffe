/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   cfg_adc_mapping.hpp
 * @brief  ADC Mapping configuration section.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-02-20
 * @copyright  2024 (license to be defined)
 */

#ifndef CFG_ADC_MAPPING_HPP_
#define CFG_ADC_MAPPING_HPP_

#include "configuration_enums.hpp"
#include "sections/cfg_section.hpp"

namespace cfg {

/// @brief Configuration for an ADC.
/// @see adc_config.md
struct AdcConfig {
  /// @brief ADC Types, a way to specify how to interpret the ADC values.
  enum AdcType { RAW_COUNT, PERCENTAGE, VOLTAGE_DIVIDER, VOLTAGE_REFERENCE };

  /// @brief How to interpret the ADC values.
  AdcType type = AdcType::RAW_COUNT;

  /// @brief The resolution of the ADC in bits.
  uint32_t resolution = 12;

  // Voltage Divider
  ///@brief The voltage reference of the ADC in millivolts.
  uint32_t voltage_reference = 3300;
  /// @brief The resistance of the first resistor in the voltage div in ohms.
  uint32_t resistor_1 = 1000;
  /// @brief The resistance of the second resistor in the voltage div in ohms.
  uint32_t resistor_2 = 1000;

  // Percentage
  /// @brief The maximum value to use when calculating the percentage.
  uint32_t max = 4095;
  /// @brief The minimum value to use when calculating the percentage.
  uint32_t min = 0;
  /// @brief Whether or not to clamp the percentage to 0-100.
  bool clamp = false;
}

} // namespace cfg

#endif /* CFG_ADC_MAPPING_HPP_ */