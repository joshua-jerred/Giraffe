/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   adc_data.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2025-03-02
/// @copyright  2025 (license to be defined)

#pragma once

#include "hardware/adc.h"

#include "periodic_processor.hpp"

namespace power_board {

#define ADC_NUM ADC_TEMPERATURE_CHANNEL_NUM
#define ADC_PIN (26 + ADC_NUM)
#define ADC_VREF 3.3f
#define ADC_RANGE (1 << 12)
#define ADC_CONVERT (ADC_VREF / (ADC_RANGE - 1))

class AdcData : public PeriodicProcessor {
public:
  AdcData(uint64_t module_process_interval_ms)
      : PeriodicProcessor(module_process_interval_ms) {
    bi_decl(bi_1pin_with_name(ADC_PIN, "ADC input pin"));
    adc_init();
    adc_set_temp_sensor_enabled(true);
    // adc_gpio_init(ADC_PIN);
    // adc_select_input(ADC_NUM);
  }

  float read() {
    uint16_t result = adc_read();
    return result * ADC_CONVERT;
  }

private:
  void processImpl() {
    float temp_c = readTemperatureC();
    // printf("Temperature: %f\n", temp_c);
  }

  float readTemperatureC() {
    adc_select_input(4);

    // From Raspberry Pi Pico-series C/C++ SDK documentation
    uint16_t adc_count = adc_read();
    const float conversion_factor = 3.3f / (1 << 12);
    float voltage = adc_count * conversion_factor;
    float temp_c = 27 - ((voltage - 0.706f) / 0.001721f);

    return temp_c;
  }
};

} // namespace power_board