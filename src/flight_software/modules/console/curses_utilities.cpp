#include "curses_utilities.h"

#include <iostream>

class Window {
 public:
  Window(int height, int width, int x, int y, std::string title)
      : height_(height), width_(width), x_(x), y_(y), title_(title) {
  }
  ~Window() {
    delwin(p_window_);
  }

  Window(const Window &) = delete;             // No copy constructor
  Window &operator=(const Window &) = delete;  // No copy assignment

  void init() {
    p_window_ = newwin(height_, width_, y_, x_);
    box(p_window_, 0, 0);
    refresh();
    refreshWindow();
  }

 protected:
  void refreshWindow() {
    mvwprintw(p_window_, 0, 1, "%s", title_.c_str());
    wrefresh(p_window_);
  }

  WINDOW *p_window_ = nullptr;
  int height_;
  int width_;
  int x_;
  int y_;
  std::string title_;
};

ncurs::Environment::Environment(int height, int width)
    : height_(height), width_(width) {
}

ncurs::Environment::~Environment() {
}

void ncurs::Environment::init(MainMenu &main_menu) {
  main_menu_ = main_menu;

  screen_ = initscr();  // Start curses mode
  nodelay(screen_, TRUE);
  noecho();  // Don't echo input
  // curs_set(0);  // Hide cursor

  for (const SubMenu &sub : main_menu_) {
    std::cout << sub.title_ << std::endl;
    for (const MenuOption &opt : sub.options_) {
      std::cout << "  " << opt.title_ << std::endl;
    }
  }
}

void ncurs::Environment::update() {
  checkInput();
}

void ncurs::Environment::end() {
  endwin();
  echo();
  curs_set(1);
}

void ncurs::Environment::checkInput() {
  int ch = getch();
  if (ch == 0x1B && getch() == 0x5B) {
    ch = getch();
    switch (ch) {
      case 0x42:
        std::cout << "Down" << std::endl;
        break;
      case 0x44:
        std::cout << "Left" << std::endl;
        break;
      case 0x41:
        std::cout << "Up" << std::endl;
        break;
      case 0x43:
        std::cout << "Right" << std::endl;
        break;
      default:
        std::cout << "Unknown" << std::endl;
        break;
    }
  }
  flushinp();
}