#include "console_pages.hpp"
#include <BoosterSeat/string_formatting.hpp>
#include <BoosterSeat/time.hpp>
#include <functional>

inline std::string b2str(bool val) {
  return val ? "true" : "false";
}
inline std::string title_and_data(std::string title, std::string data) {
  return title + ": " + data;
}

std::array<std::string, console_pages::kNumPageLines>
console_pages::Pages::getCurrentPage() {

  content_ = {}; // Clear the content (set all lines to "")

  switch (current_page_) {
  case PageOption::GFS_STATUS:
    gfsStatus();
    break;
  case PageOption::DATA:
    data();
    break;
  case PageOption::LOG:
    log();
    break;
  case PageOption::SERVER:
    server();
    break;
  case PageOption::CONSOLE:
    console();
    break;
  default:
    break;
  }
  return content_;
}

console_pages::Menu console_pages::Pages::getCurrentMenu() {
  return current_menu_;
}

void console_pages::Pages::navigateMenu(const console_pages::PageOption key) {
  const int kWindowHeight = 8;
  for (int i = 0; i < kWindowHeight; ++i) {
    content_[i] = "";
  }
  current_page_ = key;
}

std::string LandR(const std::string &l, const std::string &r) {
  constexpr int kWidth = 19;
  std::string ret = "";
  ret += BoosterSeat::layout::fixedWidthString(
      l, BoosterSeat::layout::Alignment::LEFT, kWidth);
  ret += BoosterSeat::layout::fixedWidthString(
      r, BoosterSeat::layout::Alignment::RIGHT, kWidth);
  return ret;
}
void console_pages::Pages::gfsStatus() {
  data::blocks::ModulesStatuses mod_stats =
      shared_data_.blocks.modules_statuses.get();
  data::blocks::SystemInfo sys_info = shared_data_.blocks.system_info.get();

  content_[0] = LandR(BoosterSeat::time::utcTimeString() + " UTC ",
                      BoosterSeat::time::localTimeString() + " Local");
  content_[1] =
      LandR("Uptime: " + shared_data_.misc.getUptimeString(),
            "Ld Avg: " + BoosterSeat::string::f2s(sys_info.cpu_load_avg_1, 1) +
                " " + BoosterSeat::string::f2s(sys_info.cpu_load_avg_5, 1) +
                " " + BoosterSeat::string::f2s(sys_info.cpu_load_avg_15, 1));

  content_[2] = "";

  content_[3] = "Modules";
  content_[4] = "Data:    " + node::status_to_string.at(mod_stats.data);
  content_[5] = "Server:  " + node::status_to_string.at(mod_stats.server);
  content_[6] = "Console: " + node::status_to_string.at(mod_stats.console);
  content_[7] = "System:  " + node::status_to_string.at(mod_stats.system);
}

void console_pages::Pages::data() {
  data::blocks::StreamsStats stats = shared_data_.blocks.stream_stats.get();

  auto stream_stat = [](const std::string &name,
                        const data::blocks::StreamsStats::StreamStats &stat) {
    return name + ": " + std::to_string(stat.current_packets) + "/" +
           std::to_string(stat.total_packets) + " : " +
           std::to_string(stat.processing_delay_ms);
  };

  content_ = {
      "Streams (current/total : delay ms)", // -- streams --
      stream_stat("Data", stats.data),      // data stream
      stream_stat("Log", stats.log),        // log stream

      "", // empty line

      " -- Data Logging Config -- ", // -- data logging config --
      "Enabled: " +
          b2str(config_.data_module_data.getLogDataToFile()), // enabled
      "Strategy: " +
          std::string(cfg::gEnum::LogStrategyToKey(
              config_.data_module_data.getLogStrategy())), // log strategy
      "Detail: " +
          std::string(cfg::gEnum::LogDetailToKey(
              config_.data_module_data.getLogDetail())), // detail level
      "Interval (ms): " +
          std::to_string(
              config_.data_module_data.getLogIntervalMs()), // log interval
      "Max File Size (MB): " +
          std::to_string(config_.data_module_data
                             .getMaxDataLogFileSizeMb()), // max file size
      "Max Archive Size (MB): " +
          std::to_string(config_.data_module_data
                             .getMaxDataArchiveSizeMb()), // max archive size
      "Archive Method: " +
          std::string(cfg::gEnum::ArchiveMethodToKey(
              config_.data_module_data.getArchiveMethod())) // archive method
  };
}

void console_pages::Pages::log() {
  content_[5] = "log";
}

void console_pages::Pages::server() {
  content_[0] = "Enabled: " + b2str(config_.server_module.getEnabled());
  content_[1] = "Socket Port: " +
                std::to_string(config_.server_module.getTcpSocketPort());
  content_[5] = "server";
}

void console_pages::Pages::console() {
  content_[5] = "console";
}

// const console_pages::ConsolePage &console_pages::Pages::gfsStatus() {

// }

// ConsolePage gfs_status_ = {
//   "", // time utc/local
//   "", // uptime/cpu ldv
//   "", // memory
//   "-- Streams -- (current/total)",
//   "", // data
//   "", // log
//   "", // empty line
//   "-- Modules --",
//   "", // data
//   "", // server
//   "", // console
//   "", // system
// };