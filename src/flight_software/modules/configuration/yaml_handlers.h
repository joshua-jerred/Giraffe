/**
 * @file yaml_handlers.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief
 * @date 2023-05-10
 * @copyright Copyright (c) 2023
 */

#ifndef YAML_HANDLERS_H_
#define YAML_HANDLERS_H_

#include "configuration_defaults.h"
#include "yaml-cpp/yaml.h"

// Utilities

// Structure to Yaml
void yamlAddGeneral(YAML::Node &node, cfg::General &cfg);

// Prase
inline bool CategoryExists(YAML::Node &node, std::string key) {
  return node[key].IsDefined();
}

inline bool getString(
    YAML::Node &node,
    const std::string &key,
    std::string &result);

inline bool getInt(
    YAML::Node &node,
    const std::string &key,
    int &result);

#endif
