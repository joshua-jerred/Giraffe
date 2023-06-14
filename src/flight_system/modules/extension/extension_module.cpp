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
                                  node::Identification::EXTENSION_MODULE);

enum class modules::ExtensionModule::ExtAction {
  UNKNOWN,
  DISABLE,
  START,
  RUN,
  STOP,
  RESTART,
  ERROR_RESTART,
  ERROR_DISABLE,
};

struct modules::ExtensionModule::ExtContainer {
  std::shared_ptr<ext::Extension> extension;
  int startup_attempts = 0;
  int restart_attempts = 0;
};

modules::ExtensionModule::ExtensionModule(data::SharedData &shared_data,
                                          cfg::Configuration &config)
    : modules::Module(metadata, shared_data, config),
      extension_interfaces_(shared_data.streams) {
}

void modules::ExtensionModule::startup() {
  updateLocalConfig(); // Load in the config for the extension.
}

void modules::ExtensionModule::loop() {
}

void modules::ExtensionModule::shutdown() {
}

void modules::ExtensionModule::processCommand(const cmd::Command &command) {
  (void)command;
}

void modules::ExtensionModule::updateLocalConfig() {
  // Load in the config for the extensions.
  // extension_metadata_ = configuration_.extensions
  auto &mdl_cfg = configuration_.extension_module;
  status_polling_interval_ = mdl_cfg.getStatusPollingRate();
  max_restart_attempts_ = mdl_cfg.getMaxRestartAttempts();
  restart_delay_ = mdl_cfg.getRestartDelayMinimum();
  startup_timeout_ = mdl_cfg.getStartTimeout();
}

// State machine for the extension module.