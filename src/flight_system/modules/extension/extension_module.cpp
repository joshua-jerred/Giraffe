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
#include "simulated_extensions.hpp"
// --------------------

static const std::vector<cfg::ExtensionMetadata> SIMULATED_EXTENSIONS = {
    cfg::ExtensionMetadata{
        "sim_temp", true, cfg::gEnum::ExtensionType::SIM_TEMP, 1000, false, ""},
    cfg::ExtensionMetadata{
        "sim_pres", true, cfg::gEnum::ExtensionType::SIM_PRES, 1000, false, ""},
    cfg::ExtensionMetadata{"sim_hum", true, cfg::gEnum::ExtensionType::SIM_HUM,
                           1000, false, ""},
    cfg::ExtensionMetadata{"sim_gps", true, cfg::gEnum::ExtensionType::SIM_GPS,
                           1000, false, ""},
    cfg::ExtensionMetadata{"sim_imu", true, cfg::gEnum::ExtensionType::SIM_IMU,
                           1000, false, ""},
};

namespace modules {

static MetaData metadata("extension_module",
                         node::Identification::EXTENSION_MODULE, 100);

ExtensionModule::ExtensionModule(data::SharedData &shared_data,
                                 cfg::Configuration &config)
    : modules::Module(metadata, shared_data, config),
      extension_resources_(shared_data.streams,
                           config.hardware_interface.getI2CBus(),
                           shared_data.status_led) {
}

void ExtensionModule::startup() {
  updateLocalConfig(); // Load in the config for the extension.

// Load in the simulated extensions.
#ifdef SIMULATED_EXTENSIONS
  for (auto &ext_meta : SIMULATED_EXTENSIONS) {
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
    // Create the extension.
    auto extension = createExtension(ext_meta);
    if (!extension.has_value()) {
      error(DiagnosticId::EXTENSION_MODULE_failedToCreate, ext_meta.name);
      continue;
    }
    info("Loaded extension: " + ext_meta.name);
    // Add the extension to the list of extensions.
    extensions_.push_back(extension.value());
  }
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
  (void)command;
}

void ExtensionModule::updateLocalConfig() {
  // Load in the config for the extensions.
  // extension_metadata_ = configuration_.extensions
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
    giraffe_assert(false); // Shouldn't get here
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
    break;
  default:
    giraffe_assert(false);
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
  (void)ext;
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
  (void)ext;
  giraffe_assert(false); // not implemented yet
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
  (void)ext;
  giraffe_assert(false); // not implemented yet
}

void ExtensionModule::errorDisableState(ExtContainer &ext) {
  (void)ext;
  giraffe_assert(false); // not implemented yet
}

} // namespace modules