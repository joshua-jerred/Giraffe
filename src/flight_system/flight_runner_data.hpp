/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   flight_runner_data.hpp
 * @brief  Non-volatile data for the flight runner.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-03
 * @copyright  2024 (license to be defined)
 */

#ifndef FLIGHT_RUNNER_DATA_HPP_
#define FLIGHT_RUNNER_DATA_HPP_

#include <cstdint>

namespace giraffe {

class FlightRunnerData {
public:
  /**
   * @brief Attempts to load the data from a file.
   */
  FlightRunnerData();
  ~FlightRunnerData();

  /**
   * @brief Fully resets the flight runner data as if the system has never
   * started before.
   */
  void fullReset();

  void incrementNumStartups();
  uint32_t getNumStartups() const;

private:
  /**
   * @brief Attempts to load the data from a file.
   */
  void loadData();

  /**
   * @brief Attempts to save the data to a file.
   */
  void saveData();

  /**
   * @brief Set to true when the data has been loaded from a file.
   */
  bool load_status_ = false;

  /**
   * @brief Set to true when the data has been saved to a file.
   */
  bool save_status_ = false;

  /**
   * @brief The number of times the system has been started.
   */
  uint32_t num_startups_ = 0;
};

} // namespace giraffe

#endif /* FLIGHT_RUNNER_DATA_HPP_ */