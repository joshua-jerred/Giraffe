#ifndef CONSOLE_PAGES_HPP_
#define CONSOLE_PAGES_HPP_

#include "configuration.hpp"
#include "shared_data.hpp"
#include <array>
#include <string>
#include <utility>
#include <vector>

namespace console_pages {
inline constexpr int kNumPageLines = 12;

enum class PageOption { GFS_STATUS, DATA, LOG, SERVER, CONSOLE, BACK };

typedef std::pair<std::string, PageOption> Option;
typedef std::vector<Option> Menu;

class Pages {
public:
  Pages(cfg::Configuration &config, data::SharedData &shared_data)
      : config_(config), shared_data_(shared_data) {}
  ~Pages() = default;

  std::array<std::string, kNumPageLines> getCurrentPage();
  Menu getCurrentMenu();
  void navigateMenu(PageOption key);

private:
  void gfsStatus();
  void data();
  void log();
  void server();
  void console();

  cfg::Configuration &config_;
  data::SharedData &shared_data_;

  PageOption current_page_ = PageOption::GFS_STATUS;
  std::array<std::string, kNumPageLines> content_ = {"Test", "Test2", "Test3"};
  Menu current_menu_ = {
      {"GFS Status", PageOption::GFS_STATUS},
      {"Data", PageOption::DATA},
      {"Log", PageOption::LOG},
      {"Server", PageOption::SERVER},
      {"Console", PageOption::CONSOLE},
  };
};

} // namespace console_pages

#endif /* CONSOLE_PAGES_HPP_ */