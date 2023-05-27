#include "console_module.h"
#include <functional>

#include "time_types.hpp"
#include "units.hpp"

static modules::MetaData metadata("console_module",
                                  node::Identification::CONSOLE_MODULE);

modules::ConsoleModule::ConsoleModule(data::SharedData &shared_data,
                                      cfg::Configuration &config)
    : modules::Module(metadata, shared_data, config), pages_(shared_data), ncurs_env_(pages_) {
  metadata_.sleep_interval_ =
      100; // for key board processing, not the actual update rate
}

modules::ConsoleModule::~ConsoleModule() {}

void modules::ConsoleModule::startup() { ncurs_env_.start(metadata_.sleep_interval_); }

void modules::ConsoleModule::loop() { ncurs_env_.update(); }

void modules::ConsoleModule::shutdown() { ncurs_env_.end(); }

void modules::ConsoleModule::processCommand(const command::Command &command) {
  (void)command;
}