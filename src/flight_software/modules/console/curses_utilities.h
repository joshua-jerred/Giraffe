#ifndef CURSES_UTILITIES_H_
#define CURSES_UTILITIES_H_

#include <ncurses.h>

#include <functional>
#include <string>
#include <vector>

namespace ncurs {
namespace internal {
class Window {
 public:
  Window() {
  }
  ~Window() {
    delwin(p_window_);
  }

  Window(const Window &) = delete;             // No copy constructor
  Window &operator=(const Window &) = delete;  // No copy assignment

  void setPosition(int x, int y) {
    x_ = x;
    y_ = y;
  }

  void setSize(int width, int height) {
    width_ = width;
    height_ = height;
  }

  void win_init();
  void win_reset();
  void win_refresh();
  void win_clear();

  WINDOW *p_window_ = nullptr;

  std::string title_ = "";
  int height_ = 5;
  int width_ = 5;
  int x_ = 0;
  int y_ = 0;
};
}  // namespace internal

struct MenuOption {
  MenuOption(std::string title, std::vector<MenuOption> sub_menus = {})
      : title_(title), sub_menus_(sub_menus) {
    if (sub_menus_.size() == 0) {
      endpoint = true;
    }
  }

  std::string title_;
  std::vector<MenuOption> sub_menus_;
  std::function<std::vector<std::string>(void)> console_data_ = []() {
    return std::vector<std::string>({"empty"});
  };

  bool endpoint = false;
};

typedef std::vector<MenuOption> Menu;

class Environment {
 public:
  Environment() {
  }
  ~Environment() {
  }
  Environment(const Environment &) = delete;             // No copy constructor
  Environment &operator=(const Environment &) = delete;  // No copy assignment

  void initialize(Menu &main_menu, int endpoint_update_rate_ms = 1000);
  void update();
  void end();

 private:

  enum class Focus { MENU, DATA };
  enum class NavKey { LEFT, RIGHT, UP, DOWN };

  void checkInput();
  void displayMenu();
  void displayData();
  void navigateMenu(NavKey key);

  Environment::Focus focus_ = Environment::Focus::MENU;

  std::string last_key = "";  // Remove this

  const int kHeight_ = 15;
  const int kMenuWidth_ = 20;
  const int kDataWidth_ = 40;
  const int kPadding_ = 1;

  Menu main_menu_ = {};

  std::vector<Menu *> menu_path_ = {&main_menu_};
  Menu *current_menu_ = &main_menu_;

  int current_menu_hover_ = 0;
  int current_menu_num_options_ = 0;

  int endpoint_update_rate_ms_ = 1000;

  WINDOW *screen_ = nullptr;
  internal::Window menu_window_ = internal::Window();
  internal::Window data_window_ = internal::Window();
};

}  // namespace ncurs

#endif