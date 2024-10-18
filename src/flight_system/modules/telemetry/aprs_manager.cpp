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

#include "aprs_manager.hpp"

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

AprsManager::AprsManager(data::SharedData &shared_data,
                         cfg::Configuration &config, gdl::IDataLink &data_link,
                         bst::ITimer &data_update_timer)
    : shared_data_(shared_data), config_(config), data_link_(data_link),
      data_update_timer_(data_update_timer) {
  data_update_timer_.setTimeout(DATA_UPDATE_RATE_MS);
  loadConfiguration();
}

void AprsManager::loadConfiguration() {
  initializePositionPacketData();
  initializeTelemetryData();
}

void AprsManager::initializePositionPacketData() {
  bool success = false;
  bool failure = false;

  if (failure) {
    position_packet_initialized_ = false;
  } else {
    position_packet_initialized_ = true;
  }
}

void AprsManager::initializeTelemetryData() {
  bool success = false;
  bool failure = false;
  // GFS APRS Telemetry is currently set up with hard-coded parameters. If
  // there is value to make this configurable in the future, this can be
  // changed but it does not seem necessary at this time.

  success = telemetry_data_.setProjectTitle(config_.general.getProjectName());
  if (!success) {
    logger_.error(DiagnosticId::TELEMETRY_aprsTelemetryInitialization,
                  "Project Title");
    failure = true;
  }

  success = telemetry_data_.setComment(config_.telemetry_aprs.getComment());
  if (!success) {
    logger_.error(DiagnosticId::TELEMETRY_aprsTelemetryInitialization,
                  "comment");
    failure = true;
  }

  // With parameter name, unit, bit sense, and equation messages, the address
  // that fills byte 1-9 of the information field is the address of the
  // station sending the telemetry data. This is due to the fact that a
  // separate station (ground station) could send out these packets for us.
  // Just another odd detail of the APRS spec.
  try {
    telemetry_data_.setTelemetryStationAddress(
        config_.telemetry.getCallSign(), config_.telemetry_aprs.getSsid());
  } catch (const signal_easel::Exception &e) {
    logger_.error(DiagnosticId::TELEMETRY_aprsTelemetryInitialization,
                  "station address");
    failure = true;
  }

  // Telemetry Parameter Initialization
  {
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
      failure = true;
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

    // B2 - Data Link Status (RSSI Valid) | Label: "ulink"
    auto &b2 = telemetry_data_.getDigital(Parameter::Id::B2);
    success = b2.setName("ulnk");
    success = b2.setUnitOrLabel("act");
    if (!success) {
      param_error("b2");
    }

    // Use the next 3 parameters to encode the current flight phase
    // 0 - Unknown     -- 0b000
    // 1 - Pre-Launch  -- 0b001
    // 2 - Launch      -- 0b010
    // 3 - Ascent      -- 0b011
    // 4 - Descent     -- 0b100
    // 5 - Recovery    -- 0b101
    // 6 - unused      -- 0b110
    // 7 - unused      -- 0b111

    // B3 - Flight Phase msb
    auto &b3 = telemetry_data_.getDigital(Parameter::Id::B3);
    success = b3.setName("ph2");
    success = b3.setUnitOrLabel("msb");
    if (!success) {
      param_error("b3");
    }

    // B4 - Flight Phase
    auto &b4 = telemetry_data_.getDigital(Parameter::Id::B4);
    success = b4.setName("ph1");
    success = b4.setUnitOrLabel("");
    if (!success) {
      param_error("b4");
    }

    // B5 - Flight Phase lsb
    auto &b5 = telemetry_data_.getDigital(Parameter::Id::B5);
    success = b5.setName("ph0");
    success = b5.setUnitOrLabel("lsb");
    if (!success) {
      param_error("b5");
    }

    // B6 - Phase prediction valid
    auto &b6 = telemetry_data_.getDigital(Parameter::Id::B6);
    success = b6.setName("phv");
    success = b6.setUnitOrLabel("v");
    if (!success) {
      param_error("b6");
    }

    // B7 - External Temperature Valid
    auto &b7 = telemetry_data_.getDigital(Parameter::Id::B7);
    success = b7.setName("tv");
    success = b7.setUnitOrLabel("v");
    if (!success) {
      param_error("b7");
    }

    // B8 - Barometric Pressure Valid
    auto &b8 = telemetry_data_.getDigital(Parameter::Id::B8);
    success = b8.setName("bv");
    success = b8.setUnitOrLabel("v");
    if (!success) {
      param_error("b8");
    }
  }

  if (failure) {
    telemetry_data_initialized_ = false;
    data_link_.getConfig().setAprsTelemetrySenderEnabled(false);
  } else {
    telemetry_data_initialized_ = true;

    data_link_.getConfig().setAprsTelemetrySenderEnabled(
        config_.telemetry_aprs.getTelemetryPackets());
    data_link_.getConfig().setAprsTelemetryDataIntervalMs(
        config_.telemetry_aprs.getTelemetryPacketInterval() * 1000);
  }
}

