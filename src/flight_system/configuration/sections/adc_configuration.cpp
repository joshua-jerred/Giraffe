/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   adc_configuration.cpp
 * @brief  ADC Configuration through 'extra_args'
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-05
 * @copyright  2023 (license to be defined)
 */

#include "adc_configuration.hpp"

namespace extension {

inline std::string getKeyValue(const std::string &config_string,
                               const std::string &key) {
  // find the key
  auto value_start = config_string.find(key + "=");
  if (value_start == std::string::npos) {
    return "";
  }

  // find the end of the value
  auto value_end = config_string.find(",", value_start);
  if (value_end == std::string::npos) {
    value_end = config_string.length();
  }

  return config_string.substr(value_start + key.length() + 1,
                              value_end - value_start - key.length() - 1);
}

inline bool parsePercentAdc(std::string config_string, AdcConfig &out_config) {
  constexpr int min_length = 12;

  if (config_string.length() == 12) {
    out_config.min = 0;
    out_config.max = (1 << out_config.resolution) - 1;
    return true;
  }

  std::string min = getKeyValue(config_string, "min");
  std::string max = getKeyValue(config_string, "max");
  std::string clamp = getKeyValue(config_string, "clamp");

  // Make sure min and max are numbers (non-negative)

  int min_int = 0;
  int max_int = (1 << out_config.resolution) - 1;

  try {
    if (min.length() > 0) {
      min_int = std::stoi(min);
    }
    if (max.length() > 0) {
      max_int = std::stoi(max);
    }
  } catch (std::exception &e) {
    out_config.type = AdcType::RAW_COUNT;
    return false;
  }

  constexpr int kMaximumValue = 1 << 16;

  if (min_int < 0 || max_int < 0 || min_int >= max_int ||
      min_int > kMaximumValue || max_int > kMaximumValue) {
    out_config.type = AdcType::RAW_COUNT;
    return false;
  }

  out_config.min = min_int;
  out_config.max = max_int;

  if (clamp.empty()) {
    out_config.clamp = false;
    return true;
  }

  if (clamp == "true") {
    out_config.clamp = true;
    return true;
  } else if (clamp == "false") {
    out_config.clamp = false;
    return true;
  } else {
    out_config.type = AdcType::RAW_COUNT;
    return false;
  }
}

inline bool parseVoltageDividerAdc(std::string config_string,
                                   AdcConfig &out_config) {
  std::string r1 = getKeyValue(config_string, "r1");
  std::string r2 = getKeyValue(config_string, "r2");
  std::string vref = getKeyValue(config_string, "ref");

  if (r1.empty() || r2.empty()) {
    out_config.type = AdcType::RAW_COUNT;
    return false;
  }

  int r1_int = 0;
  int r2_int = 0;

  try {
    r1_int = std::stoi(r1);
    r2_int = std::stoi(r2);
  } catch (std::exception &e) {
    out_config.type = AdcType::RAW_COUNT;
    return false;
  }

  constexpr int kMaximumResistance = 1000000;
  if (r1_int < 1 || r2_int < 1 || r1_int > kMaximumResistance ||
      r2_int > kMaximumResistance) {
    out_config.type = AdcType::RAW_COUNT;
    return false;
  }

  out_config.resistor_1 = r1_int;
  out_config.resistor_2 = r2_int;

  if (vref.empty()) {
    out_config.voltage_reference = 3300;
    return true;
  }

  int vref_int = 0;
  try {
    vref_int = std::stoi(vref);
  } catch (std::exception &e) {
    out_config.type = AdcType::RAW_COUNT;
    return false;
  }

  constexpr int kMaximumMillivolts = 50000;
  if (vref_int < 1 || vref_int > kMaximumMillivolts) {
    out_config.type = AdcType::RAW_COUNT;
    return false;
  }

  out_config.voltage_reference = vref_int;

  return true;
}

inline bool parseVoltageReferenceAdc(std::string config_string,
                                     AdcConfig &out_config) {
  const std::string exact = "type=v_ref";
  if (config_string == exact) {
    out_config.type = AdcType::VOLTAGE_REFERENCE;
    out_config.voltage_reference = 3300;
    return true;
  }

  std::string vref = getKeyValue(config_string, "ref");
  if (vref.empty()) {
    out_config.type = AdcType::RAW_COUNT;
    return false;
  }

  int vref_int = 0;
  try {
    vref_int = std::stoi(vref);
  } catch (std::exception &e) {
    out_config.type = AdcType::RAW_COUNT;
    return false;
  }

  constexpr int kMaximumMillivolts = 50000;
  if (vref_int < 1 || vref_int > kMaximumMillivolts) {
    out_config.type = AdcType::RAW_COUNT;
    return false;
  }

  out_config.voltage_reference = vref_int;

  return true;
}

bool parseAdcConfig(std::string config_string, AdcConfig &out_config) {
  uint32_t resolution = out_config.resolution;
  out_config = AdcConfig();
  out_config.resolution = resolution;

  if (config_string.empty()) {
    out_config.type = AdcType::RAW_COUNT;
    return true;
  }

  std::string type = getKeyValue(config_string, "type");
  if (type == "raw") {
    out_config.type = AdcType::RAW_COUNT;
    return true;
  } else if (type == "percent") {
    out_config.type = AdcType::PERCENTAGE;
    return parsePercentAdc(config_string, out_config);
  } else if (type == "v_div") {
    out_config.type = AdcType::VOLTAGE_DIVIDER;
    return parseVoltageDividerAdc(config_string, out_config);
  } else if (type == "v_ref") {
    out_config.type = AdcType::VOLTAGE_REFERENCE;
    return parseVoltageReferenceAdc(config_string, out_config);
  } else {
    out_config.type = AdcType::RAW_COUNT;
    return false;
  }
}

} // namespace extension