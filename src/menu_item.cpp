// Copyright 2023 twowind
#include "menu_item.hpp"

MenuItem::MenuItem(std::string name, std::function<void(WINDOW*)> func)
    : name(name), func(func) {
  item = ::new_item(this->name.c_str(), "");
}

MenuItem::~MenuItem() { ::free_item(item); }
