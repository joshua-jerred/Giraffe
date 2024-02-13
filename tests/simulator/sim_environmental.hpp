/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   sim_environmental.hpp
 * @brief  SimEnvironmental Simulation for the GFS Simulator
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-02-13
 * @copyright  2024 (license to be defined)
 */

#ifndef GFS_SIM_ENVIRONMENTAL_HPP_
#define GFS_SIM_ENVIRONMENTAL_HPP_

namespace gfs_sim {

struct EnvData {
  double altitude_meters{};
  double temperature_celsius{};
  double pressure_millibars{};
  double wind_direction_degrees{};
  double wind_speed_mps{};
  double relative_humidity_percent{};
};

class SimEnvironmental {
public:
  SimEnvironmental() = default;

  void setAltitudeMeters(double altitude_meters);

  double getTemperatureCelsius() const;
  double getWindSpeedMPS() const;
  double getWindDirectionDegrees() const;
  double getPressureMillibars() const;
  double getRelativeHumidityPercent() const;

private:
  double altitude_meters_ = 0.0;
  EnvData env_data_{};
};

} // namespace gfs_sim

#endif /* GFS_SIM_ENVIRONMENTAL_HPP_ */