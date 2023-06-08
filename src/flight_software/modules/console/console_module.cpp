#include "console_module.h"
#include <functional>

#include "time_types.hpp"
#include "units.hpp"

/**
 * @brief The update interval is manually set to 50ms in this module.
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
  ncurs_env_.start(metadata_.sleep_interval_);
}

void modules::ConsoleModule::loop() {
  ncurs_env_.update();
}

void modules::ConsoleModule::shutdown() {
  ncurs_env_.end();
}

void modules::ConsoleModule::processCommand(const command::Command &command) {
  (void)command;
}