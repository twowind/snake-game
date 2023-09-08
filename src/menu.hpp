// Copyright 2023 twowind
#pragma once

#include <curses.h>
#include <menu.h>
#include <sys/types.h>

#include <algorithm>
#include <cstring>
#include <initializer_list>
#include <string>
#include <utility>
#include <vector>

#include "menu_item.hpp"

class Menu {
 public:
  /**
   * @brief If x and y are -1, then center the display
   *
   * @param items
   * @param y
   * @param x
   */
  Menu(std::initializer_list<MenuItem*> items, int y, int x);
  explicit Menu(std::initializer_list<MenuItem*> items) : Menu(items, -1, -1) {}
  Menu(const Menu&) = delete;
  Menu(Menu&&) = delete;
  Menu& operator=(const Menu&) = delete;
  Menu& operator=(Menu&&) = delete;
  ~Menu();

  void ShowWaitClose() const;

 private:
  MENU* menu;
  WINDOW* win;
  std::vector<ITEM*> menu_items;
  std::vector<MenuItem*> items;

  inline const std::pair<int, int> CalcSize(
      const std::vector<MenuItem*>& items) const {
    auto item = std::max_element(
        items.cbegin(), items.cend(), [](auto const a, auto const b) {
          return a->GetName().length() < b->GetName().length();
        });
    return std::pair<int, int>(items.size(), (*item)->GetName().length());
  }
};