bool AprsManager::sendOnAirDefinitionPackets() {
  if (!telemetry_data_initialized_) {
    return false;
  }

  if (!data_link_.sendTelemetryParameterNames(telemetry_data_)) {
    return false;
  }
  if (!data_link_.sendTelemetryUnitsAndLabels(telemetry_data_)) {
    return false;
  }
  if (!data_link_.sendTelemetryCoefficients(telemetry_data_)) {
    return false;
  }
  if (!data_link_.sendTelemetryBitSense(telemetry_data_)) {
    return false;
  }

  return true;
}

void AprsManager::update() {
  // No need to update the data.
  if (!data_update_timer_.isDone()) {
    return;
  }
  data_update_timer_.reset();

  if (config_.telemetry_aprs.getPositionPackets()) {
    updatePositionPacketData();
  }

  if (config_.telemetry_aprs.getTelemetryPackets()) {
    updateTelemetryData();
  }
}

void AprsManager::updatePositionPacketData() {
  /// @todo Implement
}
void AprsManager::updateTelemetryData() {
  /// @todo Implement

  auto convert_to_raw_no_exponent =
      [](double value, signal_easel::aprs::telemetry::AnalogParameter &par) {
        double coefficient_b = par.getCoefficientBd();
        double coefficient_c = par.getCoefficientCd();

        value = (value - coefficient_c) / coefficient_b;

        return static_cast<uint8_t>(std::clamp(value, 0.0, 255.0));
      };

  const auto calculated_data = shared_data_.blocks.calculated_data.get();

  // Analog Parameters
  {
    auto &a1 = telemetry_data_.getAnalog(Parameter::Id::A1);
    uint8_t converted_value = convert_to_raw_no_exponent(
        calculated_data.battery_voltage_mv / 1000.0, a1);
    a1.setRawValue(converted_value);
  }
  {
    auto &a2 = telemetry_data_.getAnalog(Parameter::Id::A2);
    uint8_t converted_value =
        convert_to_raw_no_exponent(calculated_data.external_temperature_c, a2);
    a2.setRawValue(converted_value);
  }
  {
    auto &a3 = telemetry_data_.getAnalog(Parameter::Id::A3);
    uint8_t converted_value = convert_to_raw_no_exponent(
        calculated_data.barometric_pressure_mbar, a3);
    a3.setRawValue(converted_value);
  }
  {
    auto &a4 = telemetry_data_.getAnalog(Parameter::Id::A4);
    uint8_t converted_value =
        convert_to_raw_no_exponent(calculated_data.relative_humidity, a4);
    a4.setRawValue(converted_value);
  }
  {
    auto &a5 = telemetry_data_.getAnalog(Parameter::Id::A5);
    uint8_t converted_value =
        convert_to_raw_no_exponent(calculated_data.uplink_rssi, a5);
    a5.setRawValue(converted_value);
  }

  // Digital Parameters
  {
    auto &b1 = telemetry_data_.getDigital(Parameter::Id::B1);
    b1.setValue(calculated_data.gps_fix_valid);
  }
  {
    auto &b2 = telemetry_data_.getDigital(Parameter::Id::B2);
    b2.setValue(calculated_data.uplink_rssi_valid);
  }
  {
    const auto &phase = shared_data_.flight_data.getFlightPhase();

    bool b3_msb = false;
    bool b4_mid = false;
    bool b5_lsb = false;

    switch (phase) {
    case FlightPhase::UNKNOWN:
      b3_msb = false;
      b4_mid = false;
      b5_lsb = false;
      break;
    case FlightPhase::PRE_LAUNCH:
      b3_msb = false;
      b4_mid = false;
      b5_lsb = true;
      break;
    case FlightPhase::LAUNCH:
      b3_msb = false;
      b4_mid = true;
      b5_lsb = false;
      break;
    case FlightPhase::ASCENT:
      b3_msb = false;
      b4_mid = true;
      b5_lsb = true;
      break;
    case FlightPhase::DESCENT:
      b3_msb = true;
      b4_mid = false;
      b5_lsb = false;
      break;
    case FlightPhase::RECOVERY:
      b3_msb = true;
      b4_mid = false;
      b5_lsb = true;
      break;
    default:
      b3_msb = false;
      b4_mid = false;
      b5_lsb = false;
      break;
    };

    auto &b3 = telemetry_data_.getDigital(Parameter::Id::B3);
    b3.setValue(b3_msb);
    auto &b4 = telemetry_data_.getDigital(Parameter::Id::B4);
    b4.setValue(b4_mid);
    auto &b5 = telemetry_data_.getDigital(Parameter::Id::B5);
    b5.setValue(b5_lsb);
  }
  {
    auto &b6 = telemetry_data_.getDigital(Parameter::Id::B6);
    /// @todo Implement
    // b6.setValue(calculated_data.phase_prediction_valid);
  }
  {
    auto &b7 = telemetry_data_.getDigital(Parameter::Id::B7);
    b7.setValue(calculated_data.external_temperature_valid);
  }
  {
    auto &b8 = telemetry_data_.getDigital(Parameter::Id::B8);
    b8.setValue(calculated_data.barometric_pressure_valid);
  }

  data_link_.updateTelemetryData(telemetry_data_);
}

} // namespace giraffe