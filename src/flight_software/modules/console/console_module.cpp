#include "console_module.h"

#include <functional>
#include <iostream>

#include "configuration_internal.h"

static modules::MetaData metadata("console_module",
                                  node::Identification::CONSOLE_MODULE);

modules::ConsoleModule::ConsoleModule(data::Streams &streams,
                                      cfg::Configuration &config)
    : modules::Module(metadata, streams, config) {
  metadata_.sleep_interval_ =
      100;  // for key processing, not the actual update rate
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
  ncurs::MenuOption status = ncurs::MenuOption(
      "Status", {}, std::bind(&modules::ConsoleModule::status, this));

  ncurs::MenuOption gen_cfg = ncurs::MenuOption(
      "General", {}, std::bind(&modules::ConsoleModule::generalConfig, this));

  ncurs::MenuOption debug_cfg = ncurs::MenuOption(
      "Debug", {}, std::bind(&modules::ConsoleModule::debugConfig, this));

  ncurs::MenuOption server_cfg = ncurs::MenuOption(
      "Server", {}, std::bind(&modules::ConsoleModule::serverConfig, this));

  ncurs::Menu main_menu = {status,
                           {"Configuration", {gen_cfg, debug_cfg, server_cfg}},
                           {"Data", {{"data1"}, {"data2"}, {"data3"}}}};

  cfg::Debug debug_cfg_ = configuration_.getDebug();
  ncurs_env_.initialize(main_menu, debug_cfg_.console_update_interval);
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

inline std::string b2str(bool val) {
  return val ? "true" : "false";
}

std::vector<std::string> modules::ConsoleModule::status() {
  int current_data_packets = streams_.data_stream.getNumPackets();
  int total_data_packets = streams_.data_stream.getTotalPackets();

  int current_error_packets = streams_.error_stream.getNumPackets();
  int total_error_packets = streams_.error_stream.getTotalPackets();

  return std::vector<std::string>({
      "-- Streams -- (current/total)",
      "Data: " + std::to_string(current_data_packets) + "/" +
          std::to_string(total_data_packets),
      "Error: " + std::to_string(current_error_packets) + "/" +
          std::to_string(total_error_packets),
  });
};

std::vector<std::string> modules::ConsoleModule::generalConfig() {
  cfg::General general = configuration_.getGeneral();
  return std::vector<std::string>(
      {"Project Name: " + general.project_name,
       "Main Board Type: " +
           cfg::kMainBoardToString.at(general.main_board_type),
       "Starting Procedure: " +
           cfg::kProcedureTypeToString.at(general.starting_procedure)});
}

std::vector<std::string> modules::ConsoleModule::debugConfig() {
  cfg::Debug debug = configuration_.getDebug();
  return std::vector<std::string>(
      {"Console Enabled: " + b2str(debug.console_enabled),
       "Console Update Interval: " +
           std::to_string(debug.console_update_interval),
       "Print Errors: " + b2str(debug.print_errors)});
}

std::vector<std::string> modules::ConsoleModule::serverConfig() {
  cfg::Server server = configuration_.getServer();
  return std::vector<std::string>(
      {"TCP Socket Port: " + std::to_string(server.tcp_socket_port)});
}