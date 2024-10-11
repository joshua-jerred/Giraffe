/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   i_giraffe_data_link.hpp
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-10-11
 * @copyright  2024 (license to be defined)
 */

#pragma once

#include <cstdint>

#include <SignalEasel/aprs.hpp>

namespace giraffe::gdl {

/// @brief Interface for the Giraffe Data Link.
class IDataLink {
public:
  using TelemetryData = signal_easel::aprs::telemetry::TelemetryData;

  // So far this interface is just a here so the APRS telemetry sender can
  // access these methods of the Data Link.
  virtual bool sendTelemetryData(TelemetryData &telemetry_data,
                                 uint32_t message_id) = 0;
  virtual bool sendTelemetryCoefficients(TelemetryData &telemetry_data) = 0;
  virtual bool sendTelemetryParameterNames(TelemetryData &telemetry_data) = 0;
  virtual bool sendTelemetryUnitsAndLabels(TelemetryData &telemetry_data) = 0;
  virtual bool sendTelemetryBitSense(TelemetryData &telemetry_data) = 0;
};

} // namespace giraffe::gdl