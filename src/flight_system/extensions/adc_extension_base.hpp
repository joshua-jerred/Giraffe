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

namespace extension {

/**
 * @brief The ADC timeout multiplier based on the update interval.
 */
inline constexpr uint32_t kWatchdogMultiplier = 4;

template <uint32_t AdcResolution> class AdcExtensionBase : public Extension {
public:
  AdcExtensionBase(ExtensionResources &resources,
                   cfg::ExtensionMetadata metadata)
      : Extension(resources, metadata),
        adc_watchdog_(metadata.update_interval * kWatchdogMultiplier) {
    adc_config_.resolution = adc_resolution_;
    if (!parseAdcConfig(metadata.extra_args, adc_config_)) {
      error(DiagnosticId::EXTENSION_adcConfigFail);
    }
  }

protected:
  virtual void adcStartup(){};
  virtual bool readAdc(uint32_t &value) = 0;
  virtual void adcShutdown(){};

private:
  void startup() override {
    adcStartup();
    if (fault_flag_) {
      return;
    }
    adc_watchdog_.reset();
  }

  void loop() override {
    if (adc_watchdog_.isDone()) {
      raiseFault(DiagnosticId::EXT_FAULT_adcWatchdogTimeout);
      return;
    }
    uint32_t adc_value;
    if (!readAdc(adc_value)) {
      return;
    }

    switch (adc_config_.type) {
    case AdcType::RAW_COUNT:
      rawCount(adc_value);
      break;
    case AdcType::PERCENTAGE:
      percentage(adc_value);
      break;
    case AdcType::VOLTAGE_DIVIDER:
      voltageDivider(adc_value);
      break;
    default:
      giraffe_assert(false);
      break;
    }

    adc_watchdog_.reset();
  }

  void shutdown() override {
  }

  void rawCount(uint32_t value) {
  }

  void percentage(uint32_t value) {
  }

  void voltageDivider(uint32_t value) {
  }

  const uint32_t adc_resolution_ = AdcResolution;

  AdcConfig adc_config_{};
  BoosterSeat::Timer adc_watchdog_;
};

} // namespace extension

#endif /* EXTENSION_HPP_ */
