/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   extension_module.cpp
 * @brief  The implementation of the new extension module.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-12
 * @copyright  2023 (license to be defined)
 */

#include "extension_module.hpp"

static modules::MetaData metadata("extension_module",
                                  node::Identification::EXTENSION_MODULE, 100);

modules::ExtensionModule::ExtensionModule(data::SharedData &shared_data,
                                          cfg::Configuration &config)
    : modules::Module(metadata, shared_data, config) {
}

void modules::ExtensionModule::startup() {
}

void modules::ExtensionModule::loop() {
}

void modules::ExtensionModule::shutdown() {
}

void modules::ExtensionModule::processCommand(const command::Command &command) {
  (void)command;
}