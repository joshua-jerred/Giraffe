#ifndef CONSOLE_PAGES_HPP_
#define CONSOLE_PAGES_HPP_

#include "configuration.hpp"
#include "shared_data.hpp"
#include <array>
#include <string>
#include <utility>
#include <vector>

namespace console_pages {
enum class PageOption { GFS_STATUS, DATA, LOG, SERVER, EXTENSIONS };

inline constexpr int kMaxNumPageLines = 25;
inline constexpr int kDataWindowWidth = 60;
inline constexpr int kDataWindowHeight = 10;
inline constexpr PageOption kInitialPage = PageOption::DATA;

typedef std::pair<std::string, PageOption> Option;
typedef std::vector<Option> Menu;

class Pages {
public:
  Pages(cfg::Configuration &config, data::SharedData &shared_data)
      : config_(config), shared_data_(shared_data) {
  }
  ~Pages() = default;

  const std::array<std::string, kMaxNumPageLines> &getCurrentPage();
  int getNumLinesOnPage() const {
    return current_num_lines_;
  }
  Menu getCurrentMenu();
  void navigateMenu(PageOption key);
  int getMaxNumPageLines() const {
    return kMaxNumPageLines;
  }

private:
  void gfsStatus();
  void data();
  void log();
  void server();
  void extensions();
  void setNumLinesOnPage(const int num_lines);

  cfg::Configuration &config_;
  data::SharedData &shared_data_;

  PageOption current_page_ = kInitialPage;
  int current_num_lines_ = 0;
  std::array<std::string, kMaxNumPageLines> content_ = {};
  Menu current_menu_ = {
      {"GFS Status", PageOption::GFS_STATUS},
      {"Data", PageOption::DATA},
      {"Log", PageOption::LOG},
      {"Server", PageOption::SERVER},
      {"Extensions", PageOption::EXTENSIONS},
  };
};

} // namespace console_pages

#endif /* CONSOLE_PAGES_HPP_ */