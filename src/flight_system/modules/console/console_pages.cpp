#include "console_pages.hpp"
#include <BoosterSeat/numbers.hpp>
#include <BoosterSeat/string_formatting.hpp>
#include <BoosterSeat/time.hpp>
#include <functional>

inline auto rnd = BoosterSeat::doubleToPrecisionTwo;
namespace bst = BoosterSeat::time;

inline std::string b2str(bool val) {
  return val ? "true" : "false";
}
inline std::string title_and_data(std::string title, std::string data) {
  return title + ": " + data;
}

const std::array<std::string, console_pages::kMaxNumPageLines> &
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
  case PageOption::EXTENSIONS:
    extensions();
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

/**
 * @brief Helper function to format two strings to be left and right aligned
 * within the console window.
 *
 * @param l - left string
 * @param r - right string
 * @return std::string - formatted string
 */
std::string LandR(const std::string &l, const std::string &r) {
  constexpr int kWidth = console_pages::kDataWindowWidth / 2 - 1;
  std::string ret = "";
  ret += BoosterSeat::layout::fixedWidthString(
      l, BoosterSeat::layout::Alignment::LEFT, kWidth);
  ret += BoosterSeat::layout::fixedWidthString(
      r, BoosterSeat::layout::Alignment::RIGHT, kWidth);
  return ret;
}

void console_pages::Pages::gfsStatus() {
  current_num_lines_ = 9;

  data::blocks::ModulesStatuses mod_stats =
      shared_data_.blocks.modules_statuses.get();
  data::blocks::SystemInfo sys_info = shared_data_.blocks.system_info.get();

  content_[0] = LandR(bst::timeString(bst::TimeZone::UTC) + " UTC ",
                      bst::timeString(bst::TimeZone::LOCAL) + " Local");
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
  content_[8] = "Extension: " + node::status_to_string.at(mod_stats.extension);
}

void console_pages::Pages::data() {
  constexpr int kNumLines = 19;
  setNumLinesOnPage(kNumLines);

  data::blocks::StreamsStats stats = shared_data_.blocks.stream_stats.get();
  data::blocks::DataLogStats log_stats =
      shared_data_.blocks.data_log_stats.get();

  auto stream_stat = [](const std::string &name,
                        const data::blocks::StreamsStats::StreamStats &stat) {
    return name + ": " + std::to_string(stat.current_packets) + "/" +
           std::to_string(stat.total_packets) + " : " +
           std::to_string(stat.processing_delay_ms);
  };

  content_ = {
      "Streams (current/total : delay ms)",  // -- streams --
      LandR(stream_stat("Data", stats.data), // data stream
            stream_stat("Log", stats.log)),  // log stream
      LandR(stream_stat("GPS", stats.gps),   // gps stream
            stream_stat("IMU", stats.imu)),  // imu stream

      "",
      " -- Data/Log Files Info -- ",
      "Data Dir/File Valid: " + b2str(log_stats.data_dir) + " / " +
          b2str(log_stats.data_file),
      "Log Dir/File Valid: " + b2str(log_stats.log_dir) + " / " +
          b2str(log_stats.log_file),
      "Data File Path: " + log_stats.data_file_path,
      "Log File Path: " + log_stats.log_file_path,
      "Data File Size: " + std::to_string(log_stats.data_file_size) + " MB",
      "Log File Size: " + std::to_string(log_stats.log_file_size) + " MB",
      "Data/Log Archive Dirs Valid: " + b2str(log_stats.data_archive_dir) +
          " / " + b2str(log_stats.log_archive_dir),
      "Data/Log Archive Size: " +
          std::to_string(log_stats.data_archive_dir_size) + " MB / " +
          std::to_string(log_stats.log_archive_dir_size) + " MB",

      "Data/Log Archive Dirs Valid: " + b2str(log_stats.data_archive_dir) +
          " / " + b2str(log_stats.log_archive_dir),
      ""

      "",
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
      // "Max Archive Size (MB): " +
      //     std::to_string(config_.data_module_data
      //                        .getMaxDataArchiveSizeMb()), // max archive size
      // "Archive Method: " +
      //     std::string(cfg::gEnum::ArchiveMethodToKey(
      //         config_.data_module_data.getArchiveMethod())) // archive method
  };
}

void console_pages::Pages::log() {
  current_num_lines_ = 6;
  content_[5] = "log";
}

void console_pages::Pages::server() {
  current_num_lines_ = 6;
  content_[0] = "Enabled: " + b2str(config_.server_module.getEnabled());
  content_[1] = "Socket Port: " +
                std::to_string(config_.server_module.getTcpSocketPort());
  content_[5] = "server";
}

void console_pages::Pages::extensions() {
  current_num_lines_ = 6;
  content_[5] = "extensions";
}

void console_pages::Pages::setNumLinesOnPage(const int num_lines) {
  if (num_lines > console_pages::kMaxNumPageLines) {
    // This should never happen as it's an array.
    // Pick the value properly.
    throw std::runtime_error("num_lines > kMaxNumLines");
  }

  current_num_lines_ = num_lines;
}