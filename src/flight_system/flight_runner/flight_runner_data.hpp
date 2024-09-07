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

#include <BoosterSeat/time.hpp>
#include <cstdint>

#include "flight_phase.hpp"

namespace giraffe {

/**
 * @brief Data that is persisted across system restarts. Used by the flight
 * runner to track the state of the system.
 */
class FlightRunnerData {
public:
  /**
   * @brief The reason the system was shutdown. Stored in the data file.
   */
  enum class ShutdownReason {
    /**
     * @brief The system stopped accidentally, not on purpose. Most likely a
     * software crash.
     *
     */
    CRASH_OR_UNKNOWN,

    /**
     * @brief User requested a shutdown by pressing CTRL+C.
     */
    CTRL_C,

    /**
     * @brief User requested a shutdown via the telemetry sdn command.
     */
    TELEMETRY_SDN_COMMAND
  };

  /**
   * @brief The default constructor will attempt to load the data from the saved
   * file.
   */
  FlightRunnerData();

  /**
   * @brief Destructor will attempt to save to the data file.
   */
  ~FlightRunnerData();

  /**
   * @brief Fully resets the flight runner data as if the system has never
   * started before.
   */
  void fullReset();

  void incrementNumStartups();
  uint32_t getNumStartups() const;

  void setShutdownReason(ShutdownReason shutdown_reason);

  bool getSecondsSinceStartup(int64_t &num_seconds);
  bool getSecondsSincePreviousStartup(int64_t &num_seconds);
  bool getSecondsSincePreviousShutdown(int64_t &num_seconds);

  void setFlightPhase(FlightPhase flight_phase);
  FlightPhase getFlightPhase() const;

  /**
   * @brief Clears the launch position/set it to invalid.
   */
  void clearLaunchPosition();

  /**
   * @brief Set the launch position. Called on the transition from pre-launch to
   * launch.
   * @param valid - True if the position is valid.
   * @param latitude - The latitude of the launch position.
   * @param longitude - The longitude of the launch position.
   * @param altitude - The altitude of the launch position.
   */
  void setLaunchPosition(bool valid, double latitude, double longitude,
                         double altitude);

  /**
   * @brief Get the Launch Position object
   *
   * @param[out] valid - True if the position is valid.
   * @param[out] latitude - The latitude of the launch position.
   * @param[out] longitude - The longitude of the launch position.
   * @param[out] altitude - The altitude of the launch position.
   */
  void getLaunchPosition(bool &valid, double &latitude, double &longitude,
                         double &altitude);

private:
  /**
   * @brief Attempts to save the data to a file.
   */
  void saveData(bool shutdown_save = false);

  /**
   * @brief Set to true when the data has been loaded from a file.
   */
  bool load_status_ = false;

  /**
   * @brief Set to true when the data has been saved to a file.
   */
  bool save_status_ = false;

  /**
   * @brief The time the system was started. Immediately set once after startup.
   * Not updated after a full reset.
   * @todo this is a duplicate of the time in shared_date_.flight_data
   */
  bst::Time startup_time_ = {};

  /**
   * @brief The previous time the system was started. Updated at startup.
   */
  bst::Time previous_startup_time_ = {};
  bool previous_startup_time_valid_ = false;

  /**
   * @brief Used to store the time the system was previously shut down. Updated
   * at shutdown.
   */
  bst::Time previous_shutdown_time_ = {};
  bool previous_shutdown_time_valid_ = false;

  ShutdownReason shutdown_reason_ = ShutdownReason::CRASH_OR_UNKNOWN;

  /**
   * @brief The number of times the system has been started.
   */
  uint32_t num_startups_ = 0;

  /**
   * @details This is only set in detectFlightPhase.
   * @see detectFlightPhase
   */
  FlightPhase flight_phase_ = FlightPhase::UNKNOWN;

  /**
   * @brief The launch position. Set to invalid by default.
   */
  bool launch_position_valid_ = false;
  double launch_position_latitude_ = 0.0;
  double launch_position_longitude_ = 0.0;
  double launch_position_altitude_ = 0.0;

  /**
   * @brief The path to the data file. Set in the constructor.
   */
  std::string data_file_path_ = "";
};

} // namespace giraffe

#endif /* FLIGHT_RUNNER_DATA_HPP_ */