/**
 * @file module-configuration.cpp
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief This file implements the class configuration module.
 *
 * @version 0.1
 * @date 2022-10-03
 * @copyright Copyright (c) 2022
 */

#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <regex>
#include <string>
#include <vector>

#include "data-stream.h"
#include "modules.h"
using namespace modules;

/**
 * @details
 * Acts the same as the standard json object, but this maintains the same
 * structure as the config file upon loading and saving.
 * Requires nlohmann json version 3.9 or higher.
 */
using json = nlohmann::ordered_json;

/**
 * @brief The following enumerations are used to link the values
 * in the configuration file with the values of enumerations
 * defined in config-types.h
 *
 * Important note from the nlohmann json documentation:
 * "When using get<ENUM_TYPE>(), undefined JSON values will default to the first
 * pair specified in your map. Select this default pair carefully."
 *
 * This is why the first pair is 'error' so the configuration module can
 * detect an incorrect value in the config file.
 *
 * @see config-types.h
 */
NLOHMANN_JSON_SERIALIZE_ENUM(ConfigData::Mainboard,
                             {{ConfigData::Mainboard::ERROR, "error"},
                              {ConfigData::Mainboard::OTHER, "other"},
                              {ConfigData::Mainboard::PI_ZERO_W_2,
                               "pi_zero_w_2"},
                              {ConfigData::Mainboard::PI_4, "pi_4"}})

NLOHMANN_JSON_SERIALIZE_ENUM(FlightProcedure::Type,
                             {{FlightProcedure::Type::ERROR, "error"},
                              {FlightProcedure::Type::TESTING, "testing"},
                              {FlightProcedure::Type::STANDARD, "standard"},
                              {FlightProcedure::Type::RECOVERY, "recovery"},
                              {FlightProcedure::Type::FAILSAFE, "failsafe"}})

NLOHMANN_JSON_SERIALIZE_ENUM(
    ExtensionMetadata::Category,
    {{ExtensionMetadata::Category::ERROR, "error"},
     {ExtensionMetadata::Category::OTHER, "other"},
     {ExtensionMetadata::Category::GPS, "gps"},
     {ExtensionMetadata::Category::CAMERA, "camera"},
     {ExtensionMetadata::Category::BATTERY, "battery"},
     {ExtensionMetadata::Category::SYSTEM, "system"},
     {ExtensionMetadata::Category::INTERNAL_SENSOR, "internal_sensor"},
     {ExtensionMetadata::Category::EXTERNAL_SENSOR, "external_sensor"}})

NLOHMANN_JSON_SERIALIZE_ENUM(
    ExtensionMetadata::Interface,
    {{ExtensionMetadata::Interface::OTHER, "other"},
     {ExtensionMetadata::Interface::INTERNAL, "internal"},
     {ExtensionMetadata::Interface::I2C, "i2c"},
     {ExtensionMetadata::Interface::UART, "uart"},
     {ExtensionMetadata::Interface::ONEWIRE, "onewire"},
     {ExtensionMetadata::Interface::USB, "usb"},
     {ExtensionMetadata::Interface::GPIO, "gpio"}})

std::string GetString(const json& j, const std::string& key) {
  if (j.contains(key) && j[key].is_string()) {
    return j[key].get<std::string>();
  } else {
    return "";
  }
}

int GetBool(const json& j, const std::string& key) {
  if (j.contains(key) && j[key].is_boolean()) {
    return j[key].get<bool>() ? 1 : 0;
  } else {
    return -1;
  }
}

int GetInt(const json& j, const std::string& key) {
  if (j.contains(key) && j[key].is_number_integer()) {
    return j[key].get<int>();
  } else {
    return -1;
  }
}

double GetDouble(const json& j, const std::string& key) {
  if (j.contains(key) && j[key].is_number_float()) {
    return j[key].get<double>();
  } else {
    return -1;
  }
}

double IsDouble(std::string s) {
  try {
    return std::stod(s);
  } catch (std::invalid_argument const& e) {
    return -1;
  }
}

