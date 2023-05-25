#include "console_module.h"

#include <functional>
#include <iostream>

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


  // ncurs::MenuOption server_cfg = ncurs::MenuOption(
  //     "Server Module", {},
  //     std::bind(&modules::ConsoleModule::serverModuleConfig, this));

  ncurs::Menu main_menu = {status,
                           //{"Configuration", config_menu},
                           {"Data", {{"data1"}, {"data2"}, {"data3"}}}};

  // cfg::ConsoleModule cns_cfg = configuration_.getConsoleModule();
  // ncurs_env_.initialize(main_menu, cns_cfg.update_interval);
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

// std::vector<std::string> modules::ConsoleModule::configSection() {
//   switch (section) {
//     default:
//       return std::vector<std::string>({"not implemented"});
//   }
// }
