// Copyright 2023 twowind
#pragma once

#include <curses.h>
#include <menu.h>

#include <algorithm>
#include <functional>
#include <string>

class MenuItem {
 public:
  MenuItem(std::string name, std::function<void(WINDOW*)> func);
  explicit MenuItem(std::string name) : MenuItem(name, [](auto item) {}) {}
  ~MenuItem();

  MenuItem(const MenuItem&) = delete;
  MenuItem(MenuItem&&) = delete;
  MenuItem& operator=(const MenuItem&) = delete;
  MenuItem& operator=(MenuItem&&) = delete;

  std::function<void(WINDOW*)> func;
  inline const std::string& GetName() const { return name; }
  inline ::ITEM* GetItem() const { return item; }

 private:
  std::string name;
  ::ITEM* item;
};