char GetChar(const json& j, const std::string& key) {
  if (j.contains(key) && j[key].is_string()) {
    std::string s = j[key].get<std::string>();
    if (s.length() == 1) {
      return s[0];
    }
  }
  return 0;
}

/** @todo Convert freq to XXX.XXXX format*/
std::string FormatFrequency(std::string frequency) { return frequency; }

/**
 * @brief Construct a new ConfigModule::ConfigModule object
 * @param None
 */
ConfigModule::ConfigModule(DataStream& data_stream)
    : Module(data_stream, configurables::prefix::kConfigModule,
             "Configuration") {
  config_data_.start_time = time(0);
}

/**
 * @brief Deconstructs the ConfigModule object
 */
ConfigModule::~ConfigModule() {}

/**
 * @brief Load and parse a configuration file from a given path.
 * @details This function will load and parse a json file from a given path.
 * If the file fails to open or if it does not exists it will return -1.
 * If it does load, it will call ParseAll()
 * @param file_path The local path to the configuration file.
 * @return int 0 if successfully loaded and parsed, -1 if the file failed to
 * open, -2 if there were any errors parsing the file.
 * @see ParseAll()
 */
int ConfigModule::load(std::string file_path) {
  data_stream_.updateModuleStatus("config", ModuleStatus::RUNNING);
  config_file_path_ = file_path;

  std::ifstream fs(config_file_path_);  // open file
  if (fs.fail()) {
    data_stream_.updateModuleStatus("config", ModuleStatus::ERROR_STATE);
    return -1;  // file failed to open
  }

  json_buffer_ = json::parse(fs);
  ParseAll();

  data_stream_.updateModuleStatus("config", ModuleStatus::STOPPED);
  return 0;
}

/**
 * @brief Returns all of the configuration data as a ConfigData object. All
 * errors will be included, refer to getErrors() for a list of errors.
 * @param None
 * @return ConfigData - All of the configuration data.
 */
ConfigData ConfigModule::getAll() { return config_data_; }

void ConfigModule::doCommand(GFSCommand command) { (void)command; }

/**
 * @brief Calls all of the parse functions to load the configuration data.
 * @param None
 * @return void
 */
void ConfigModule::ParseAll() {
  parseGeneral();
  parseExtensions();
  parseDebug();
  parseTelemetry();
  parseDataTypes();
  parseFlightProcedures();
}

/**
 * @brief Parses the general section of the configuration file.
 * @details This function pulls out the project name, mainboard type, and
 * starting proc type. It will add errors to the errors_ vector if any of the
 * required fields contain errors.
 * @param None
 * @return void
 * */
void ConfigModule::parseGeneral() {
  if (!json_buffer_.contains("general")) {
    error("GEN_NF");  // General section does not exist in config
    return;
  }

  if (!json_buffer_["general"].contains("project-name")) {
    error("GEN_PN_NF");  // Project name does not exist in config
    config_data_.general.project_name = "INVALID";
  } else {
    std::string name =
        json_buffer_["general"]["project-name"].get<std::string>();

    if (name.length() < PROJECT_NAME_MIN_LENGTH ||
        name.length() > PROJECT_NAME_MAX_LENGTH) {
      error("GEN_PN_R", name);
      config_data_.general.project_name = "INVALID";
    } else if (!std::regex_search(name, std::regex("^[a-zA-Z_ 0-9-]*$"))) {
      error("GEN_PN_I", name);
    } else {
      config_data_.general.project_name = name;
    }
  }

  if (!json_buffer_["general"].contains("main-board-type")) {
    error("GEN_MB_NF");  // Mainboard type does not exist in config
    config_data_.general.main_board = ConfigData::Mainboard::ERROR;
  } else {
    ConfigData::Mainboard mbtype =
        json_buffer_["general"]["main-board-type"].get<ConfigData::Mainboard>();
    if (mbtype == ConfigData::Mainboard::ERROR) {
      error("GEN_MB_I");
    } else {
      config_data_.general.main_board = mbtype;
    }
  }

  if (!json_buffer_["general"].contains("starting-procedure")) {
    error("GEN_SP_NF");  // Starting procedure does not exist in config
    config_data_.general.starting_proc = FlightProcedure::Type::ERROR;
  } else {
    FlightProcedure::Type ptype = json_buffer_["general"]["starting-procedure"]
                                      .get<FlightProcedure::Type>();
    if (ptype == FlightProcedure::Type::ERROR) {
      error("GEN_SP_I");
    } else {
      config_data_.general.starting_proc = ptype;
    }
  }
}

