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
  /// @brief The label for the ADC channel. Must be unique.
  std::string label{};

  /// @brief How to interpret the ADC values.
  gEnum::AdcType type = gEnum::AdcType::RAW_COUNT;

  /// @brief Name of the extension that is producing the data.
  std::string extension_name{};

  /// @brief The channel number from the extension that is producing the data.
  uint8_t channel_number = 0;

  /// @brief The resolution of the ADC in bits.
  uint32_t resolution = 12;

  // Voltage Divider
  ///@brief The voltage reference of the ADC in millivolts.
  uint32_t voltage_reference = 3300;
  /// @brief The resistance of the first resistor in the voltage div in ohms.
  uint32_t resistor_1_value = 1000;
  /// @brief The resistance of the second resistor in the voltage div in ohms.
  uint32_t resistor_2_value = 1000;

  // Percentage
  /// @brief The maximum value to use when calculating the percentage.
  uint32_t percentage_max = 4095;
  /// @brief The minimum value to use when calculating the percentage.
  uint32_t percentage_min = 0;
  /// @brief Whether or not to clamp the percentage to 0-100.
  bool percentage_clamp = false;

  Json getJson() const;

  void fromJson(const Json &json, data::LogStream &log_stream,
                const std::string &index);
};

class AdcMappings : public CfgSection {
public:
  AdcMappings(data::Streams &streams) : cfg::CfgSection(streams) {
  }

  void setFromJson(const Json &json_data);
  Json getJson() const;

  void addAdcConfig(const AdcConfig &adc_config);
  void removeAdcConfig(const std::string &label);
  std::vector<AdcConfig> getAdcConfigs() const;

private:
  bool doesLabelExist(const std::string &label) const;

  std::vector<AdcConfig> adc_configs_{};
};

} // namespace cfg

#endif /* CFG_ADC_MAPPING_HPP_ */