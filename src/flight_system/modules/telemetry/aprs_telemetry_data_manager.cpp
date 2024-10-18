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

#include <map>

#include "aprs_telemetry_data_manager.hpp"

namespace giraffe {

using namespace signal_easel::aprs::telemetry;

/// @todo This probably needs to be brought into the configuration so it's user
/// accessible.
// enum class AnalogDataSource {
//   NONE = 0,
//   BATTERY_VOLTAGE,
//   INTERNAL_TEMPERATURE,
//   EXTERNAL_TEMPERATURE,
//   PRESSURE,
//   HUMIDITY,
//   VERTICAL_SPEED,
//   HORIZONTAL_SPEED,
//   SATELLITE_COUNT,
//   DISTANCE_TRAVELED,
//   FLIGHT_TIME,
//   RSSI,
//   NUM_ANALOG_DATA_SOURCES
// };

// struct AnalogDataSourceData {
//   AnalogDataSource source_id;
//   std::string label;
//   std::string unit;
//   std::string coefficient_a;
//   std::string coefficient_b;
//   std::string coefficient_c;
// };

// clang-format off
// const std::array<AnalogDataSourceData,
//                 static_cast<size_t>(AnalogDataSource::NUM_ANALOG_DATA_SOURCES)>
//                 ANALOG_PARAMETER_OPTION_MAP{
//   {AnalogDataSource::BATTERY_VOLTAGE, "VBatt", "Volt", "0", "0.024", "4.0"},
//   {AnalogDataSource::BATTERY_VOLTAGE, "VBatt", "Volt", "0", "0.024", "4.0"},
// };
// clang-format on

AprsTelemetryDataManager::AprsTelemetryDataManager(
    data::SharedData &shared_data, cfg::Configuration &config,
    gdl::IDataLink &data_link)
    : shared_data_(shared_data), config_(config), data_link_(data_link) {
  bool success = false;

  // GFS APRS Telemetry is currently set up with hard-coded parameters. If
  // there is value to make this configurable in the future, this can be
  // changed but it does not seem necessary at this time.

  success = telemetry_data_.setProjectTitle(config.general.getProjectName());
  if (!success) {
    logger_.error(DiagnosticId::TELEMETRY_aprsTelemetryInitialization,
                  "Project Title");
  }

  success =
      telemetry_data_.setComment(config.telemetry_data_packets.getComment());
  if (!success) {
    logger_.error(DiagnosticId::TELEMETRY_aprsTelemetryInitialization,
                  "comment");
  }

  // With parameter name, unit, bit sense, and equation messages, the address
  // that fills byte 1-9 of the information field is the address of the
  // station sending the telemetry data. This is due to the fact that a
  // separate station (ground station) could send out these packets for us.
  // Just another odd detail of the APRS spec.
  try {
    telemetry_data_.setTelemetryStationAddress(config.telemetry.getCallSign(),
                                               config.telemetry_aprs.getSsid());
  } catch (const signal_easel::Exception &e) {
    logger_.error(DiagnosticId::TELEMETRY_aprsTelemetryInitialization,
                  "station address");
  }

  // Battery Voltage      - Yes
  // Internal Temperature - No, future option
  // External Temperature - Yes
  // Pressure             - Yes
  // Humidity             - Yes
  // Vertical Speed       - No, future option
  // Horizontal Speed     - No, future option
  // Satellite Count      - No, future option or binary option (3D Fix, > 5)
  // Distance Traveled    - No, future option
  // Flight Time          - No, future option
  // RSSI                 - Yes

  auto param_error = [&](const std::string &parameter_name) {
    logger_.error(DiagnosticId::TELEMETRY_aprsTelemetryInitialization,
                  parameter_name);
  };

  // ------ Analog Parameters ------ //
  // Equation: y = ax^2 + bx + c
  // Parameter Resolution: 8 bits (APRS spec)

  // A1 - Battery Voltage | Label: "VBatt" | Unit: "Volt"
  // [range: 4.0 to 10.12, resolution: 0.024]
  // Coefficients a = 0, b = 0.024, c = 4
  auto &a1 = telemetry_data_.getAnalog(Parameter::Id::A1);
  success = a1.setName("VBatt");
  success = a1.setUnitOrLabel("Volt");
  success = a1.setCoefficientA("0");
  success = a1.setCoefficientB("0.024");
  success = a1.setCoefficientC("4");
  if (!success) {
    param_error("a1");
  }

  // A2 - External Temperature | Label: "exTmp" | Unit: "degC"
  // [range: -70 to 58, resolution: 0.5]
  // Coefficients a = 0, b = 0.5, c = -70
  auto &a2 = telemetry_data_.getAnalog(Parameter::Id::A2);
  success = a2.setName("exTemp");
  success = a2.setUnitOrLabel("degC");
  success = a2.setCoefficientA("0");
  success = a2.setCoefficientB("0.5");
  success = a2.setCoefficientC("-70");
  if (!success) {
    param_error("a2");
  }

  // A3 - Pressure | Label: "pres" | Unit: "mbar"
  // [range: 0 to 1000, resolution: variable]
  // Coefficients a = 0.016, b = 0, c = 1
  auto &a3 = telemetry_data_.getAnalog(Parameter::Id::A3);
  success = a3.setName("pres");
  success = a3.setUnitOrLabel("mbar");
  success = a3.setCoefficientA("0.016");
  success = a3.setCoefficientB("0");
  success = a3.setCoefficientC("1");
  if (!success) {
    param_error("a3");
  }

  // A4 - Humidity | Label: "relHu" | Unit: "%"
  // [range: 0 to 100, resolution: 0.39]
  // Coefficients a = 0, b = -0.39, c = 100
  auto &a4 = telemetry_data_.getAnalog(Parameter::Id::A4);
  success = a4.setName("relHu");
  success = a4.setUnitOrLabel("%");
  success = a4.setCoefficientA("0");
  success = a4.setCoefficientB("0.39");
  success = a4.setCoefficientC("100");
  if (!success) {
    param_error("a4");
  }

  // A5 - RSSI | Label: "rssi" | Unit: "db"
  // [range: -100 to 0, resolution: 0.39]
  // Coefficients a = 0, b = -0.39, c = 0
  auto &a5 = telemetry_data_.getAnalog(Parameter::Id::A5);
  success = a5.setName("rssi");
  success = a5.setUnitOrLabel("dBm");
  success = a5.setCoefficientA("0");
  success = a5.setCoefficientB("-0.39");
  success = a5.setCoefficientC("0");
  if (!success) {
    param_error("a5");
  }

  // -- Digital Parameters --
  // b1 - GPS Lock (3d Fix)
  // b2 - Data Link Status (Receiving within the last X minutes)
  // b3 - Launch Detected
  // b4 - Descent Detected
  // b5 - Warning Detected
  // b6 - Error Detected
  // b7 - unused
  // b8 - unused

  // B1 - GPS Lock | Label: "3dFix"
  auto &b1 = telemetry_data_.getDigital(Parameter::Id::B1);
  success = b1.setName("3dFix");
  success = b1.setUnitOrLabel("lock");
  if (!success) {
    param_error("b1");
  }

  // B2 - Data Link Status | Label: "ulink"
  auto &b2 = telemetry_data_.getDigital(Parameter::Id::B2);
  success = b2.setName("ulnk");
  success = b2.setUnitOrLabel("act");
  if (!success) {
    param_error("b2");
  }

  // B3 - Launch Detected | Label: "lnch"
  auto &b3 = telemetry_data_.getDigital(Parameter::Id::B3);
  success = b3.setName("lch");
  success = b3.setUnitOrLabel("det");
  if (!success) {
    param_error("b3");
  }

  // B4 - Descent Detected | Label: "dcnt"
  auto &b4 = telemetry_data_.getDigital(Parameter::Id::B4);
  success = b4.setName("dst");
  success = b4.setUnitOrLabel("det");
  if (!success) {
    param_error("b4");
  }

  // B5 - Warning Detected | Label: "warn"
  auto &b5 = telemetry_data_.getDigital(Parameter::Id::B5);
  success = b5.setName("wrn");
  success = b5.setUnitOrLabel("atv");
  if (!success) {
    param_error("b5");
  }

  // B6 - Error Detected | Label: "err"
  auto &b6 = telemetry_data_.getDigital(Parameter::Id::B6);
  success = b6.setName("er");
  success = b6.setUnitOrLabel("");
  if (!success) {
    param_error("b6");
  }

  // B7 - unused
  auto &b7 = telemetry_data_.getDigital(Parameter::Id::B7);
  success = b7.setName("");
  success = b7.setUnitOrLabel("");
  if (!success) {
    param_error("b7");
  }

  // B8 - unused
  auto &b8 = telemetry_data_.getDigital(Parameter::Id::B8);
  success = b8.setName("");
  success = b8.setUnitOrLabel("");
  if (!success) {
    param_error("b8");
  }
}

void AprsTelemetryDataManager::update() {
  /// @todo Populate the data fields of telemetry_data_ at a set interval
  /// and send it over to the Data Link.
}

} // namespace giraffe