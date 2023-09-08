// Copyright 2023 twowind
#pragma once

#include <menu.h>
#include <ncurses.h>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>
#include <list>
#include <memory>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "menu.hpp"
#include "panel.hpp"

enum class GameStatus { kMain, kGaming, kStop, kOver, kExit };

class Game {
 public:
  Game(const Game&) = delete;
  Game(Game&&) = delete;
  Game& operator=(const Game&) = delete;
  Game& operator=(Game&&) = delete;
  ~Game() {
    delete main_menu;
    delete stop_menu;
    delete msg_panel;
    delete snake;
    delete apple;
  }

 public:
  static Game& GetInstance();
  void Init();
  void Run();
  void Exit();

 private:
  ushort speed = 8;
  uint score = 0;
  uint best_score = 0;
  GameStatus status = GameStatus::kMain;
  std::chrono::system_clock::time_point game_start;

  std::list<std::pair<int, int>>* snake = new std::list<std::pair<int, int>>();
  std::pair<int, int>* apple = new std::pair<int, int>();

  const int kMinTerWidth = 100;
  const int kMinTerHeight = 10;

  const char kAppleIcon = '0';
  const char kSnakeHeadIcon = '@';
  const char kSnakeBodyIcon = '0';

  const ushort kBorder = 1;
  const ushort kHeadHeight = 1;

  const char* kStartGame = "Start Game";
  const char* kContinue = "Continue";
  const char* kEndGame = "End Game";

  Menu* main_menu;
  Menu* stop_menu;
  Panel* msg_panel;

 private:
  Game() = default;

  void ResetSnake();

  void DrawScene();

  void DrawSnake();

  void DrawOverPanel();

  void SceneClear();

  void DrawApple();

  void DrawScore();

  void DrawTime(int64_t time);

  void GenApple();

  void UpdateGame(int choice);

  void Reset();

  void DrawBest() {
    static char str[64] = {0};
    std::snprintf(str, sizeof(str), "The Best Score: %u", best_score);
    mvprintw(kBorder, (COLS - std::strlen(str)) / 2, "%s", str);
  }

  // TODO(twowind)
  inline void UpdateSpeed() {
    if (score % 100 == 0 && score < 1000) {
      ++speed;
    }
  }

  // TODO(twowind)
  inline std::chrono::milliseconds get_delay() {
    return std::chrono::milliseconds(1000 - speed * 100);
  }

  inline bool BodyCrashCheck() {
    return std::find_if(++snake->begin(), snake->end(),
                        [snake_header = snake->front()](auto item) {
                          return item.first == snake_header.first &&
                                 item.second == snake_header.second;
                        }) != snake->end();
  }

  inline bool EdgeCrashCheck() {
    auto [first, second] = snake->front();
    return first == 2 * kBorder + kHeadHeight - 1 || first == ::LINES - 1 ||
           second == 0 || second == COLS - 1;
  }

  inline auto CalcMiddle(int y, int x) {
    return std::pair<int, int>((::LINES - y) / 2, (::COLS - x) / 2);
  }
};
