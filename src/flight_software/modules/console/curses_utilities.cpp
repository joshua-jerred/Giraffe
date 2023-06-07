#include "curses_utilities.h"

#include "time_types.hpp"

#include <algorithm>

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

void ncurs::Environment::start(int endpoint_update_rate_ms) {
  endpoint_update_rate_ms_ = endpoint_update_rate_ms;

  screen_ = initscr(); // Start curses mode
  nodelay(screen_, TRUE);
  noecho();    // Don't echo input
  curs_set(0); // Hide cursor

  menu_window_.setPosition(0, 0);
  menu_window_.setSize(kMenuWidth_, kHeight_);

  data_window_.setPosition(kMenuWidth_ + kPadding_, 0);
  data_window_.setSize(kDataWidth_, kHeight_);

  data_scroll_bar_.setPosition(kMenuWidth_ + kPadding_ + kDataWidth_, 0);
  data_scroll_bar_.setSize(2, kHeight_);

  menu_window_.win_init();
  data_window_.win_init();
  data_scroll_bar_.win_init();

  displayMenu();
  displayData();
  displayScrollBar();
}

void ncurs::Environment::update() {
  checkInput();

  data_window_.win_reset();

  static giraffe_time::TimePoint update_clock = giraffe_time::Clock::now();
  int mils_elapsed = giraffe_time::millisecondsElapsed(update_clock);
  if (mils_elapsed > endpoint_update_rate_ms_) {
    displayData();
    update_clock = giraffe_time::Clock::now();
  }
}

void ncurs::Environment::end() {
  endwin();
  echo();
  curs_set(1);
}

void ncurs::Environment::checkInput() {
  int ch = getch();
  switch (ch) {
  case 0x1B: // Possible Arrow Key
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
      displayScrollBar();
    }
    break;
  case -1: // No key
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
    if (focus_ == Focus::MENU) {
      current_menu_hover_ +=
          current_menu_hover_ < current_menu_num_options_ - 1 ? 1 : 0;
    } else if (focus_ == Focus::DATA) {
      scrollDataDown();
    }
    break;

  case Environment::NavKey::UP:
    if (focus_ == Focus::MENU) {
      current_menu_hover_ -= current_menu_hover_ > 0 ? 1 : 0;
    } else if (focus_ == Focus::DATA) {
      scrollDataUp();
    }
    break;

  case Environment::NavKey::RIGHT:
    focus_ = Focus::DATA;
    break;

  case Environment::NavKey::LEFT:
    focus_ = Focus::MENU;
    break;
  default:
    break;
  };

  pages_.navigateMenu(pages_.getCurrentMenu()[current_menu_hover_].second);
}

void ncurs::Environment::displayMenu() {
  menu_window_.win_clear();

  int i = 1;
  current_menu_num_options_ = pages_.getCurrentMenu().size();

  for (const console_pages::Option &option : pages_.getCurrentMenu()) {
    std::string title = option.first;

    if (i - 1 == current_menu_hover_ && focus_ == Focus::MENU) {
      wattron(menu_window_.p_window_, A_REVERSE);
    } else if (i - 1 == current_menu_hover_ && focus_ == Focus::DATA) {
      title += " -->";
      wattroff(menu_window_.p_window_, A_REVERSE);
      mvwprintw(menu_window_.p_window_, i++, 1, "%s", title.c_str());
      continue;
    } else {
      wattroff(menu_window_.p_window_, A_REVERSE);
    }
    // pages_.navigateMenu(option.second);
    // if (!option.endpoint) { // has sub-options
    //   title += " +";
    // }
    mvwprintw(menu_window_.p_window_, i++, 1, "%s", title.c_str());
  }
  menu_window_.win_refresh();
}

void ncurs::Environment::displayData() {
  data_window_.win_clear();
  static int j = 0;

  if (true) {
    /**
     * @todo remove this
     */
    mvwprintw(data_window_.p_window_, 1, 1, "iter: %i", j++);

    int window_line_pos = 1; // start below the border
    const std::array<std::string, console_pages::kMaxNumPageLines> &page =
        pages_.getCurrentPage();

    for (int i = current_scroll_pos_ + 1; i < current_scroll_pos_ + kHeight_ - 1; i++) {
      if (i >= console_pages::kMaxNumPageLines) {
        throw std::runtime_error("Scroll pos out of bounds");
      }
      std::string line = page[i];
      mvwprintw(data_window_.p_window_, window_line_pos++, 1, "%s", line.c_str());
    }
  } else {
    mvwprintw(data_window_.p_window_, 1, 1, "iter: %i", j++);
    mvwprintw(data_window_.p_window_, 2, 1, "Current Menu Options: %i",
              current_menu_num_options_);
    mvwprintw(data_window_.p_window_, 3, 1, "Current Hover: %i",
              current_menu_hover_);

    std::string path = "";
    // for (auto &menu : menu_path_) {
    //   path += " > ";
    // }

    mvwprintw(data_window_.p_window_, 4, 1, "Current Path: %s", path.c_str());

    mvwprintw(data_window_.p_window_, 5, 1, "Last Key: %s", last_key.c_str());
  }

  data_window_.win_refresh();
}

void ncurs::Environment::displayScrollBar() {
  data_scroll_bar_.win_clear();
  data_scroll_bar_.win_refresh();

  scroll_handle_height_ =
      std::clamp(pages_.getNumLinesOnPage() / kHeight_, 1, kHeight_ - 2);
  int scroll_handle_end = current_scroll_pos_ + scroll_handle_height_;

  if (pages_.getNumLinesOnPage() < kHeight_) {
    scroll_handle_end = kHeight_;
  }

  for (int i = 0; i < kHeight_; i++) {
    if (focus_ != Focus::DATA) {
      mvwprintw(data_scroll_bar_.p_window_, i, 0, "  ");
    } else {
      if (i >= current_scroll_pos_ && i < scroll_handle_end) {
        mvwprintw(data_scroll_bar_.p_window_, i, 0, "||");
        continue;
      }
      mvwprintw(data_scroll_bar_.p_window_, i, 0, "--");
    }
  }
  data_scroll_bar_.win_refresh();
}

void ncurs::Environment::scrollDataUp() {
  if (current_scroll_pos_ > 0) {
    current_scroll_pos_--;
  }
}

void ncurs::Environment::scrollDataDown() {
  if (current_scroll_pos_ <
      pages_.getNumLinesOnPage() - kHeight_ - scroll_handle_height_ &&
      current_scroll_pos_ < pages_.getMaxNumPageLines()) {
    current_scroll_pos_++;
  }
}