/**
 * @brief Parses the extensions section of the configuration file.
 * @details generates a vector of extension metadata objects. It checks for
 * sequential IDs, a name length, extension type, etc.
 * @param None
 * @return void
 * @todo type does not check for valid types.
 * @todo additional arguments for i2c, serial, etc. This should be changed into
 * a vector of string arguments to send to the extension as it's most flexible.
 * with all type of extensions.
 */
void ConfigModule::parseExtensions() {
  int number_of_extensions = 0;
  for (const auto& item : json_buffer_["extensions"].items()) {
    ExtensionMetadata newExtension;
    try {
      int id = item.value()["id"].get<int>();
      if (id != number_of_extensions + 1) {
        error("EXT_ID_R", id);
      } else if (id < EXTENSION_ID_MIN || id > EXTENSION_ID_MAX) {
        error("EXT_ID_S", id);
      } else {
        newExtension.id = id;
      }

      std::string name = item.value()["name"].get<std::string>();
      if (name.length() < EXTENSION_NAME_MIN_LENGTH ||
          name.length() >= EXTENSION_NAME_MAX_LENGTH) {
        error("EXT_NM_R", name);
      } else if (!std::regex_search(name, std::regex("^[a-zA-Z_0-9-]*$"))) {
        error("EXT_NM_I", name);
      } else {
        newExtension.name = name;
      }

      std::string etype = item.value()["type"].get<std::string>();
      if (etype.length() < 3 || etype.length() > 20) {
        error("EXT_TP_R", etype);
      } else {
        newExtension.extension_type = etype;
      }

      ExtensionMetadata::Category category =
          item.value()["category"].get<ExtensionMetadata::Category>();
      if (category == ExtensionMetadata::Category::ERROR) {
      } else {
        newExtension.category = category;

        if (category == ExtensionMetadata::Category::GPS) {
          if (config_data_.extensions.gps_data_name.size() > 0) {
            error("MGPS");
          } else {
            config_data_.extensions.gps_data_name = name;
          }
        } else if (category == ExtensionMetadata::Category::BATTERY) {
          if (config_data_.extensions.battery_data_name.size() > 0) {
            error("MBAT");
          } else {
            config_data_.extensions.battery_data_name = name;
          }
        } else if (category == ExtensionMetadata::Category::SYSTEM) {
          if (config_data_.extensions.system_data_name.size() > 0) {
            error("MSYS");
          } else {
            config_data_.extensions.system_data_name = name;
          }
        }
      }

      ExtensionMetadata::Interface interface =
          item.value()["interface"].get<ExtensionMetadata::Interface>();
      if (interface == ExtensionMetadata::Interface::ERROR) {
        // Report error here
      } else {
        if (interface == ExtensionMetadata::Interface::ONEWIRE) {  // On onewire

          if (!item.value().contains("onewire-id")) {
            newExtension.extra_args.one_wire_id = "NOT FOUND";
            error("EXT_OW_NF", id);
          } else {
            std::string id = item.value()["onewire-id"].get<std::string>();

            if (!std::regex_search(id, std::regex("28-[0-9&a-f]{12}"))) {
              error("EXT_OW_I", id);

            } else {
              newExtension.extra_args.one_wire_id = id;
            }
          }

        } else if (interface == ExtensionMetadata::Interface::I2C) {  // On I2C
          std::string address = item.value()["i2c-address"].get<std::string>();

          int address_num = -1;
          std::stringstream strs;
          strs << std::hex << address;  // convert hex string to int
          strs >> address_num;

          if (address_num < 0 || address_num > 127) {
            error("EXT_I2_A", address);
          } else {
            newExtension.extra_args.I2C_device_address = address;
          }

          int bus = item.value()["i2c-bus"].get<int>();
          if (bus < 0 || bus > 2) {
            error("ETC_I2_B", bus);
          } else {
            newExtension.extra_args.I2C_bus = bus;
          }
        }
        newExtension.interface = interface;
      }

      int interval = item.value()["update-interval"].get<int>();
      if (interval < EXTENSION_INTERVAL_MIN ||
          interval > EXTENSION_INTERVAL_MAX) {
        error("EXT_UI_R", interval);
        newExtension.update_interval = 1000;
      } else {
        newExtension.update_interval = interval;
      }

      bool flight_critical = item.value()["flight-critical"].get<bool>();
      if (flight_critical != 0 && flight_critical != 1) {
        if (item.value().contains("critical-source")) {
          std::string critical_source =
              item.value()["critical-source"].get<std::string>();
          if (critical_source == "PRES_MBAR") {
            config_data_.extensions.pressure_data_name = name;
          }
        }
      } else {
        newExtension.critical = flight_critical;
      }

      config_data_.extensions.extensions_list.push_back(newExtension);
      number_of_extensions++;
    } catch (const std::exception& e) {
      error("EXT_P", std::to_string(number_of_extensions++) + e.what());
    }
  }

  if (config_data_.extensions.battery_data_name.size() == 0) {
    error("NO_BAT");
  }
  if (config_data_.extensions.system_data_name.size() == 0) {
    error("NO_SYS");
  }
  if (config_data_.extensions.gps_data_name.size() == 0) {
    error("NO_GPS");
  }
}

