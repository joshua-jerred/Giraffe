/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   sim_electrical.hpp
 * @brief  GFS Simulator Electrical Implementation
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-02-13
 * @copyright  2024 (license to be defined)
 */

#ifndef GFS_SIM_ELECTRICAL_HPP_
#define GFS_SIM_ELECTRICAL_HPP_

#include "sim_parameters.hpp"

namespace gfs_sim {

class SimElectrical {
public:
  SimElectrical(SimData &data) : data_(data) {
  }
  ~SimElectrical() = default;

  void update(double delta_time_s) {
    // Update the battery soc
    battery_soc_ -= delta_time_s * K_BATTERY_DISCHARGE_RATE;
    data_.battery_voltage = (K_BATTERY_VOLTAGE_FULL - K_BATTERY_VOLTAGE_EMPTY) *
                                (battery_soc_ / 100.0) +
                            K_BATTERY_VOLTAGE_EMPTY;

    // Update the ADC channels
    data_.adc_ch_0 = adc_ch0_;
    data_.adc_ch_1 = adc_ch1_;
    data_.adc_ch_2 = adc_ch2_;
    data_.adc_ch_3 = adc_ch3_;
    data_.adc_ch_4 = adc_ch4_;
    data_.adc_ch_5 = adc_ch5_;
    data_.adc_ch_6 = adc_ch6_;
    data_.adc_ch_7 = adc_ch7_;
  }

private:
  double battery_soc_{K_BATTERY_INITIAL_SOC};
  double adc_ch0_ = {K_BATTERY_VOLTAGE_FULL};
  double adc_ch1_ = {0.0}; // current sense
  double adc_ch2_ = {K_5V_BUS_VOLTAGE};
  double adc_ch3_ = {K_3V3_BUS_VOLTAGE};
  double adc_ch4_ = {K_4V_BUS_VOLTAGE};
  double adc_ch5_ = {0.0};
  double adc_ch6_ = {0.0};
  double adc_ch7_ = {0.0};

  SimData &data_;
};

} // namespace gfs_sim

#endif /* GFS_SIM_ELECTRICAL_HPP_ */