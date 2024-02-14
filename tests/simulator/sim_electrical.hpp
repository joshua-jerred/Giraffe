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
  SimElectrical() = default;
  ~SimElectrical() = default;

  void update(double delta_time_s) {
    (void)delta_time_s;
  }

private:
};

} // namespace gfs_sim

#endif /* GFS_SIM_ELECTRICAL_HPP_ */