/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   adc_extension_base.hpp
 * @brief  Generic ADC Extension Wrapper
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-05
 * @copyright  2023 (license to be defined)
 */

#ifndef ADC_EXTENSION_BASE_HPP_
#define ADC_EXTENSION_BASE_HPP_

#include <BoosterSeat/timer.hpp>

#include "adc_configuration.hpp"
#include "extension_base.hpp"
#include "giraffe_assert.hpp"

namespace extension {

/**
 * @brief The ADC timeout multiplier based on the update interval.
 */
inline constexpr uint32_t kWatchdogMultiplier = 4;

// template <uint32_t AdcResolution>
class AdcExtensionBase : public Extension {
  // static_assert(AdcResolution >= 4 && AdcResolution <= 16);

public:
  AdcExtensionBase(ExtensionResources &resources,
                   cfg::ExtensionMetadata metadata)
      : Extension(resources, metadata),
        adc_watchdog_(metadata.update_interval * kWatchdogMultiplier) {
    // adc_config_.resolution = adc_resolution_;
    // if (!parseAdcConfig(metadata.extra_args, adc_config_)) {
    // error(DiagnosticId::EXTENSION_adcConfigFail);
    // }
  }

protected:
  virtual void adcStartup(){};
  virtual bool readAdc() {
    giraffe_assert(false);
    return false;
  };
  virtual void adcShutdown(){};

  void reportAdcData(uint32_t value, uint8_t channel) {
    dataWithSecondaryIdentifier(data::DataId::ADC_rawCount, value,
                                std::to_string(channel));
  }

private:
  void startup() override {
    adcStartup();
    if (isFaulted()) {
      return;
    }
    adc_watchdog_.reset();
  }

  void loop() override {
    if (adc_watchdog_.isDone()) {
      raiseFault(DiagnosticId::EXT_FAULT_adcWatchdogTimeout);
      return;
    }
    if (!readAdc()) {
      return;
    }

    // switch (adc_config_.type) {
    // case AdcType::RAW_COUNT:
    //   data(data::DataId::ADC_rawCount, adc_value);
    //   break;
    // case AdcType::PERCENTAGE:
    //   percentage(adc_value);
    //   break;
    // case AdcType::VOLTAGE_DIVIDER:
    //   voltageDivider(adc_value);
    //   break;
    // case AdcType::VOLTAGE_REFERENCE:
    //   voltageReference(adc_value);
    //   break;
    // default:
    //   giraffe_assert(false);
    //   break;
    // }

    adc_watchdog_.reset();
  }

  void shutdown() override {
  }

  // void percentage(uint32_t value) {
  //   int max = adc_config_.max;
  //   int min = adc_config_.min;
  //   int range = max - min;
  //   int scaled = value - min;
  //   int percentage = (scaled * 100) / range;
  //   data(data::DataId::ADC_percentage, percentage);
  // }

  // void voltageDivider(uint32_t value) {
  //   uint32_t unscaled =
  //       (value * adc_config_.voltage_reference) / (1 << adc_resolution_);
  //   uint32_t millivolts =
  //       (unscaled * (adc_config_.resistor_1 + adc_config_.resistor_2)) /
  //       adc_config_.resistor_2;
  //   data(data::DataId::ADC_voltage, millivolts);
  // }

  // void voltageReference(uint32_t value) {
  //   uint32_t millivolts =
  //       (value * adc_config_.voltage_reference) / (1 << adc_resolution_);
  //   data(data::DataId::ADC_voltage, millivolts);
  // }

  // const uint32_t adc_resolution_ = AdcResolution;

  // AdcConfig adc_config_{};
  bst::Timer adc_watchdog_;
};

} // namespace extension

#endif /* EXTENSION_HPP_ */
