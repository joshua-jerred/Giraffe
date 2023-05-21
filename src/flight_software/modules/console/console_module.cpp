#include "console_module.h"

#include <functional>
#include <iostream>

static modules::MetaData metadata("console_module",
                                  node::Identification::CONSOLE_MODULE);

modules::ConsoleModule::ConsoleModule(data::Streams &streams,
                                      cfg::Configuration &config)
    : modules::Module(metadata, streams, config) {
  cfg::Debug debug_cfg_ = configuration_.getDebug();
  metadata_.sleep_interval = debug_cfg_.console_update_interval;
}

modules::ConsoleModule::~ConsoleModule() {
}

void doAThing1() {
  std::cout << "Doing a thing 1!!!" << std::endl;
}

void doAThing2() {
  std::cout << "Doing a thing 2!!!" << std::endl;
}

void modules::ConsoleModule::startup() {
  // ncurs::Menu config_menu = {"Configuration", {{"cfg Option 1"}, {"cfg Option
  // 2"}}}; ncurs::Menu data_menu = {"Streams", {{"data Option 1"}, {"data
  // Option 2"}}};

  ncurs::Menu main_menu = {
    {"Configuration", {
      {"cfg1"},
      {"cfg2"},
      {"cfg3"}
      }}, 
    {"Data", {
      {"data1"},
      {"data2"},
      {"data3"}
    }}
  };
  ncurs_env_.initialize(main_menu);
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