/**
 * @brief Parses the debug section of the configuration file.
 * @param None
 * @return void
 * @todo Error checking
 */
void ConfigModule::parseDebug() {
  config_data_.debug.console_enabled =
      json_buffer_["debugging"]["console-enabled"].get<bool>();

  config_data_.debug.console_update_interval =
      json_buffer_["debugging"]["console-update-interval"].get<int>();

  config_data_.debug.web_server_enabled =
      json_buffer_["debugging"]["web-server-enabled"].get<bool>();

  config_data_.debug.web_server_socket_port =
      json_buffer_["debugging"]["web-server-socket-port"].get<int>();
}

void ConfigModule::ParseRadios(json radios_json) {
  if (!radios_json.is_array()) {
    error("RAD_NA");
    return;
  }

  int number_of_radios = radios_json.size();
  if (number_of_radios == 0) {
    error("RAD_NR");
    return;
  }

  for (int i = 0; i < number_of_radios; i++) {
    json radio = radios_json[i];
    RadioMetadata new_radio;

    int radio_id = GetInt(radio, "id");
    if (radio_id < 0) {
      error("RAD_ID_N", std::to_string(radio_id));
      return;
    }
		new_radio.radio_id = radio_id;

    std::string radio_name = GetString(radio, "name");
    if (radio_name == "") {
      error("RAD_N_N", std::to_string(radio_id));
      return;
    }
    if (radio_name.size() > EXTENSION_NAME_MAX_LENGTH || radio_name.size() < EXTENSION_NAME_MIN_LENGTH) {
      error("RAD_N_L", std::to_string(radio_id));
      return;
    }
    new_radio.radio_name = radio_name;

    std::string radio_type = GetString(radio, "radio-type");
    if (radio_type == "") {
      error("RAD_T_N", std::to_string(radio_id));
      return;
    }
		new_radio.radio_type = radio_type;

    int priority = GetInt(radio, "priority");
    if (priority < 0) {
      error("RAD_PR_N", std::to_string(radio_id));
      priority = 1;
    }

    if (radio.contains("frequency-ranges") &&
        radio["frequency-ranges"].is_array()) {
      json frequency_ranges = radio["frequency-ranges"];
      for (unsigned int j = 0; j < frequency_ranges.size(); j++) {
        json frequency_range = frequency_ranges[j];

        std::string min_frequency = GetString(frequency_range, "min");
        if (IsDouble(min_frequency) < 0) {
          error("RAD_FR_H", min_frequency);
          return;
        }

        std::string max_frequency = GetString(frequency_range, "max");
        if (IsDouble(max_frequency) < 0) {
          error("RAD_FR_L", max_frequency);
          return;
        }

        min_frequency = FormatFrequency(min_frequency);
        max_frequency = FormatFrequency(max_frequency);
        new_radio.frequency_ranges.push_back(
            std::make_pair(min_frequency, max_frequency));
      }
    }

    if (!radio.contains("modes") || !radio["modes"].is_array()) {
      error("RAD_MDS");
      return;
    }

    json& modes = radio["modes"];
    if (modes.contains("TX")) {
      new_radio.TX_enabled = true;
    }
    if (modes.contains("RX")) {
      new_radio.RX_enabled = true;
    }
    if (modes.contains("APRS")) {
      new_radio.APRS_enabled = true;
    }
    if (modes.contains("SSTV")) {
      new_radio.SSTV_enabled = true;
    }
    if (modes.contains("DATA")) {
      new_radio.DATA_enabled = true;
    }

    if (!radio.contains("features") || !radio["features"].is_array()) {
      error("RAD_FTS");
      return;
    }

    json& features = radio["features"];
    if (features.contains("frequency-switching")) {
      new_radio.frequency_switching_capable = true;
    }
    if (features.contains("power-saving")) {
      new_radio.power_saving_capable = true;
    }
    if (features.contains("high-low-power")) {
      new_radio.high_low_power_capable = true;
    }
    if (features.contains("separate-tx-rx")) {
      new_radio.separate_tx_rx_capable = true;
    }
    if (features.contains("bandwidth-switching")) {
      new_radio.bandwidth_switching_capable = true;
    }
    if (features.contains("rssi")) {
      new_radio.rssi_capable = true;
    }
    if (features.contains("volume-control")) {
      new_radio.volume_control_capable = true;
    }
    if (features.contains("squelch-control")) {
      new_radio.squelch_control_capable = true;
    }
    if (!radio.contains("interface")) {
      error("RAD_INT");
      return;
    }

    json& interface_json = radio["interface"];

    ExtensionMetadata::Interface interface;
    try {
      interface = interface_json["type"].get<ExtensionMetadata::Interface>();
      if (interface == ExtensionMetadata::Interface::ERROR) {
        error("RAD_INT_E");
      }
    } catch (nlohmann::detail::type_error& e) {
      error("RAD_INT_U");
      return;
    }
    std::cout << "Radio 1" << std::endl;
    if (interface == ExtensionMetadata::Interface::UART) {
      new_radio.interface = ExtensionMetadata::Interface::UART;

      std::string uart_path = GetString(interface_json, "uart-port");
      if (uart_path.size() == 0) {
        error("RAD_INT_U");
        return;
      }
      new_radio.extra_args.uart_device_path = uart_path;

      int baud_rate = GetInt(interface_json, "uart-baud-rate");
      if (baud_rate < 50) {
        error("RAD_UART_BR");
        return;
      }
      new_radio.extra_args.uart_baud_rate = baud_rate;
    } else {
      error("RAD_INT_NI");
    }

    if (interface_json.contains("gpio") && interface_json["gpio"].is_array()) {
      json &gpio_json = interface_json["gpio"];
      int ptt = GetInt(gpio_json, "ptt");
      int power = GetInt(gpio_json, "power");
      int squelch = GetInt(gpio_json, "squelch-detect");

      new_radio.gpio_ptt = ptt > 0 ? ptt : -1;
      new_radio.gpio_power = power > 0 ? power : -1;
      new_radio.gpio_squelch_detect = squelch > 0 ? squelch : -1;
    }

    config_data_.telemetry.radios.push_back(new_radio);
  }
}
/**
 * @brief Parses the telemetry section of the configuration file.
 * @details Currently not fully implemented. It will first check in the config
 * file to see if telemetry is enabled in the config. If it isn't it will
 * immediately return. If it is enabled, it will read in the rest of the
 * telemetry configuration. This is currently not fully implemented and will
 * only read the callsign.
 * @param None
 * @return void
 * @todo Error Checking
 * @todo Implement the rest of the telemetry configuration
 */
