/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   aprs_manager.hpp
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-10-11
 * @copyright  2024 (license to be defined)
 */

#pragma once

#include <BoosterSeat/timer.hpp>

#include "i_giraffe_data_link.hpp"

#include "configuration.hpp"
#include "logger.hpp"
#include "shared_data.hpp"

namespace giraffe {

/// @brief Manages GDL APRS Position and Telemetry Data.
/// @details This class is responsible structuring the telemetry data
/// packets and updating the GDL's telemetry sender with the data.
/// This class is now also responsible for sending APRS Position packets.
/// @todo Consider renaming to "AprsDataManager"
class AprsManager {
public:
  /// @brief Constructor
  /// @param shared_data - Shared data object - needed for getting data and
  /// logging.
  /// @param config - GFS Configuration.
  /// @param data_link - The Data Link.
  AprsManager(data::SharedData &shared_data, cfg::Configuration &config,
              gdl::IDataLink &data_link, bst::ITimer &data_update_timer_);

  /// @brief Force a configuration reload.
  void loadConfiguration();

  /// @brief This method handles everything that needs to be done. It must be
  /// called at least at the rate of the configured send rate.
  void update();

  /// @brief Sends the on-air definition packets.
  /// @details This will add the PARM, UNIT, EQNS, and BITS packets to the data
  /// link broadcast queue. The purpose of this method is to allow an external
  /// process (Data Link Commands) to request these instead of sending them
  /// automatically.
  /// @return \c true if the packets were successfully added to the broadcast
  /// queue, \c false otherwise.
  bool sendOnAirDefinitionPackets();

  static constexpr int DATA_UPDATE_RATE_MS = 1000;

private:
  /// @brief Initializes the APRS Position Packet data from the configuration.
  void initializePositionPacketData();

  /// @brief Initializes the APRS Telemetry data from the configuration.
  void initializeTelemetryData();

  void updatePositionPacketData();

  void updateTelemetryData();

  /// @brief Set to true after a successful call to
  /// initializePositionPacketData.
  bool position_packet_initialized_ = false;

  /// @brief Set to true after a successful call to initializeTelemetryData.
  bool telemetry_data_initialized_ = false;

  gdl::IDataLink::TelemetryData telemetry_data_{};

  data::SharedData &shared_data_;

  cfg::Configuration &config_;

  gdl::IDataLink &data_link_;

  /// @brief Timer used for updating the local state of the telemetry data.
  /// @details This is not used for sending the data. That is handled by GDLs
  /// telemetry sender.
  bst::ITimer &data_update_timer_;

  Logger logger_{shared_data_, node::Identification::TELEMETRY_MODULE,
                 "AprsTDM"};
};

} // namespace giraffe