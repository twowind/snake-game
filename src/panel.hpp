// Copyright 2023 twowind
#pragma once

#include <curses.h>
#include <panel.h>

#include <algorithm>
#include <string>

class Panel {
 public:
  Panel(const int width, const int start_y, const int start_x);
  void Show(const std::string &str) const;

  ~Panel() {
    ::del_panel(panel);
    ::delwin(win);
  }

 private:
  WINDOW *win;
  PANEL *panel;
  std::string msg;

  inline void UpdatePanel() const {
    ::update_panels();
    ::doupdate();
  }
};
