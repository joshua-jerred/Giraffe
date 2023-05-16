/**
 * @file loader.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Loads the configuration from a JSON file.
 * @date 2023-05-14
 * @copyright Copyright (c) 2023
 */

#include <filesystem>
#include <fstream>

#include "configuration.h"
#include "configuration_internal.h"

using json = nlohmann::ordered_json;

void cfg::file::saveConfiguration(cfg::Configuration &config,
                                  const std::string &file_path,
                                  bool overwrite) {
  if (!overwrite && std::filesystem::exists(file_path)) {
    throw cfg::ConfigurationException(
        "SV_NO_OW", "will not overwrite the configuration file");
  }
  std::ofstream out(file_path);
  if (out.fail()) {
    throw cfg::ConfigurationException("SV_FOF",
                                      "failed to open the file while saving");
  }
  constexpr int indent = 2;
  std::string data = cfg::json::allToJson(config).dump(indent);
  out << data;
}

inline bool valid_section(const json &config_json, const std::string &section_key) {
  return config_json.contains(section_key) && config_json[section_key].is_object();
}

void cfg::file::loadConfiguration(cfg::Configuration &config,
                                  const std::string &file_path) {
  using json = nlohmann::ordered_json;

  if (!std::filesystem::exists(file_path)) {
    throw cfg::ConfigurationException(
        "LD_FDE", "while loading, file does not exist: " + file_path);
  }
  std::ifstream in(file_path);
  if (in.fail()) {
    throw cfg::ConfigurationException(
        "LD_FOF", "could not open the file while loading: " + file_path);
  }

  json parsed;
  try {
    parsed = json::parse(in);
  } catch (json::parse_error &e) {
    throw cfg::ConfigurationException("LD_PERR",
                                      "while parsing the file: " + file_path);
  }

  if (valid_section(parsed, "general")) {

  }

}