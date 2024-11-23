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
  using SettingOption = std::variant<int, bool, std::string>;
  using SettingMap = std::map<std::string, SettingOption>;

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

      if (std::holds_alternative<int>(settings_map_.at(key))) {
        if (value.is_number()) {
          settings_map_.at(key) = value.get<int>();
        } else {
          logger_.warn("Invalid type for key: " + key);
          valid = false;
        }
      } else if (std::holds_alternative<bool>(settings_map_.at(key))) {
        if (value.is_boolean()) {
          settings_map_.at(key) = value.get<bool>();
        } else {
          logger_.warn("Invalid type for key: " + key);
          valid = false;
        }
      } else if (std::holds_alternative<std::string>(settings_map_.at(key))) {
        if (value.is_string()) {
          settings_map_.at(key) = value.get<std::string>();
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

    for (const auto &[key, value] : settings_map_) {
      if (std::holds_alternative<int>(value)) {
        json_data[key] = std::get<int>(value);
      } else if (std::holds_alternative<bool>(value)) {
        json_data[key] = std::get<bool>(value);
      } else if (std::holds_alternative<std::string>(value)) {
        json_data[key] = std::get<std::string>(value);
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
    for (const auto &[key, value] : settings_map_) {
      if (std::holds_alternative<int>(value)) {
        config[key] = std::get<int>(value);
      } else if (std::holds_alternative<bool>(value)) {
        config[key] = std::get<bool>(value);
      } else if (std::holds_alternative<std::string>(value)) {
        config[key] = std::get<std::string>(value);
      } else {
        logger_.warn("Unknown setting type for key: " + key);
      }
    }

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

  giraffe::ILogger &logger_;

  SettingMap settings_map_ = {
      {"setting1", 1},
      {"setting2", true},
      {"setting3", "default"},
  };
};

} // namespace flight_system_agent