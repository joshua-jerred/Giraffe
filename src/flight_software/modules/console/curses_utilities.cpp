#include "curses_utilities.h"

void ncurs::internal::Window::win_init() {
  p_window_ = newwin(height_, width_, y_, x_);
  box(p_window_, 0, 0);
  refresh();
  win_refresh();
}

void ncurs::internal::Window::win_reset() {
  // box(p_window_, 0, 0);
  //  mvwprintw(p_window_, 0, 1, "%s", title_.c_str());
  refresh();
  wrefresh(p_window_);
}

void ncurs::internal::Window::win_refresh() {
  // box(p_window_, 0, 0);
  //  mvwprintw(p_window_, 0, 1, "%s", title_.c_str());
  wrefresh(p_window_);
}

void ncurs::internal::Window::win_clear() {
  wclear(p_window_);
  wattroff(p_window_, A_REVERSE);
  box(p_window_, 0, 0);
}

void ncurs::Environment::initialize(Menu &main_menu) {
  main_menu_ = main_menu;

  screen_ = initscr();  // Start curses mode
  nodelay(screen_, TRUE);
  noecho();     // Don't echo input
  curs_set(0);  // Hide cursor

  menu_window_.setPosition(0, 0);
  menu_window_.setSize(kMenuWidth_, kHeight_);

  data_window_.setPosition(kMenuWidth_ + kPadding_, 0);
  data_window_.setSize(kDataWidth_, kHeight_);

  menu_window_.win_init();
  data_window_.win_init();

  displayMenu();
  displayData();
}

void ncurs::Environment::update() {
  checkInput();
  // refresh();
  //  menu_window_.refreshWindow();
  //  wrefresh(screen_);
}

void ncurs::Environment::end() {
  endwin();
  echo();
  curs_set(1);
}

void ncurs::Environment::checkInput() {
  int ch = getch();
  switch (ch) {
    case 0x1B:  // Possible Arrow Key
      if (getch() == 0x5B) {
        ch = getch();
        switch (ch) {
          case 0x42:
            last_key = "down";
            navigateMenu(Environment::NavKey::DOWN);
            break;
          case 0x44:
            last_key = "left";
            navigateMenu(Environment::NavKey::LEFT);
            break;
          case 0x41:
            last_key = "up";
            navigateMenu(Environment::NavKey::UP);
            break;
          case 0x43:
            last_key = "right";
            navigateMenu(Environment::NavKey::RIGHT);
            break;
          default:
            last_key = "other special";
            break;
        }
        displayMenu();
        displayData();
      }
      break;
    case -1:  // No key
      break;
    case 10:
      last_key = "enter";
      break;
    default:
      last_key = "other " + std::to_string(ch);
      break;
  }
  flushinp();
}

void ncurs::Environment::navigateMenu(Environment::NavKey key) {
  switch (key) {
    case Environment::NavKey::DOWN:
      current_menu_hover_ +=
          current_menu_hover_ < current_menu_num_options_ - 1 ? 1 : 0;
      break;

    case Environment::NavKey::UP:
      current_menu_hover_ -= current_menu_hover_ > 0 ? 1 : 0;
      break;

    case Environment::NavKey::RIGHT:
      if (current_menu_->at(current_menu_hover_).sub_menus_.size() > 0) {
        menu_path_.push_back(current_menu_);
        current_menu_ = &(current_menu_->at(current_menu_hover_).sub_menus_);
        current_menu_hover_ = 0;
        current_menu_num_options_ = current_menu_->size();
      }
      break;

    case Environment::NavKey::LEFT:
      if (menu_path_.size() > 1) {
        current_menu_ = menu_path_.back();
        menu_path_.pop_back();
        current_menu_hover_ = 0;
        current_menu_num_options_ = current_menu_->size();
      }
      break;
    default:
      break;
  };
}

void ncurs::Environment::displayMenu() {
  menu_window_.win_clear();

  int i = 1;
  current_menu_num_options_ = current_menu_->size();

  for (MenuOption &option : *current_menu_) {
    if (i - 1 == current_menu_hover_) {
      wattron(menu_window_.p_window_, A_REVERSE);
    } else {
      wattroff(menu_window_.p_window_, A_REVERSE);
    }
    mvwprintw(menu_window_.p_window_, i++, 1, "%s", option.title_.c_str());
  }
  menu_window_.win_refresh();
}

void ncurs::Environment::displayData() {
  data_window_.win_clear();

  static int i = 0;
  mvwprintw(data_window_.p_window_, 1, 1, "iter: %i", i++);
  mvwprintw(data_window_.p_window_, 2, 1, "Current Menu Options: %i",
            current_menu_num_options_);
  mvwprintw(data_window_.p_window_, 3, 1, "Current Hover: %i",
            current_menu_hover_);

  std::string path = "";
  for (auto &menu : menu_path_) {
    path += " > ";
  }

  mvwprintw(data_window_.p_window_, 4, 1, "Current Path: %s", path.c_str());

  mvwprintw(data_window_.p_window_, 5, 1, "Last Key: %s", last_key.c_str());

  data_window_.win_refresh();
}
