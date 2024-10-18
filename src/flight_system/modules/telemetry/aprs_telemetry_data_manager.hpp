/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   aprs_telemetry_data_setup.hpp
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-10-11
 * @copyright  2024 (license to be defined)
 */

#pragma once

#include "i_giraffe_data_link.hpp"

#include "configuration.hpp"
#include "logger.hpp"
#include "shared_data.hpp"

namespace giraffe {

/// @brief Manages APRS Telemetry data for the Flight System.
/// @details This class is responsible structuring the telemetry data
/// packets and updating the data fields at a regular rate. The data is then
/// sent over to the DataLink's APRS Telemetry Sender.
class AprsTelemetryDataManager {
public:
  AprsTelemetryDataManager(data::SharedData &shared_data,
                           cfg::Configuration &config,
                           gdl::IDataLink &data_link);

  ~AprsTelemetryDataManager() = default;

  void update();

private:
  gdl::IDataLink::TelemetryData telemetry_data_{};

  data::SharedData &shared_data_;

  cfg::Configuration &config_;

  gdl::IDataLink &data_link_;

  Logger logger_{shared_data_, node::Identification::TELEMETRY_MODULE,
                 "AprsTDM"};
};

} // namespace giraffe