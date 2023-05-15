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
                                  const std::string &file_path) {
  std::string data = cfg::json::allToJson(config).dump();
  std::cout << data;
  return true;
}