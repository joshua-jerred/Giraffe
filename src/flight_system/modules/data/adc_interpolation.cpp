/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   adc_interpolation.cpp
 * @brief  ADC Interpolation Utility
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-02-20
 * @copyright  2024 (license to be defined)
 */

#include <algorithm>

#include "adc_interpolation.hpp"
#include "giraffe_assert.hpp"

namespace data_middleware {

AdcInterpolation::AdcInterpolation(data::SharedData &shared_data,
                                   cfg::Configuration &configuration)
    : shared_data_(shared_data), configuration_(configuration),
      logger_(shared_data.streams.log, node::Identification::DATA_MODULE,
              "ADC Interp") {

  // Load the ADC configuration into the map
  auto adc_cfg = configuration_.adc_mappings.getAdcConfigs();
  for (auto &cfg : adc_cfg) {
    std::string key = cfg.extension_name + ":" +
                      std::to_string(static_cast<int>(cfg.channel_number));
    logger_.info("Loaded ADC Mapping For: " + cfg.extension_name +
                 " Channel: " + std::to_string(cfg.channel_number));
    adc_map_.insert_or_assign(key, cfg);
  }
}

void AdcInterpolation::processDataPacket(const data::DataPacket &data_packet) {
  if (data_packet.identifier != data::DataId::ADC_count) {
    return;
  }

  const auto &adc_cfg = getAdcConfig(data_packet.secondary_identifier);
  switch (adc_cfg.type) {
  case cfg::gEnum::AdcType::RAW_COUNT:
    shared_data_.frames.adc.insert(adc_cfg.label,
                                   {data_packet.numeric_value, 0});
    break;
  case cfg::gEnum::AdcType::PERCENTAGE:
    insertPercentageValue(adc_cfg, data_packet.numeric_value);
    break;
  case cfg::gEnum::AdcType::VOLTAGE_DIVIDER:
    insertVoltageDividerValue(adc_cfg, data_packet.numeric_value);
    break;
  default:
    giraffe_assert(false);
    break;
  }
}

void AdcInterpolation::insertVoltageDividerValue(const cfg::AdcConfig &adc_cfg,
                                                 uint32_t value) {
  uint32_t unscaled =
      (value * adc_cfg.voltage_reference) / (1 << adc_cfg.resolution);
  uint32_t millivolts =
      (unscaled * (adc_cfg.resistor_1_value + adc_cfg.resistor_2_value)) /
      adc_cfg.resistor_2_value;

  if (adc_cfg.label == "bat_mv") {
    shared_data_.streams.data.addNumericData(node::Identification::DATA_MODULE,
                                             data::DataId::BATTERY_voltageMv,
                                             millivolts);
  }

  shared_data_.frames.adc.insert(adc_cfg.label, {value, millivolts});
}

void AdcInterpolation::insertPercentageValue(const cfg::AdcConfig &adc_cfg,
                                             uint32_t value) {
  const double max = static_cast<double>(adc_cfg.percentage_max);
  const double min = static_cast<double>(adc_cfg.percentage_min);
  const double range = max - min;
  double percentage = ((static_cast<double>(value) - min) / range) * 100.0;

  if (adc_cfg.percentage_clamp) {
    percentage = std::clamp(percentage, 0.0, 100.0);
  }

  shared_data_.frames.adc.insert(adc_cfg.label, {value, percentage});
}

const cfg::AdcConfig &AdcInterpolation::getAdcConfig(const std::string &key) {
  if (!adc_map_.contains(key)) {
    logger_.error(DiagnosticId::DATA_MODULE_adcInterpolationUnknownSource, key);

    // generate a basic configuration and add it to the map
    cfg::AdcConfig cfg;
    cfg.label = key; // labels must be unique, so this is a decent fallback
    cfg.type = cfg::gEnum::AdcType::RAW_COUNT;
    adc_map_.insert_or_assign(key, cfg);
  }

  return adc_map_.at(key);
}

} // namespace data_middleware