void ConfigModule::parseTelemetry() {
  json& telemetry = json_buffer_["telemetry"];

  config_data_.telemetry.telemetry_enabled = false;
  int telem_enabled = GetBool(telemetry, "telemetry-enabled");
  if (telem_enabled == 0) {
    return;
  } else if (telem_enabled != 1) {
    error("TEL_EN");
    return;
  }

  std::string callsign = GetString(telemetry, "callsign");
  if (callsign == "" || callsign == "NOCALL") {
    error("TEL_CS_NF", callsign);
    return;
  } else if (callsign.size() > 6 || callsign.size() < 3) {
    error("TEL_CS_L", callsign);
    return;
  }

  if (!telemetry.contains("radios")) {
    error("TEL_NO_RAD");  // Radios required for telemetry
    return;
  } else {
    ParseRadios(telemetry["radios"]);
  }

  // Ensure callsign is uppercase
  std::transform(callsign.begin(), callsign.end(), callsign.begin(),
                 [](unsigned char c) { return std::toupper(c); });

  config_data_.telemetry.telemetry_enabled = true;
  config_data_.telemetry.call_sign = callsign;

  // First disable modes
  config_data_.telemetry.aprs_enabled = false;
  config_data_.telemetry.sstv_enabled = false;
  config_data_.telemetry.data_packets_enabled = false;
  // APRS Configuration ------------------------------------------------------
  if (json_buffer_["telemetry"].contains("aprs") &&
      (GetBool(json_buffer_["telemetry"]["aprs"], "enabled") == 1)) {
    // APRS is enabled
    json& aprs = json_buffer_["telemetry"]["aprs"];

    // Load APRS configuration
    std::string aprs_frequency = GetString(aprs, "frequency");
    int ssid = GetInt(aprs, "ssid");
    std::string destination_address = GetString(aprs, "destination-address");
    int destination_ssid = GetInt(aprs, "destination-ssid");
    char symbol_table = GetChar(aprs, "symbol-table");
    char symbol = GetChar(aprs, "symbol");
    std::string comment = GetString(aprs, "comment");
    int position_packets = GetBool(aprs, "position-enabled");
    int telemetry_packets = GetBool(aprs, "telemetry-enabled");

    // Check for errors and set defaults
    double freq = IsDouble(aprs_frequency);
    if ((freq < 144 || freq > 148) && (freq < 420 || freq > 450)) {
      error("APRS_F", aprs_frequency);
      aprs_frequency = configurables::config_defaults::kAprsFrequency;
    }

    if (ssid < 0 || ssid > 15) {
      error("APRS_SSID", std::to_string(ssid));
      ssid = 0;
    }

    if (destination_address.size() > 6) {
      error("APRS_DA", destination_address);
      destination_address =
          configurables::config_defaults::kAprsDestinationAddress;
    }
    // Convert to uppercase
    std::transform(destination_address.begin(), destination_address.end(),
                   destination_address.begin(),
                   [](unsigned char c) { return std::toupper(c); });

    if (symbol_table != '/' && symbol_table != '\\') {
      error("APRS_ST", std::to_string(symbol_table));
      symbol_table = '/';  // Default to primary
    }

    if (symbol < '!' || symbol > '~') {
      error("APRS_S", std::to_string(symbol));
      symbol = '/';  // Default to dot
    }

    if (comment.size() > configurables::config_limits::kAprsMemoMaxSize) {
      error("APRS_M", std::to_string(comment.size()));
      comment =
          comment.substr(0, configurables::config_limits::kAprsMemoMaxSize);
    }

    if (position_packets != 0 && position_packets != 1) {
      error("APRS_PP");
      position_packets =
          configurables::config_defaults::kAprsPositionPacketEnabled;
    }

    if (telemetry_packets != 0 && telemetry_packets != 1) {
      error("APRS_TP");
      telemetry_packets =
          configurables::config_defaults::kAprsTelemetryPacketEnabled;
    }

    // Set in config_data_
    config_data_.telemetry.aprs_enabled = true;
    config_data_.telemetry.aprs_freq = aprs_frequency;
    config_data_.telemetry.aprs_ssid = ssid;
    config_data_.telemetry.aprs_destination_address = destination_address;
    config_data_.telemetry.aprs_destination_ssid = destination_ssid;
    config_data_.telemetry.aprs_alternate_symbol_table = (symbol_table == '\\');
    config_data_.telemetry.aprs_symbol = symbol;
    config_data_.telemetry.aprs_comment = comment;
    config_data_.telemetry.aprs_position_packets = position_packets;
    config_data_.telemetry.aprs_telemetry_packets = telemetry_packets;
  }  // End APRS

  // SSTV Configuration
  // -------------------------------------------------------
  if (json_buffer_["telemetry"].contains("sstv") &&
      (GetBool(json_buffer_["telemetry"]["sstv"], "enabled") == 1)) {
    // SSTV is enabled
    json& sstv = json_buffer_["telemetry"]["sstv"];

    // Load SSTV configuration
    std::string sstv_frequency = GetString(sstv, "frequency");
    std::string sstv_mode = GetString(sstv, "mode");
    std::string sstv_comment = GetString(sstv, "comment");
    int sstv_save_images = GetBool(sstv, "save-images");
    int sstv_overlay_data = GetBool(sstv, "overlay-data");

    // Check for errors and set defaults
    double freq = IsDouble(sstv_frequency);
    if (freq < 20 || freq > 480) {
      error("SSTV_F", sstv_frequency);
      sstv_frequency = configurables::config_defaults::kSstvFrequency;
    }

    if (sstv_mode != "robot36") {  // Only mode supported for now
      error("SSTV_M", sstv_mode);
      sstv_mode = configurables::config_defaults::kSstvMode;
    }

    if (sstv_comment.size() >
        configurables::config_limits::kSstvCommentMaxSize) {
      error("SSTV_C", std::to_string(sstv_comment.size()));
      sstv_comment = sstv_comment.substr(
          0, configurables::config_limits::kSstvCommentMaxSize);
    }

    if (sstv_save_images != 0 && sstv_save_images != 1) {
      error("SSTV_SI");
      sstv_save_images = configurables::config_defaults::kSstvSaveImages;
    }

    if (sstv_overlay_data != 0 && sstv_overlay_data != 1) {
      error("SSTV_OD");
      sstv_overlay_data = configurables::config_defaults::kSstvOverlayData;
    }

    // Set in config_data_
    config_data_.telemetry.sstv_enabled = true;
    config_data_.telemetry.sstv_freq = sstv_frequency;
    config_data_.telemetry.sstv_mode = sstv_mode;
    config_data_.telemetry.sstv_comment = sstv_comment;
    config_data_.telemetry.sstv_save_images = sstv_save_images;
    config_data_.telemetry.sstv_overlay_data = sstv_overlay_data;
  }  // End SSTV

  // Data Packets Configuration
  // ------------------------------------------------
  if (json_buffer_["telemetry"].contains("data-packets") &&
      (GetBool(json_buffer_["telemetry"]["data-packets"], "enabled") == 1)) {
    // Data Packets is enabled
    json& data_packets = json_buffer_["telemetry"]["data-packets"];

    // Load Data Packets configuration
    std::string data_packets_frequency = GetString(data_packets, "frequency");
    std::string data_packets_mode = GetString(data_packets, "mode");

    // Check for errors and set defaults
    double freq = IsDouble(data_packets_frequency);
    if (freq < 20 || freq > 480) {
      error("DP_F", data_packets_frequency);
      data_packets_frequency =
          configurables::config_defaults::kDataPacketsFrequency;
    }

    if (data_packets_mode != "bpsk125" && data_packets_mode != "bpsk250" &&
        data_packets_mode != "bpsk500" && data_packets_mode != "bpsk1000" &&
        data_packets_mode != "qpsk125" && data_packets_mode != "qpsk250" &&
        data_packets_mode != "qpsk500") {
      error("DP_M", data_packets_mode);
      data_packets_mode = configurables::config_defaults::kDataPacketsMode;
    }

    // Set in config_data_
    config_data_.telemetry.data_packets_enabled = true;
    config_data_.telemetry.data_packets_freq = data_packets_frequency;
    config_data_.telemetry.data_packets_mode = data_packets_mode;
  }  // End Data Packets
}

