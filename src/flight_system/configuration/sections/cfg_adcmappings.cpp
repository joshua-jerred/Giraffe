/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   cfg_adcmappings.cpp
 * @brief  ADC Mapping configuration section.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-02-20
 * @copyright  2024 (license to be defined)
 */

#include "cfg_adcmappings.hpp"

#include "validation.hpp"

namespace cfg {

Json AdcConfig::getJson() const {
  return Json::object({{"label", label},
                       {"type", gEnum::K_ADC_TYPE_TO_STRING_MAP.at(type)},
                       {"extension_name", extension_name},
                       {"channel_number", channel_number},
                       {"resolution", resolution},
                       {"voltage_reference", voltage_reference},
                       {"resistor_1_value", resistor_1_value},
                       {"resistor_2_value", resistor_2_value},
                       {"percentage_max", percentage_max},
                       {"percentage_min", percentage_min},
                       {"percentage_clamp", percentage_clamp}});
}

void AdcConfig::fromJson(const Json &json, data::LogStream &log,
                         const std::string &index) {
  const std::string section_name = "adc_mappings." + index;

  validation::setValidValue<std::string>(log, json, section_name, "label",
                                         label, 3, 10, "");
  validation::setValidEnum<gEnum::AdcType>(
      log, json, section_name, "type", type, gEnum::K_STRING_TO_ADC_TYPE_MAP);
  validation::setValidValue<std::string>(log, json, extension_name,
                                         "extension_name", label, 3, 10, "");
  validation::setValidValue<uint8_t>(log, json, section_name, "channel_number",
                                     channel_number, 0, 12, "");
  validation::setValidValue<uint32_t>(log, json, section_name, "resolution",
                                      resolution, 3, 32, "");
  validation::setValidValue<uint32_t>(log, json, section_name,
                                      "voltage_reference", voltage_reference, 0,
                                      20000, "");
  validation::setValidValue<uint32_t>(log, json, section_name,
                                      "resistor_1_value", resistor_1_value, 0,
                                      1000000, "");
  validation::setValidValue<uint32_t>(log, json, section_name,
                                      "resistor_2_value", resistor_2_value, 0,
                                      1000000, "");
  validation::setValidValue<uint32_t>(log, json, section_name, "percentage_max",
                                      percentage_max, 0, 65536, "");
  validation::setValidValue<uint32_t>(log, json, section_name, "percentage_min",
                                      percentage_min, 0, 65536, "");
  validation::setValidValue<bool>(log, json, section_name, "percentage_clamp",
                                  percentage_clamp, 0, 0, "");
}

void AdcMappings::setFromJson(const Json &json_data) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);

  // Ensure that the adc mappings are in an array.
  if (!json_data.is_array()) {
    error(DiagnosticId::CONFIG_adcMappingsNotArray);
    return;
  }

  // Iterate over the ADC Mappings.
  int num_attempted = 0;
  for (const Json &adc_mapping_json : json_data) {
    if (!adc_mapping_json.is_object()) {
      error(DiagnosticId::CONFIG_adcMappingNotObject,
            std::to_string(num_attempted++));
      continue;
    }

    // Create the ADC Mapping configuration.
    AdcConfig adc_config;
    std::string index = std::to_string(num_attempted);
    adc_config.fromJson(adc_mapping_json, streams_.log, index);

    // Check if the extension name is already in use.
    if (doesLabelExist(adc_config.label)) {
      error(DiagnosticId::CONFIG_adcMappingLabelAlreadyExists,
            "on load " + adc_config.label);
      continue;
    }

    // Add the extension metadata to the list.
    adc_configs_.push_back(adc_config);
    num_attempted++;
  }
}

Json AdcMappings::getJson() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  Json j = Json::array();
  for (const AdcConfig &adc_config : adc_configs_) {
    j.push_back(adc_config.getJson());
  }
  return j;
}

void AdcMappings::addAdcConfig(const AdcConfig &adc_config) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  if (doesLabelExist(adc_config.label)) {
    error(DiagnosticId::CONFIG_adcMappingLabelAlreadyExists,
          "on add " + adc_config.label);
    return;
  }
  adc_configs_.push_back(adc_config);
}

void AdcMappings::removeAdcConfig(const std::string &label) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  for (auto it = adc_configs_.begin(); it != adc_configs_.end(); it++) {
    if (it->label == label) {
      adc_configs_.erase(it);
      return;
    }
  }
  error(DiagnosticId::CONFIG_adcMappingLabelAlreadyExists,
        "on remove " + label);
}

std::vector<AdcConfig> AdcMappings::getAdcConfigs() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return adc_configs_;
}

bool AdcMappings::doesLabelExist(const std::string &label) const {
  for (const AdcConfig &adc_config : adc_configs_) {
    if (adc_config.label == label) {
      return true;
    }
  }
  return false;
}

} // namespace cfg