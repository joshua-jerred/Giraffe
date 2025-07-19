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
#include "giraffe_assert.hpp"

// ---- EXTENSIONS ----
#include "bme280.hpp"
#include "bmi088.hpp"
#include "ds18b20.hpp"
#include "mcp3021.hpp"
#include "rgb_status_led.hpp"
#include "samm8q.hpp"
// --------------------

#if RUN_IN_SIMULATOR == 1
#include "simulated_extensions.hpp"
#endif

#if GFS_UNIT_TEST_BUILD == 1
#include "unit_test_extension.hpp"
#endif

namespace modules {

static MetaData metadata("extension_module",
                         node::Identification::EXTENSION_MODULE, 50);

ExtensionModule::ExtensionModule(data::SharedData &shared_data,
                                 cfg::Configuration &config)
    : modules::Module(metadata, shared_data, config),
      extension_resources_(shared_data.streams,
                           config.hardware_interface.getI2CBus(),
                           shared_data.status_led) {
}

void ExtensionModule::startup() {
  updateLocalConfig(); // Load in the config for the extension.
  shared_data_.status_led.setBlue(giraffe::StatusLedState::State::BLINK);

// Load in the simulated extensions.
#ifdef RUN_IN_SIMULATOR
  for (auto &ext_meta : extension::K_SIMULATED_EXTENSIONS_VEC) {
    // Create the extension.
    auto extension = createExtension(ext_meta);
    if (!extension.has_value()) {
      error(DiagnosticId::EXTENSION_MODULE_failedToCreate, ext_meta.name);
      continue;
    }
    info("Loaded simulated extension: " + ext_meta.name);
    // Add the extension to the list of extensions.
    extensions_.push_back(extension.value());
  }
#endif

  // Load in the extensions
  for (auto &ext_meta : configuration_.extensions.getExtensions()) {
    addExtension(ext_meta);
  }
}

void ExtensionModule::addExtension(const cfg::ExtensionMetadata &ext_meta) {
  // Create the extension.
  auto extension = createExtension(ext_meta);
  if (!extension.has_value()) {
    error(DiagnosticId::EXTENSION_MODULE_failedToCreate, ext_meta.name);
    return;
  }
  info("Loaded extension: " + ext_meta.name);
  // Add the extension to the list of extensions.
  extensions_.push_back(extension.value());
}

void ExtensionModule::loop() {
  if (!status_polling_timer_.isDone()) {
    return;
  }
  status_polling_timer_.reset();

  stats_.num_configured = extension_metadata_.size();
  stats_.num_active = 0;
  stats_.num_inactive = 0;

  // Send the extensions through the state machine.
  for (auto &ext : extensions_) {
    stateMachine(ext);

    auto status = ext.extension->getStatus();
    if (static_cast<uint16_t>(status) & node::K_ACTIVE_STATUSES) {
      stats_.num_active++;
    } else {
      stats_.num_inactive++;
    }
  }

  /// @todo this really doesn't belong here. This data is accessible via shared
  /// data, there should be a common LED controller
  if (stats_.num_inactive == 0) {
    shared_data_.status_led.setBlue(giraffe::StatusLedState::State::OFF);
  }

  /// @todo Consider updating the below less often, the extension module runs
  /// a lot. Maybe it isn't worth fixing now /shrug

  // Update the info for individual extensions.
  stats_.extension_statuses.clear();
  for (auto &ext : extensions_) {
    using namespace data::blocks;

    stats_.extension_statuses.push_back(ExtensionModuleStats::ExtensionStatus{
        .name = ext.metadata.name,
        .control_status = actionToString(ext.action),
        .internal_status = ext.extension->getStatus()});
  }

  // Update the stats.
  shared_data_.blocks.extension_module_stats.set(stats_);
}

void ExtensionModule::shutdown() {
  // Shutdown the extensions that are running.
  const std::string indent = "    ";
  for (auto &ext : extensions_) {
    std::cout << indent << "stopping extension: " << ext.metadata.name;
    if (static_cast<uint16_t>(ext.extension->getStatus()) &
        node::K_ACTIVE_STATUSES) {
      ext.extension->stop();
      std::cout << " done" << std::endl;
    } else {
      std::cout << " not running" << std::endl;
    }
  }
}

void ExtensionModule::processCommand(const cmd::Command &command) {
  switch (command.command_id) {
  case cmd::CommandId::EXTENSION_MODULE_addPreConfiguredExtension:
    addPreConfiguredExtension(command.str_arg);
    break;
  case cmd::CommandId::EXTENSION_MODULE_disableExtension: {
    // Find the extension by name.
    auto ext = std::find_if(extensions_.begin(), extensions_.end(),
                            [&command](const ExtContainer &ext) {
                              return ext.metadata.name == command.str_arg;
                            });

    if (ext == extensions_.end()) {
      error(DiagnosticId::EXTENSION_MODULE_failedToFindExtension,
            "stop: " + command.str_arg);
      std::cout << "stop fail " + command.str_arg << std::endl;
      return;
    }

    // Stop the extension.
    ext->action = ExtAction::DISABLE;
    info("cmd stop: " + command.str_arg);
    std::cout << "good stop : " + command.str_arg << std::endl;
  } break;
  case cmd::CommandId::EXTENSION_MODULE_enableExtension: {
    // Find the extension by name.
    auto ext = std::find_if(extensions_.begin(), extensions_.end(),
                            [&command](const ExtContainer &ext) {
                              return ext.metadata.name == command.str_arg;
                            });

    if (ext == extensions_.end()) {
      error(DiagnosticId::EXTENSION_MODULE_failedToFindExtension,
            "start: " + command.str_arg);
      return;
    }

    // Start the extension if it is not running.
    if (static_cast<uint16_t>(ext->extension->getStatus()) &
        node::K_INACTIVE_STATUSES) {
      ext->action = ExtAction::START;
      info("cmd start: " + command.str_arg);
    } else {
      error(DiagnosticId::EXTENSION_MODULE_invalidCommand,
            "enable: " + command.str_arg);
    }
  } break;
  case cmd::CommandId::EXTENSION_MODULE_restartExtension: {
    // Find the extension by name.
    auto ext = std::find_if(extensions_.begin(), extensions_.end(),
                            [&command](const ExtContainer &ext) {
                              return ext.metadata.name == command.str_arg;
                            });

    if (ext == extensions_.end()) {
      error(DiagnosticId::EXTENSION_MODULE_failedToFindExtension,
            "restart: " + command.str_arg);
      std::cout << "restart: " << command.str_arg << std::endl;
      return;
    }
    std::cout << "restart good: " << command.str_arg << std::endl;
    // Restart the extension.
    ext->action = ExtAction::RESTART;
    info("cmd restart: " + command.str_arg);
  } break;
  default:
    error(DiagnosticId::EXTENSION_MODULE_invalidCommand);
    break;
  }
}

void ExtensionModule::updateLocalConfig() {
  // Load in the config for the extensions.
  extension_metadata_ = configuration_.extensions.getExtensions();
  auto &mdl_cfg = configuration_.extension_module;
  status_polling_interval_ = mdl_cfg.getStatusPollingRate();
  max_restart_attempts_ = mdl_cfg.getMaxRestartAttempts();
  restart_delay_ = mdl_cfg.getRestartDelayMinimum();
  startup_timeout_ = mdl_cfg.getStartTimeout();
  max_startup_attempts_ = mdl_cfg.getMaxStartupAttempts();

  status_polling_timer_.setTimeout(status_polling_interval_);

  /**
   * @todo Implement the loading/updating of extensions
   */
}

std::optional<ExtensionModule::ExtContainer>
ExtensionModule::createExtension(const cfg::ExtensionMetadata &meta) {
  std::shared_ptr<extension::Extension> extension;
  std::optional<ExtContainer> option;

  switch (meta.type) {
  case cfg::gEnum::ExtensionType::UNKNOWN: // generally means config error
    return option;
#if RUN_IN_SIMULATOR == 1
  case cfg::gEnum::ExtensionType::SIM_TEST:
    extension =
        std::make_shared<extension::SimTest>(extension_resources_, meta);
    break;
  case cfg::gEnum::ExtensionType::SIM_TEMP:
    extension = std::make_shared<extension::SimTemperatureSensor>(
        extension_resources_, meta);
    break;
  case cfg::gEnum::ExtensionType::SIM_PRES:
    extension = std::make_shared<extension::SimPressureSensor>(
        extension_resources_, meta);
    break;
  case cfg::gEnum::ExtensionType::SIM_HUM:
    extension = std::make_shared<extension::SimHumiditySensor>(
        extension_resources_, meta);
    break;
  case cfg::gEnum::ExtensionType::SIM_GPS:
    extension =
        std::make_shared<extension::SimGpsSensor>(extension_resources_, meta);
    break;
  case cfg::gEnum::ExtensionType::SIM_IMU:
    extension =
        std::make_shared<extension::SimImuSensor>(extension_resources_, meta);
    break;
  case cfg::gEnum::ExtensionType::SIM_ADC:
    extension =
        std::make_shared<extension::SimAdcSensor>(extension_resources_, meta);
    break;
  case cfg::gEnum::ExtensionType::SIM_CAMERA:
    extension =
        std::make_shared<extension::SimCamera>(extension_resources_, meta);
    break;
#endif
#if GFS_UNIT_TEST_BUILD
  case cfg::gEnum::ExtensionType::UNIT_TEST_EXTENSION:
    extension = std::make_shared<extension::UnitTestExtension>(
        extension_resources_, meta);
    break;
#endif
  case cfg::gEnum::ExtensionType::RGB_LED:
    extension =
        std::make_shared<extension::RgbStatusLed>(extension_resources_, meta);
    break;
  case cfg::gEnum::ExtensionType::BME280:
    extension = std::make_shared<extension::Bme280>(extension_resources_, meta);
    break;
  case cfg::gEnum::ExtensionType::SAM_M8Q:
    extension = std::make_shared<extension::SamM8q>(extension_resources_, meta);
    break;
  case cfg::gEnum::ExtensionType::DS18B20:
    extension =
        std::make_shared<extension::Ds18b20>(extension_resources_, meta);
    break;
  case cfg::gEnum::ExtensionType::MCP3021:
    extension =
        std::make_shared<extension::Mcp3021>(extension_resources_, meta);
    break;
    // case cfg::gEnum::ExtensionType::MAX11615:
    // extension =
    // std::make_shared<extension::Max11615>(extension_resources_, meta);
    // break;
  case cfg::gEnum::ExtensionType::BMI088:
    extension = std::make_shared<extension::Bmi088>(extension_resources_, meta);
    break;
  default:
    error(DiagnosticId::EXTENSION_MODULE_invalidExtensionType,
          "type: " + std::to_string(static_cast<uint16_t>(meta.type)));
    return option;
  }

  option = ExtContainer(extension, meta, startup_timeout_, restart_delay_);
  return option;
}

// State machine for the extension module.
void ExtensionModule::stateMachine(ExtContainer &ext) {
  switch (ext.action) {
  case ExtAction::UNKNOWN:
    unknownState(ext);
    break;
  case ExtAction::DISABLE:
    disableState(ext);
    break;
  case ExtAction::START:
    startState(ext);
    break;
  case ExtAction::RUN:
    runState(ext);
    break;
  case ExtAction::STOP:
    stopState(ext);
    break;
  case ExtAction::RESTART:
    restartState(ext);
    break;
  case ExtAction::ERROR_START:
    errorStartState(ext);
    break;
  case ExtAction::ERROR_RESTART:
    errorRestartState(ext);
    break;
  case ExtAction::ERROR_STOP:
    giraffe_assert(false);
    stopState(ext);
    break;
  case ExtAction::ERROR_DISABLE:
    errorDisableState(ext);
    break;
  default:
    giraffe_assert(false);
    stopState(ext);
    break;
  }
}

void ExtensionModule::unknownState(ExtContainer &ext) {
  if (!ext.metadata.enabled) { // If the extension is disabled.
    ext.action = ExtAction::DISABLE;
    return;
  }

  ext.action = ExtAction::START;
  ext.startup_shutdown_timer.setTimeout(startup_timeout_);
  ext.startup_shutdown_timer.reset();
}

void ExtensionModule::disableState(ExtContainer &ext) {
  auto status = ext.extension->getStatus();

  // If the extension is running, stop it.
  if (static_cast<uint16_t>(status) & node::K_ACTIVE_STATUSES) {
    ext.extension->stop();
    return;
  }
  // Otherwise, the extension stays here.
}

void ExtensionModule::startState(ExtContainer &ext) {
  auto status = ext.extension->getStatus();
  if (status == node::Status::STOPPED) {
    ext.extension->start();
    return;
  } else if (status == node::Status::RUNNING) {
    ext.action = ExtAction::RUN; // Extension started
    return;
  } else if (status == node::Status::STARTING) {
    if (ext.startup_shutdown_timer.isDone()) { // did not start in time
      ext.action = ExtAction::ERROR_START;
      ext.startup_shutdown_timer.reset();
    }
    return;
  } else if (status == node::Status::ERROR) {
    ext.action = ExtAction::ERROR_RESTART;
    return;
  }
  giraffe_assert(false); // Should never get here.
}

void ExtensionModule::runState(ExtContainer &ext) {
  auto status = ext.extension->getStatus();
  if (status == node::Status::RUNNING) {
    // Extension running
    /// @todo watchdog timer?
    return;
  } else if (status == node::Status::ERROR) {
    ext.action = ExtAction::ERROR_RESTART;
    return;
  }
  giraffe_assert(false); // Should never get here.
}

void ExtensionModule::stopState(ExtContainer &ext) {
  auto status = ext.extension->getStatus();
  if (status == node::Status::RUNNING) {
    ext.extension->stop();
    return;
  } else if (status == node::Status::STOPPING) {
    if (ext.startup_shutdown_timer.isDone()) { // did not stop in time
      giraffe_assert(false);                   // Not implemented yet
    }
    return;
  } else if (status == node::Status::STOPPED) {
    // Extension stopped
    return;
  } else if (status == node::Status::ERROR) {
    /// @todo handle the fault code?
    return;
  }
  giraffe_assert(false); // Should never get here.
}

void ExtensionModule::restartState(ExtContainer &ext) {
  const auto status = ext.extension->getStatus();
  if (status == node::Status::RUNNING) { // If it's running, stop it.
    ext.extension->stop();
    ext.startup_shutdown_timer.setTimeout(startup_timeout_);
    ext.startup_shutdown_timer.reset();
  } else if (status == node::Status::STOPPED) { // If it's stopped, pass off to
                                                // the start state.
    info("stopped, restarting: " + ext.metadata.name);

    ext.action = ExtAction::START;
    ext.startup_shutdown_timer.setTimeout(startup_timeout_);
    ext.startup_shutdown_timer.reset();
  } else if (status == node::Status::STOPPING) { // If it's stopping, check the
                                                 // timer.
    if (ext.startup_shutdown_timer.isDone()) { // If it didn't stop in time, go
                                               // to the error state.
      ext.action = ExtAction::ERROR_RESTART;
      return;
    }
  } else if (status == node::Status::ERROR) {
    ext.action = ExtAction::ERROR_RESTART;
  } else {
    giraffe_assert(false); // Should never get here.
  }
}

void ExtensionModule::errorStartState(ExtContainer &ext) {
  auto status = ext.extension->getStatus();
  if (status == node::Status::ERROR) {
    if (ext.startup_attempts < max_startup_attempts_) {
      /// @todo handle the fault code
      /// @todo add delay
      ext.startup_attempts++;
      ext.extension->reset();
      ext.action = ExtAction::START;
      return;
    } else {
      // Reached max restart attempts, disable the extension.
      ext.action = ExtAction::ERROR_DISABLE;
      return;
    }
  }
}

void ExtensionModule::errorRestartState(ExtContainer &ext) {
  auto status = ext.extension->getStatus();
  if (status == node::Status::ERROR) {
    // if critical, always attempt to restart
    if (ext.restart_attempts < max_restart_attempts_ || ext.metadata.critical) {
      /// @todo handle the fault code
      /// @todo add delay

      ext.restart_attempts++;
      ext.extension->reset();
      ext.action = ExtAction::RESTART;
      return;
    } else {
      // Reached max restart attempts, disable the extension.
      ext.action = ExtAction::ERROR_DISABLE;
      return;
    }
  }
  error(DiagnosticId::EXTENSION_MODULE_internalError,
        "invalid error restart state: " + ext.metadata.name +
            " status: " + std::to_string(static_cast<uint16_t>(status)));
}

void ExtensionModule::errorDisableState(ExtContainer &ext) {
  if (!ext.metadata.enabled) {
    // Already disabled, nothing to do.
    return;
  }

  auto status = ext.extension->getStatus();
  if (status == node::Status::ERROR) {
    error(DiagnosticId::EXTENSION_MODULE_extensionErrorDisabled,
          "Extension: " + ext.metadata.name + " - " +
              std::to_string(static_cast<uint16_t>(status)));

    ext.extension->stop();
    ext.metadata.enabled = false;
    return;
  }
}

} // namespace modules