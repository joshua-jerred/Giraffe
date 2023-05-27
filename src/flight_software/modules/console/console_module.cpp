#include "console_module.h"
#include <functional>
#include <iostream>

#include "time_types.hpp"
#include "units.hpp"

static modules::MetaData metadata("console_module",
                                  node::Identification::CONSOLE_MODULE);

modules::ConsoleModule::ConsoleModule(data::SharedData &shared_data,
                                      cfg::Configuration &config)
    : modules::Module(metadata, shared_data, config) {
  metadata_.sleep_interval_ =
      100; // for key board processing, not the actual update rate
}

modules::ConsoleModule::~ConsoleModule() {}

std::vector<std::string> doAThing1() {
  return std::vector<std::string>({"not empty"});
}

std::vector<std::string> doAThing2() {
  return std::vector<std::string>({"not empty 2"});
}

void modules::ConsoleModule::startup() {
  ncurs::MenuOption status = ncurs::MenuOption(
      "GFS Status", {}, std::bind(&modules::ConsoleModule::status, this));

  // ncurs::MenuOption server_cfg = ncurs::MenuOption(
  //     "Server Module", {},
  //     std::bind(&modules::ConsoleModule::serverModuleConfig, this));

  ncurs::Menu main_menu = {status, {"Data", {status, {"data2"}, {"data3"}}}};

  int update_interval = configuration_.console_module.getUpdateInterval();
  ncurs_env_.initialize(main_menu, update_interval);
}

void modules::ConsoleModule::loop() { ncurs_env_.update(); }

void modules::ConsoleModule::shutdown() { ncurs_env_.end(); }

void modules::ConsoleModule::processCommand(const command::Command &command) {
  (void)command;
}

inline std::string b2str(bool val) { return val ? "true" : "false"; }
inline std::string title_and_data(std::string title, std::string data) {
  return title + ": " + data;
}
inline std::string row_align(const std::string &left, const std::string &center_char,
                             const std::string &right) {
                              
                             }

std::vector<std::string> modules::ConsoleModule::status() {
  data::blocks::StreamsStats stats = shared_data_.blocks.stream_stats.get();
  data::blocks::ModulesStatuses mod_stats =
      shared_data_.blocks.modules_statuses.get();

  std::string header = giraffe_time::utcTimeString() + " UTC | " +
                       giraffe_time::localTimeString() + " Local";

  // lambda formatter
  auto fmt_stream_stats =
      [](std::string title,
         data::blocks::StreamsStats::StreamStats dat) noexcept {
        return title + ": " + std::to_string(dat.current_packets) + "/" +
               std::to_string(dat.total_packets) +
               " | p_delay: " + std::to_string(dat.processing_delay_ms);
      };

  auto fmt_mod = [](std::string title, node::Status status) noexcept {
    return title + ": " + node::status_to_string.at(status);
  };

  return std::vector<std::string>(
      {header, "-- Streams -- (current/total)", "", // empty line
       fmt_stream_stats("Data", stats.data), fmt_stream_stats("Log", stats.log),
       "", // empty line
       "-- Modules --", fmt_mod("Data", mod_stats.data),
       fmt_mod("Server", mod_stats.server),
       fmt_mod("Console", mod_stats.console),
       fmt_mod("System", mod_stats.system)});
};

// std::vector<std::string> modules::ConsoleModule::configSection() {
//   switch (section) {
//     default:
//       return std::vector<std::string>({"not implemented"});
//   }
// }
