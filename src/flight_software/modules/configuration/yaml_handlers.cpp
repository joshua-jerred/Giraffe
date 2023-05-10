/**
 * @file yaml_handlers.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief 
 * @date 2023-05-10
 * @copyright Copyright (c) 2023
 */

#include "yaml_handlers.h"

void yamlAddGeneral(cfg::General &cfg, YAML::Node &node) {
    node["general"]["project_name"] = cfg.project_name;
    //node["general"]["main_board"] = 
}