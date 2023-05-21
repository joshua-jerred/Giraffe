#include "console_module.h"
#include "configuration_internal.h"

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

std::vector<std::string> doAThing1() {
  return std::vector<std::string>({"not empty"});
}

std::vector<std::string> doAThing2() {
  return std::vector<std::string>({"not empty 2"});
}

void modules::ConsoleModule::startup() {
  ncurs::MenuOption gen_cfg = ncurs::MenuOption("General");
  gen_cfg.console_data_ = std::bind(&modules::ConsoleModule::generalConfig, this);

  ncurs::Menu main_menu = {{"Configuration", {gen_cfg, {"cfg2"}, {"cfg3"}}},
                           {"Data", {{"data1"}, {"data2"}, {"data3"}}}};
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

inline std::string boolToString(bool val) {
  return val ? "true" : "false";
}

std::vector<std::string> modules::ConsoleModule::generalConfig() {
  cfg::General general = configuration_.getGeneral();

  return std::vector<std::string>({
    "Project Name: " + general.project_name,
    "Main Board Type: " + cfg::kMainBoardToString.at(general.main_board_type),
    "Starting Procedure: " + cfg::kProcedureTypeToString.at(general.starting_procedure)
    });
}
