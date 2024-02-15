/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   sim_parameters.hpp
 * @brief  Parameters for the GFS Simulator
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-02-13
 * @copyright  2024 (license to be defined)
 */

#ifndef GFS_SIM_PARAMETERS_HPP_
#define GFS_SIM_PARAMETERS_HPP_

#include <atomic>

namespace gfs_sim {

enum class SimState { PRE_LAUNCH, ASCENT, POP, DESCENT, LANDING, LANDED };

struct SimData {
  std::atomic<double> temperature_c{0.0};
  std::atomic<double> pressure_mbar{0.0};
  std::atomic<double> humidity_percent{0.0};

  std::atomic<double> altitude_m{0.0};
  std::atomic<double> vert_velocity_mps{0.0};
  std::atomic<double> vert_accel_mps2{0.0};
  std::atomic<double> horz_velocity_mps{0.0};
  std::atomic<double> heading_deg{0.0};
  std::atomic<double> latitude_deg{0.0};
  std::atomic<double> longitude_deg{0.0};

  std::atomic<double> adc_ch_0{0.0};
  std::atomic<double> adc_ch_1{0.0};
  std::atomic<double> adc_ch_2{0.0};
  std::atomic<double> adc_ch_3{0.0};
  std::atomic<double> adc_ch_4{0.0};
  std::atomic<double> adc_ch_5{0.0};
  std::atomic<double> adc_ch_6{0.0};
  std::atomic<double> adc_ch_7{0.0};
};

inline constexpr uint32_t K_UPDATE_INTERVAL_MS = 100;
inline constexpr uint32_t K_SLEEP_INTERVAL_MS = K_UPDATE_INTERVAL_MS / 4;
inline constexpr double K_DELTA_TIME_S = K_UPDATE_INTERVAL_MS / 1000.0;
inline constexpr uint32_t K_PRINT_INTERVAL_MS = 1000;
inline constexpr bool K_PRINT_SIM_DATA = true;

// -- BalloonPhysics Parameters --
// Start Parameters
inline constexpr double K_START_ALTITUDE_METERS = 0.0;
inline constexpr double K_START_VERTICAL_SPEED_MPS = 0.0;
inline constexpr double K_START_VERTICAL_ACCELERATION_MPS2 = 0.0;
inline constexpr double K_START_HORIZONTAL_SPEED_MPS = 0.0;
inline constexpr double K_START_LATITUDE = 39.725796;
inline constexpr double K_START_LONGITUDE = -83.561191;
// Pre-launch Parameters
inline constexpr double K_PRELAUNCH_LAUNCH_DELAY_S =
    1.0; // set to 0 if the start parameters are non-zero
// Ascent Parameters
inline constexpr double K_ASCENT_ACCELERATION_MPS2 = 0.3;
inline constexpr double K_ASCENT_ACCELERATION_VARIANCE = 0.1;
inline constexpr double K_ASCENT_RATE_METERS_PER_SECOND_MAX = 8.0;
inline constexpr double K_ASCENT_RATE_METERS_PER_SECOND_MIN = 2.5;
inline constexpr double K_POP_ALTITUDE_METERS = 5000; // 30000.0;
// Pop Parameters
inline constexpr double K_POP_DESCENT_RATE_METERS_PER_SECOND_TARGET = -30.0;
inline constexpr double K_POP_ACCELERATION_MPS2 = -7.0;
inline constexpr double K_POP_ACCELERATION_VARIANCE = 0.5;
// Descent Parameters
inline constexpr double K_DESCENT_RATE_METERS_PER_SECOND_MAX = 30.0;
inline constexpr double K_DESCENT_RATE_METERS_PER_SECOND_MIN = 5.5;
inline constexpr double K_DESCENT_DECELERATION_MPS2 = 2.5;
inline constexpr double K_DESCENT_DECELERATION_VARIANCE = 0.2;
inline constexpr double K_DESCENT_TRANSITION_ALTITUDE_METERS = 10;
// Landing Parameters
inline constexpr double K_LANDING_ALTITUDE_METERS = 0.0;

// -- Electrical Parameters --
inline constexpr double K_BATTERY_VOLTAGE_FULL = 4.2;
inline constexpr double K_BATTERY_VOLTAGE_EMPTY = 2.5;
inline constexpr double K_BATTERY_INITIAL_SOC = 100.0;   // Percent
inline constexpr double K_BATTERY_DISCHARGE_RATE = 0.02; // Percent per second
inline constexpr double K_5V_BUS_VOLTAGE = 5.0;
inline constexpr double K_3V3_BUS_VOLTAGE = 3.3;
inline constexpr double K_4V_BUS_VOLTAGE = 4.0;

} // namespace gfs_sim

#endif /* GFS_SIM_PARAMETERS_HPP_ */