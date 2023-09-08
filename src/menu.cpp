// Copyright 2023 twowind
#include "menu.hpp"

#include <curses.h>
#include <menu.h>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

#include "menu_item.hpp" 

Menu::Menu(std::initializer_list<MenuItem *> items, int y, int x)
    : items(items) {
  std::transform(items.begin(), items.end(), std::back_inserter(menu_items),
                 [](auto item) { return item->GetItem(); });
  menu_items.push_back(nullptr);
  menu = ::new_menu(menu_items.data());

  auto const[height, width] = CalcSize(items);
  if (y == -1) y = (::LINES - height) / 2;
  if (x == -1) x = (::COLS - width) / 2;
  win = ::newwin(height + 2, width + 1 + 3, y, x);

  // Enable the special keycode mode for this window
  ::keypad(win, true);

  ::set_menu_win(menu, win);
  ::set_menu_sub(menu, ::derwin(win, height, width + 1, 1, 1));
}

void Menu::ShowWaitClose() const {
  ::box(win, 0, 0);
  ::post_menu(menu);

  int choice;
  while ((choice = ::wgetch(win)) != '\n') {
    switch (choice) {
      case KEY_DOWN:
        ::menu_driver(menu, REQ_DOWN_ITEM);
        break;
      case KEY_UP:
        ::menu_driver(menu, REQ_UP_ITEM);
        break;
    }
  }

  auto const enter_item_name = ::item_name(::current_item(menu));

  ::unpost_menu(menu);
  ::wclear(win);

  // Queries and executes the callback function for this menu option
  auto const i = std::find_if(
      items.cbegin(), items.cend(), [enter_item_name](auto const item) {
        return std::strcmp(enter_item_name, item->GetName().c_str()) == 0;
      });
  (*i)->func(win);
}

Menu::~Menu() {
  for (auto item : items) {
    delete item;
    item = nullptr;
  }
  ::free_menu(menu);
  ::delwin(win);
}
