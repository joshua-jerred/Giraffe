#include <functional>

#include "console_module.hpp"

/**
 * @details The update interval is manually set to 50ms in this module.
 * This is for keyboard input processing. In the configuration there
 * is still a sleep interval that is used for actually updating the
 * data.
 */
inline constexpr int kConsoleUpdateRate = 50;

static modules::MetaData metadata("console_module",
                                  node::Identification::CONSOLE_MODULE,
                                  kConsoleUpdateRate);

modules::ConsoleModule::ConsoleModule(data::SharedData &shared_data,
                                      cfg::Configuration &config)
    : modules::Module(metadata, shared_data, config),
      pages_(config, shared_data), ncurs_env_(pages_) {
}

modules::ConsoleModule::~ConsoleModule() {
}

void modules::ConsoleModule::startup() {
  try {
    ncurs_env_.start(metadata_.sleep_interval_);
  } catch (const std::exception &e) {
    error(DiagnosticId::CONSOLE_MODULE_failedToStart,
          "exception: " + std::string(e.what()));
    return;
  }

  started_ = true;
}

void modules::ConsoleModule::loop() {
  if (!started_) {
    return; // Do not update if the module failed to start
  }
  ncurs_env_.update();
}

void modules::ConsoleModule::shutdown() {
  if (!started_) {
    return; // Do not update if the module failed to start
  }
  ncurs_env_.end();
  started_ = false;
}

void modules::ConsoleModule::processCommand(const cmd::Command &command) {
  (void)command;
}