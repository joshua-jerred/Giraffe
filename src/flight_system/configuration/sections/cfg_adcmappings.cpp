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

void AdcConfig::fromJson(const Json &json, data::LogStream &log) {
  const std::string section_name = "adc_mappings";

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

  // Ensure that the extensions are an array.
  if (!json.is_array()) {
    error(DiagnosticId::CONFIG_extensionsNotArray);
    return;
  }

  // Iterate over the extensions.
  int num_attempted = 0;
  for (const Json &ext_meta_j : j) {
    if (!ext_meta_j.is_object()) {
      error(DiagnosticId::CONFIG_extensionNotObject,
            std::to_string(num_attempted++));
      continue;
    }

    // Create the extension metadata.
    ExtensionMetadata ext_meta;
    std::string index = std::to_string(num_attempted);
    ext_meta.setFromJson(ext_meta_j, streams_.log, index);

    // Check if the extension name is already in use.
    if (doesNameExist(ext_meta.name)) {
      error(DiagnosticId::CONFIG_extensionNameAlreadyExists,
            "on load " + ext_meta.name);
      continue;
    }

    // Add the extension metadata to the list.
    extensions_.push_back(ext_meta);
    num_attempted++;
  }
}

Json AdcMappings::getJson() const {
  return Json::object({{"adc_mappings", Json::array({})}});
}

void AdcMappings::addAdcConfig(const AdcConfig &adc_config) {
}

void AdcMappings::removeAdcConfig(const std::string &label) {
}

std::vector<AdcConfig> AdcMappings::getAdcConfigs() const {
}

bool AdcMappings::doesLabelExist(const std::string &label) const {
  return false;
}

} // namespace cfg