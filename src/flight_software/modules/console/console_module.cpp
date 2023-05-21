#include "console_module.h"

#include <iostream>
#include <functional>

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
  ncurs::SubMenu config_menu = {"cfg_menu", {{"cfg Option 1"}, {"cfg Option 2"}}};
  ncurs::SubMenu data_menu = {"data_menu", {{"data Option 1"}, {"data Option 2"}}};

  ncurs::MainMenu main_menu = {config_menu, data_menu};
  ncurs_env_.init(main_menu);
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