#include "console_module.h"

#include <functional>
#include <iostream>

#include "configuration_internal.h"

static modules::MetaData metadata("console_module",
                                  node::Identification::CONSOLE_MODULE);

modules::ConsoleModule::ConsoleModule(data::SharedData &shared_data,
                                      cfg::Configuration &config)
    : modules::Module(metadata, shared_data, config) {
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

  std::vector<ncurs::MenuOption> config_menu = {

      ncurs::MenuOption("General", {},
                        std::bind(&modules::ConsoleModule::configSection, this,
                                  cfg::Section::general)),
      ncurs::MenuOption("Data Module General", {},
                        std::bind(&modules::ConsoleModule::configSection, this,
                                  cfg::Section::data_module_general)),
      ncurs::MenuOption("Data Module Data Log", {},
                        std::bind(&modules::ConsoleModule::configSection, this,
                                  cfg::Section::data_module_data_log)),
      ncurs::MenuOption("Data Module InfluxDB", {},
                        std::bind(&modules::ConsoleModule::configSection, this,
                                  cfg::Section::data_module_influxdb)),
      ncurs::MenuOption("Data Module Error Logs", {},
                        std::bind(&modules::ConsoleModule::configSection, this,
                                  cfg::Section::data_module_error_log)),
      ncurs::MenuOption("Data Module Debug", {},
                        std::bind(&modules::ConsoleModule::configSection, this,
                                  cfg::Section::data_module_debug)),
      ncurs::MenuOption("Console Module", {},
                        std::bind(&modules::ConsoleModule::configSection, this,
                                  cfg::Section::console_module)),
      ncurs::MenuOption("Server Module", {},
                        std::bind(&modules::ConsoleModule::configSection, this,
                                  cfg::Section::server_module)),
      ncurs::MenuOption("System Module", {},
                        std::bind(&modules::ConsoleModule::configSection, this,
                                  cfg::Section::system_module)),
      ncurs::MenuOption("Telemetry General", {},
                        std::bind(&modules::ConsoleModule::configSection, this,
                                  cfg::Section::telemetry)),
      ncurs::MenuOption("Telemetry APRS", {},
                        std::bind(&modules::ConsoleModule::configSection, this,
                                  cfg::Section::telemetry_aprs)),
      ncurs::MenuOption("Telemetry SSTV", {},
                        std::bind(&modules::ConsoleModule::configSection, this,
                                  cfg::Section::telemetry_sstv)),
      ncurs::MenuOption("Telemetry Data Packets", {},
                        std::bind(&modules::ConsoleModule::configSection, this,
                                  cfg::Section::telemetry_data_packets)),
  };

  // ncurs::MenuOption server_cfg = ncurs::MenuOption(
  //     "Server Module", {},
  //     std::bind(&modules::ConsoleModule::serverModuleConfig, this));

  ncurs::Menu main_menu = {status,
                           {"Configuration", config_menu},
                           {"Data", {{"data1"}, {"data2"}, {"data3"}}}};

  cfg::ConsoleModule cns_cfg = configuration_.getConsoleModule();
  ncurs_env_.initialize(main_menu, cns_cfg.update_interval);
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
  data::blocks::StreamsStats stats = shared_data_.blocks.stream_stats.get();

  // lamba formatter
  auto fmt = [](std::string title, data::blocks::StreamsStats::StreamStats dat) noexcept {
    return title + ": " + std::to_string(dat.current) + "/" +
           std::to_string(dat.total) + " | p_delay: " + std::to_string(dat.processing_delay_ms);
  };

  return std::vector<std::string>({
      "-- Streams -- (current/total)",
      fmt("Data", stats.data),
      fmt("Error", stats.error),
      fmt("Status", stats.status),
      fmt("SysInfo", stats.system_info)
  });
};

std::vector<std::string> modules::ConsoleModule::configSection(
    cfg::Section section) {
  static cfg::General general_cfg;
  static cfg::DataModuleGeneral dm_gen_cfg;
  static cfg::DataModuleDataLog dm_dl_cfg;
  static cfg::DataModuleInfluxDb dm_inflx_cfg;
  static cfg::DataModuleErrorLog dm_el_cfg;
  static cfg::DataModuleDebug dm_db_cfg;
  static cfg::ServerModule server_module_cfg;
  static cfg::SystemModule system_module_cfg;
  static cfg::Telemetry telemetry_cfg;
  static cfg::Aprs aprs_cfg;
  static cfg::Sstv sstv_cfg;
  static cfg::DataPackets data_packets_cfg;

  switch (section) {
    case cfg::Section::general:
      general_cfg = configuration_.getGeneral();
      return std::vector<std::string>(
          {"Project Name: " + general_cfg.project_name,
           "Main Board Type: " +
               cfg::kMainBoardToString.at(general_cfg.main_board_type),
           "Starting Procedure: " +
               cfg::kProcedureTypeToString.at(general_cfg.starting_procedure)});
    case cfg::Section::data_module_general:
      dm_gen_cfg = configuration_.getDataModuleGeneral();
      return std::vector<std::string>(
          {"Frame Purge Time: " + std::to_string(dm_gen_cfg.frame_purge_time)});
    case cfg::Section::data_module_data_log:
      dm_dl_cfg = configuration_.getDataModuleDataLog();
      return std::vector<std::string>(
          {"Log Data To File: " + b2str(dm_dl_cfg.log_data_to_file),
           "Log Strategy: " +
               cfg::kLogStrategyToString.at(dm_dl_cfg.log_strategy),
           "Log Detail: " + cfg::kLogDetailToString.at(dm_dl_cfg.log_detail),
           "Log Interval (ms): " + std::to_string(dm_dl_cfg.log_interval_ms),
           "Max Log File Size (MB): " +
               std::to_string(dm_dl_cfg.max_data_log_file_size_mb),
           "Max Archive Size (MB): " +
               std::to_string(dm_dl_cfg.max_data_archive_size_mb),
           "Archive Method: " +
               cfg::kArchivalMethodToString.at(dm_dl_cfg.archival_method)});
    default:
      return std::vector<std::string>({"not implemented"});
  }
}
