#include "console_pages.hpp"

#include <BoosterSeat/string_formatting.hpp>
#include <BoosterSeat/time.hpp>

inline std::string b2str(bool val) { return val ? "true" : "false"; }
inline std::string title_and_data(std::string title, std::string data) {
  return title + ": " + data;
}

std::array<std::string, console_pages::kNumPageLines>
console_pages::Pages::getCurrentPage() {
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

void console_pages::Pages::navigateMenu(console_pages::PageOption key) {
  current_page_ = key;
}

std::string landr(std::string l, std::string r) {
  constexpr int kWidth = 30;

  static std::vector<std::string> lines = {l, r};
  return BoosterSeat::layout::fixedWidthRow(
      lines, BoosterSeat::layout::Alignment::LEFT, kWidth);
}
void console_pages::Pages::gfsStatus() {
  data::blocks::StreamsStats stats = shared_data_.blocks.stream_stats.get();

  data::blocks::ModulesStatuses mod_stats =
      shared_data_.blocks.modules_statuses.get();
  data::blocks::SystemInfo sys_info = shared_data_.blocks.system_info.get();

  std::string l = "";
  std::string r = "";
  content_[0] = landr(BoosterSeat::time::utcTimeString() + " UTC ",
                      BoosterSeat::time::localTimeString() + " Local");
  content_[1] = "Uptime: " + shared_data_.misc.getUptimeString();
}

void console_pages::Pages::data() { content_[5] = "Data"; }

void console_pages::Pages::log() { content_[5] = "log"; }

void console_pages::Pages::server() { content_[5] = "server"; }

void console_pages::Pages::console() { content_[5] = "console"; }

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