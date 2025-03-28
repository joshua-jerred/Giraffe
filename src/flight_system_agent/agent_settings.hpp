/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   agent_settings.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2024-11-23
/// @copyright  2024 (license to be defined)

#pragma once

#include <fstream>
#include <map>
#include <string>
#include <variant>

#include <BoosterSeat/filesystem.hpp>

#include "agent_data.hpp"
#include "giraffe_file_paths.hpp"
#include "i_logger.hpp"
#include "json.hpp"

namespace bsfs = bst::filesystem;

namespace flight_system_agent {

class AgentSettings {
public:
  enum class SettingType { INT, BOOL, STRING };
  struct Setting {
    std::string name;
    SettingType type;
    std::variant<int, bool, std::string> value;
  };

  AgentSettings(AgentData &agent_data, giraffe::ILogger &logger)
      : agent_data_(agent_data), logger_(logger) {
    if (!loadConfig()) {
      logger_.error("Failed to load initial configuration!");
    } else {
      logger_.info("Loaded initial configuration.");
    }
  }

  ~AgentSettings() = default;

  AgentSettings(const AgentSettings &) = delete;
  AgentSettings &operator=(const AgentSettings &) = delete;
  AgentSettings(AgentSettings &&) = delete;

  bool setFromJson(const Json &json_data) {
    bool valid = true;

    for (auto &[key, value] : json_data.items()) {
      if (!settings_map_.contains(key)) {
        logger_.warn("Unknown setting key: " + key);
        valid = false;
        continue;
      }

      if (settings_map_.at(key).type == SettingType::INT &&
          std::holds_alternative<int>(settings_map_.at(key).value)) {
        if (value.is_number()) {
          settings_map_.at(key).value = value.get<int>();
        } else {
          logger_.warn("Invalid type for key: " + key);
          valid = false;
        }
      } else if (settings_map_.at(key).type == SettingType::BOOL &&
                 std::holds_alternative<bool>(settings_map_.at(key).value)) {
        if (value.is_boolean()) {
          settings_map_.at(key).value = value.get<bool>();
        } else {
          logger_.warn("Invalid type for key: " + key);
          valid = false;
        }
      } else if (settings_map_.at(key).type == SettingType::STRING &&
                 std::holds_alternative<std::string>(
                     settings_map_.at(key).value)) {
        if (value.is_string()) {
          settings_map_.at(key).value = value.get<std::string>();
        } else {
          logger_.warn("Invalid type for key: " + key);
          valid = false;
        }
      } else {
        logger_.warn("Unknown setting type for key: " + key);
        valid = false;
      }
    }

    return saveConfig() && valid;
  }

  void getSettingsJson(Json &json_data) {
    json_data.clear();

    for (const auto &[key, setting] : settings_map_) {
      if (std::holds_alternative<int>(setting.value)) {
        json_data[key] = std::get<int>(setting.value);
      } else if (std::holds_alternative<bool>(setting.value)) {
        json_data[key] = std::get<bool>(setting.value);
      } else if (std::holds_alternative<std::string>(setting.value)) {
        json_data[key] = std::get<std::string>(setting.value);
      } else {
        logger_.warn("Unknown setting type for key: " + key);
      }
    }
  }

private:
  bool loadConfig() {
    // Attempt to load the config file if it exists
    try {
      if (!bsfs::doesFileExist(settings_file_path_)) {
        logger_.warn("Config file does not exist. Creating...");
        bsfs::createFile(settings_file_path_);
        saveConfig();
        return true;
      }

      Json config;
      std::ifstream config_file(settings_file_path_);
      config = Json::parse(config_file);
      return setFromJson(config);
    } catch (const std::exception &e) {
      logger_.error("Failed to open and parse config file: " +
                    std::string(e.what()));
    }
    return false;
  }

  bool saveConfig() {
    Json config;
    getSettingsJson(config);

    try {
      std::ofstream config_file(settings_file_path_);
      config_file << config.dump(2);
      config_file.close();
      return true;
    } catch (const std::exception &e) {
      logger_.error("Failed to save config file: " + std::string(e.what()));
    }
    return false;
  }

  const std::string settings_file_path_ =
      giraffe::file_paths::getFlightSystemAgentConfigFilePath();

  AgentData &agent_data_;

  //<{{settings_map_}}@
  // clang-format off
  std::map<std::string, AgentSettings::Setting> settings_map_{
    {"gfs_monitoring", AgentSettings::Setting{"gfs_monitoring", AgentSettings::SettingType::BOOL, true}},
    {"monitoring_interval", AgentSettings::Setting{"monitoring_interval", AgentSettings::SettingType::INT, 5000}},
    {"restart_enabled", AgentSettings::Setting{"restart_enabled", AgentSettings::SettingType::BOOL, true}},
    {"restart_threshold", AgentSettings::Setting{"restart_threshold", AgentSettings::SettingType::INT, 10000}},
    {"backup_configuration", AgentSettings::Setting{"backup_configuration", AgentSettings::SettingType::STRING, "~/.giraffe/gfs_config_backup.json"}},
  };
  // clang-format on
  //@{{settings_map_}}>

  giraffe::ILogger &logger_;
};

} // namespace flight_system_agent