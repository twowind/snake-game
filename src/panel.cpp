// Copyright 2023 twowind
#include "panel.hpp"

#include <curses.h>
#include <panel.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <utility>

Panel::Panel(const int width, const int start_y, const int start_x) {
  win = ::newwin(3, width, start_y, start_x);
  panel = ::new_panel(win);
  // Enable the special keycode mode for this window
  ::keypad(win, true);
}

void Panel::Show(const std::string &str) const {
  int x = getmaxx(win);
  ::mvwprintw(win, 1, (x - str.length()) / 2, "%s", str.c_str());
  ::box(win, 0, 0);
  UpdatePanel();

  int choice;
  do {
    choice = ::wgetch(win);
  } while (choice != 27 && choice != '\n');

  ::hide_panel(panel);
  ::wclear(win);
  UpdatePanel();
}
