#ifndef CURSES_UTILITIES_H_
#define CURSES_UTILITIES_H_

#include <ncurses.h>

#include "console_pages.hpp"
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

  Window(const Window &) = delete;            // No copy constructor
  Window &operator=(const Window &) = delete; // No copy assignment

  void setPosition(int x, int y) {
    x_ = x;
    y_ = y;
  }

  void setSize(int width, int height) {
    width_ = width;
    height_ = height;
  }

  void winInit();
  void winReset();
  void winRefresh();
  void winClear();
  void winHide();

  WINDOW *p_window_ = nullptr;

  std::string title_ = "";
  int height_ = 5;
  int width_ = 5;
  int x_ = 0;
  int y_ = 0;
};

} // namespace internal

class Environment {
public:
  Environment(console_pages::Pages &pages) : pages_(pages) {
  }
  ~Environment() {
  }
  Environment(const Environment &) = delete;            // No copy constructor
  Environment &operator=(const Environment &) = delete; // No copy assignment

  void start(int endpoint_update_rate_ms = 1000);
  void update();
  void end();

private:
  console_pages::Pages &pages_;

  enum class Focus { MENU, DATA };
  enum class NavKey { LEFT, RIGHT, UP, DOWN };

  void checkInput();
  void displayMenu();
  void displayData();
  void displayScrollBar();
  void navigateMenu(NavKey key);
  void scrollDataDown();
  void scrollDataUp();
  void enforceScrollBounds();
  int getMaxScrollPos();

  Environment::Focus focus_ = Environment::Focus::MENU;

  const int kHeight_ = console_pages::K_DATA_WINDOW_HEIGHT + 2;
  const int kMenuWidth_ = 25;
  const int kDataWidth_ = console_pages::K_DATA_WINDOW_WIDTH;
  const int kPadding_ = 1;

  int current_menu_hover_ = 0;
  int current_menu_num_options_ = 0;
  int current_scroll_pos_ = 0;
  int scroll_handle_height_ = 1;

  int endpoint_update_rate_ms_ = 1000;

  WINDOW *screen_ = nullptr;
  internal::Window menu_window_ = internal::Window();
  internal::Window data_window_ = internal::Window();
  internal::Window data_scroll_bar_ = internal::Window();
};

} // namespace ncurs

#endif