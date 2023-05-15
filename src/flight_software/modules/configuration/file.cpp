/**
 * @file loader.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Loads the configuration from a JSON file.
 * @date 2023-05-14
 * @copyright Copyright (c) 2023
 */

#include <filesystem>
#include <fstream>
#include <iostream>

#include "configuration.h"
#include "configuration_internal.h"

using json = nlohmann::ordered_json;

bool cfg::file::saveConfiguration(cfg::Configuration &config,
                                  const std::string &file_path,
                                  bool overwrite) {
  if (!overwrite && std::filesystem::exists(file_path)) {
    std::cout << "will not overwrite config";
    return false;
  }
  std::ofstream out(file_path);
  if (out.fail()) {
    std::cout << "failed to open file";
    return false;
  }
  constexpr int indent = 2;
  std::string data = cfg::json::allToJson(config).dump(indent);
  out << data;
  return true;
}