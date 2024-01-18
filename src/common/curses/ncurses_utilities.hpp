#ifndef NCURSES_UTILITIES_H_
#define NCURSES_UTILITIES_H_

#include <string>
#include <vector>

#include <ncurses.h>

namespace ncurs {

constexpr int K_MAX_NUM_PAGE_LINES = 20;

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

typedef std::string MenuOption;
typedef std::vector<MenuOption> Menu;

class Pages {
public:
  Pages(){};
  virtual ~Pages(){};
  virtual const std::vector<std::string> &getPage(std::string page_name);
  const Menu &getMenuOptions() {
    return menu_;
  }

private:
  Menu menu_ = {"opt1", "opt2", "opt3"};
};

class Environment {
public:
  Environment(Pages &pages) : pages_(pages) {
  }
  ~Environment() {
  }
  Environment(const Environment &) = delete;            // No copy constructor
  Environment &operator=(const Environment &) = delete; // No copy assignment

  void start(int endpoint_update_rate_ms = 1000);
  void update();
  void end();

private:
  Pages &pages_;

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

  std::string current_page_option_ = "opt1";

  const int K_DATA_WINDOW_HEIGHT = 10;
  const int K_DATA_WINDOW_WIDTH = 50;

  const int kHeight_ = K_DATA_WINDOW_HEIGHT + 2;
  const int kMenuWidth_ = 25;
  const int kDataWidth_ = K_DATA_WINDOW_WIDTH;
  const int kPadding_ = 1;

  int current_menu_hover_ = 0;
  int current_menu_num_options_ = 0;
  int current_scroll_pos_ = 0;
  int scroll_handle_height_ = 1;

  int endpoint_update_rate_ms_ = 1000;

  WINDOW *screen_ = nullptr;
  Window menu_window_ = Window();
  Window data_window_ = Window();
  Window data_scroll_bar_ = Window();
};

} // namespace ncurs

#endif