#ifndef CURSES_UTILITIES_H_
#define CURSES_UTILITIES_H_

#include <ncurses.h>

#include <string>
#include <vector>

namespace ncurs {
struct MenuOption {
  MenuOption(std::string title) : title_(title) {
  }

  std::string title_ = "";
};

struct SubMenu {
  SubMenu(std::string title, std::vector<MenuOption> options)
      : title_(title), options_(options) {
  }

  std::string title_ = "";
  std::vector<MenuOption> options_ = {};
};

typedef std::vector<SubMenu> MainMenu;

class Environment {
 public:
  Environment(int height, int width);
  ~Environment();
  Environment(const Environment &) = delete;             // No copy constructor
  Environment &operator=(const Environment &) = delete;  // No copy assignment

  void init(MainMenu &main_menu);
  void update();
  void end();

 private:
  void checkInput();

  int height_;
  int width_;
  MainMenu main_menu_ = {};
  WINDOW *screen_ = nullptr;
};

}  // namespace ncurs

#endif