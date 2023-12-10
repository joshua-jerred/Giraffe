#ifndef CONSOLE_PAGES_HPP_
#define CONSOLE_PAGES_HPP_

#include "configuration.hpp"
#include "shared_data.hpp"
#include <array>
#include <string>
#include <utility>
#include <vector>

namespace console_pages {
enum class PageOption {
  GFS_STATUS,
  DATA,
  LOG,
  SERVER,
  EXTENSIONS,
  EXTENSION_DATA,
  SYSTEM,
  LOCATION,
  TELEMETRY
};

inline constexpr int K_MAX_NUM_PAGE_LINES = 25;
inline constexpr int K_DATA_WINDOW_WIDTH = 100;
inline constexpr int K_DATA_WINDOW_HEIGHT = 15;
inline constexpr PageOption K_INITIAL_PAGE = PageOption::GFS_STATUS;

typedef std::pair<std::string, PageOption> Option;
typedef std::vector<Option> Menu;

class Pages {
public:
  Pages(cfg::Configuration &config, data::SharedData &shared_data)
      : config_(config), shared_data_(shared_data) {
  }
  ~Pages() = default;

  const std::array<std::string, K_MAX_NUM_PAGE_LINES> &getCurrentPage();
  int getNumLinesOnPage() const {
    return current_num_lines_;
  }
  Menu getCurrentMenu();
  void navigateMenu(PageOption key);
  int getMaxNumPageLines() const {
    return K_MAX_NUM_PAGE_LINES;
  }

private:
  void gfsStatus();
  void data();
  void log();
  void server();
  void extensions();
  void system();
  void location();
  void extensionData();
  void telemetry();

  void setNumLinesOnPage(const int num_lines);

  cfg::Configuration &config_;
  data::SharedData &shared_data_;

  PageOption current_page_ = K_INITIAL_PAGE;
  int current_num_lines_ = 0;
  std::array<std::string, K_MAX_NUM_PAGE_LINES> content_ = {};
  Menu current_menu_ = {
      {"GFS Status", PageOption::GFS_STATUS},
      {"Data", PageOption::DATA},
      {"Log", PageOption::LOG},
      {"Server", PageOption::SERVER},
      {"Extensions", PageOption::EXTENSIONS},
      {"Extension Data", PageOption::EXTENSION_DATA},
      {"System", PageOption::SYSTEM},
      {"Location", PageOption::LOCATION},
      {"Telemetry", PageOption::TELEMETRY},
  };
};

} // namespace console_pages

#endif /* CONSOLE_PAGES_HPP_ */