/**
 * @brief Parses the data type section of the configuration file.
 * @details This will read in the data types from the configuration file. This
 * is what determines if it will be included in telemetry or not. Name is
 * used by the extensions, unit is used by the telemetry and should be a
 * couple characters in length.
 * @param None
 * @return void
 * @todo Update error checking.
 */
void ConfigModule::parseDataTypes() {
  for (const auto& item :
       json_buffer_["data-log-data-and-packet-contents"].items()) {
    ConfigData::DataTypes::DataType newDataType;
    try {
      newDataType.source = item.value()["source"].get<std::string>();
      newDataType.unit = item.value()["unit"].get<std::string>();
      newDataType.include_in_telemetry =
          item.value()["include-in-telemetry"].get<bool>();
      newDataType.telemetry_name =
          item.value()["telemetry-name"].get<std::string>();

      config_data_.data_types.types.push_back(newDataType);
    } catch (const std::exception& e) {
      // errors_.push_back("Error parsing data-types." + (std::string)
      // e.what());
    }
  }
}

/**
 * @brief Parses the flight proc section of the configuration file.
 * @details This is the portion that contains the actual tasks of the flight.
 * @param None
 * @return void
 */
void ConfigModule::parseFlightProcedures() {
  for (const auto& item : json_buffer_["flight-procedures"].items()) {
    FlightProcedure newFlightProcedure;

    newFlightProcedure.type = item.value()["type"].get<FlightProcedure::Type>();

    if (newFlightProcedure.type == FlightProcedure::Type::ERROR) {
      // errors_.push_back("Invalid flight proc type.");
    }

    newFlightProcedure.enabled = item.value()["enabled"].get<bool>();

    newFlightProcedure.intervals.data_log =
        item.value()["intervals"]["data-log"].get<int>();
    newFlightProcedure.intervals.data_packet =
        item.value()["intervals"]["data-packet"].get<int>();
    newFlightProcedure.intervals.sstv =
        item.value()["intervals"]["sstv"].get<int>();
    newFlightProcedure.intervals.aprs =
        item.value()["intervals"]["aprs"].get<int>();
    newFlightProcedure.intervals.picture =
        item.value()["intervals"]["picture"].get<int>();
    newFlightProcedure.intervals.health_check =
        item.value()["intervals"]["health-check"].get<int>();

    if (newFlightProcedure.type == FlightProcedure::Type::TESTING) {
      config_data_.flight_procs.testing = newFlightProcedure;
    } else if (newFlightProcedure.type == FlightProcedure::Type::STANDARD) {
      config_data_.flight_procs.standard = newFlightProcedure;
    } else if (newFlightProcedure.type == FlightProcedure::Type::RECOVERY) {
      config_data_.flight_procs.recovery = newFlightProcedure;
    }
  }
}