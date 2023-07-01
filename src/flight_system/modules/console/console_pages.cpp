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
std::string f2s(double val) {
  return BoosterSeat::string::f2s(val, 2);
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
  case PageOption::EXTENSION_DATA:
    extensionData();
    break;
  case PageOption::SYSTEM:
    system();
    break;
  case PageOption::LOCATION:
    location();
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
  constexpr int kNumLines = 15;
  setNumLinesOnPage(kNumLines);

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

  content_[9] = "";

  auto &ef = shared_data_.frames.error_frame;
  content_[10] =
      "Error Frame | Active: " + std::to_string(ef.numActiveErrors()) +
      " | Total: " + std::to_string(ef.numTotalErrors());

  int i = 11;
  auto error_ids = ef.getActiveErrorIds();
  std::string line_buffer = "";
  for (auto &id : error_ids) {
    line_buffer +=
        BoosterSeat::string::intToHex(static_cast<uint16_t>(id)) + "  ";
    if (line_buffer.size() > kDataWindowWidth - 2) {
      content_[i++] = line_buffer;
      line_buffer = "";
    }
    if (i >= current_num_lines_ - 1) {
      break;
    }
  }
  if (line_buffer.size() > 0) {
    content_[i++] = line_buffer;
  }
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
  constexpr int kNumLines = 10;
  setNumLinesOnPage(kNumLines);
  auto log = shared_data_.log_container.get();
  int num_lines = log.size();

  int i = 0;
  while (i < num_lines && i < kNumLines) {
    content_[i] = log[i];
    ++i;
  }
}

void console_pages::Pages::server() {
  constexpr int kNumLines = 9;
  setNumLinesOnPage(kNumLines);
  auto stats = shared_data_.blocks.server_module_stats.get();
  // clang-format off
  content_ = {
    "Enabled: " + b2str(config_.server_module.getEnabled()),
    "Socket Port: " + std::to_string(config_.server_module.getTcpSocketPort()),
    "",
    " -- Socket Stats -- ",
    "Connected: " + b2str(stats.is_connected),
    "Bps Up: " + f2s(stats.bytes_per_second_up),
    "Bps Down: " + f2s(stats.bytes_per_second_down),
    "Messages Received: " + std::to_string(stats.num_messages_received),
    "Invalid Received: " + std::to_string(stats.num_invalid_received)
  };
  // clang-format on
}

void console_pages::Pages::extensions() {
  constexpr int kNumLines = 6;
  setNumLinesOnPage(kNumLines);
  auto stats = shared_data_.blocks.extension_module_stats.get();
  // clang-format off
  content_ = {
    "Num Configured: " + std::to_string(stats.num_configured),
    "Num Active: " + std::to_string(stats.num_active),
    "Num Inactive: " + std::to_string(stats.num_inactive)
  };
  // clang-format on
}

void console_pages::Pages::extensionData() {
  constexpr int kNumLines = 6;
  setNumLinesOnPage(kNumLines);
  auto &frames = shared_data_.frames;

  auto temp = frames.env_temp.getAll();
  auto hum = frames.env_hum.getAll();
  auto pres = frames.env_pres.getAll();

  std::string temp_str = "";
  std::string hum_str = "";
  std::string pres_str = "";

  for (auto &item : temp) {
    temp_str += item.first + ":" + item.second.value + ", ";
  }
  for (auto &item : hum) {
    hum_str += item.first + ":" + item.second.value + ", ";
  }
  for (auto &item : pres) {
    pres_str += item.first + ":" + item.second.value + ", ";
  }

  // clang-format off
  content_ = {
    "Frame Sizes (temp, hum, pres): " + std::to_string(frames.env_temp.size())
      + ", " + std::to_string(frames.env_hum.size()) + ", "
      + std::to_string(frames.env_pres.size()),
    "Temp: " + temp_str,
    "Hum: " + hum_str,
    "Pres: " + pres_str
  };
  // clang-format on
}

void console_pages::Pages::system() {
  constexpr int kNumLines = 7;
  setNumLinesOnPage(kNumLines);

  auto sys = shared_data_.blocks.system_info.get();
  // clang-format off
  content_ = {
    "CPU Load Avg: " + BoosterSeat::string::f2s(sys.cpu_load_avg_1, 1) +
      " " + BoosterSeat::string::f2s(sys.cpu_load_avg_5, 1) +
      " " + BoosterSeat::string::f2s(sys.cpu_load_avg_15, 1),
    "CPU Temp (c): " + f2s(sys.cpu_temp_c),
    "",
    "-- Free GB | Total GB | (usage percent) --",
    "RAM: " + f2s(sys.mem_free_gb) + " | " + 
      f2s(sys.mem_total_gb) + " | (" + f2s(sys.mem_used_percent) + "%)",
    "SWAP: " + f2s(sys.swap_free_gb) + " | " + 
      f2s(sys.swap_total_gb) + " | (" + f2s(sys.swap_used_percent) + "%)",
    "DISK: " + f2s(sys.disk_free_gb) + " | " + 
      f2s(sys.disk_total_gb) + " | (" + f2s(sys.disk_used_percent) + "%)"
  };
  // clang-format on
}

void console_pages::Pages::location() {
  constexpr int kNumLines = 5;
  setNumLinesOnPage(kNumLines);

  auto data = shared_data_.blocks.location_data.get();
  // auto last_valid = data.last_valid_gps_frame;
  auto cur = data.last_gps_frame;
  // clang-format off
  content_ = {
    " -- Most Recent GPS Frame -- ",
    "Fix, Num Sats, UTC:   " + data::GpsFixStringMap.at(cur.fix) + ", " +
      std::to_string(cur.num_satellites) + ", utc time",
    "Lat, Lon, H/S:   " + BoosterSeat::string::f2s(cur.latitude, 6) + ", " 
      + BoosterSeat::string::f2s(cur.longitude, 6) + ", " +
      BoosterSeat::string::f2s(cur.horizontal_speed, 1) + " m/s",
    "ALT, V/S, HDG:   " + BoosterSeat::string::f2s(cur.altitude, 1) 
      + " m" + ", " + BoosterSeat::string::f2s(cur.heading_of_motion, 1) + 
      " deg",
    "Accuracy - Hor, Vert, H/S: " + 
      BoosterSeat::string::f2s(cur.horz_accuracy, 1) + " m, " +
      BoosterSeat::string::f2s(cur.vert_accuracy, 1) + " m, " +
      BoosterSeat::string::f2s(cur.horizontal_speed, 1) + " m/s"
  };
  // clang-format on
}

void console_pages::Pages::setNumLinesOnPage(const int num_lines) {
  if (num_lines > console_pages::kMaxNumPageLines) {
    // This should never happen as it's an array.
    // Pick the value properly.
    throw std::runtime_error("num_lines > kMaxNumLines");
  }

  current_num_lines_ = num_lines;
}