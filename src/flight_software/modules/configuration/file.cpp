/**
 * @file loader.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Loads the configuration from a JSON file.
 * @date 2023-05-14
 * @copyright Copyright (c) 2023
 */

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

#include "configuration_internal.h"
#include "configuration.h"

using json = nlohmann::ordered_json;

bool cfg::file::saveConfiguration(const cfg::Configuration &config, const std::string &file_path){
  return true;
}