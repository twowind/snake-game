// Copyright 2023 twowind
#include "game.hpp"

#include <curses.h>
#include <menu.h>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <iterator>
#include <list>
#include <string>
#include <thread>
#include <utility>

#include "menu.hpp"
#include "menu_item.hpp"
#include "panel.hpp"
#include "utils.hpp"

Game &Game::GetInstance() {
  static Game instance;
  return instance;
}

void Game::Init() {
  ::initscr();
  ::noecho();

  ::box(stdscr, 0, 0);
  // Enable special keycode mode
  ::keypad(stdscr, true);
  // Hide cursor
  ::curs_set(0);

  if (COLS < kMinTerWidth || LINES < kMinTerHeight) {
    std::string warn = "this window too low";
    auto [y, x] = CalcMiddle(1, warn.length());
    ::mvprintw(y, x, "%s", warn.c_str());
    return;
  }

  ResetSnake();

  GenApple();

  // Create main menu and stop menu
  main_menu = new Menu({
      new MenuItem(kStartGame,
                   [this](auto win) {
                     this->status = GameStatus::kGaming;
                     this->game_start = std::chrono::system_clock::now();
                   }),
      new MenuItem(kEndGame,
                   [this](auto win) { this->status = GameStatus::kExit; }),
  });
  stop_menu = new Menu({
      new MenuItem(kContinue,
                   [this](auto win) { this->status = GameStatus::kGaming; }),
      new MenuItem(kEndGame,
                   [this](auto win) { this->status = GameStatus::kExit; }),
  });

  // Create game over panel
  msg_panel = new Panel(50, LINES / 2, (COLS - 50) / 2);
}

void Game::ResetSnake() {
  snake->clear();
  snake->push_back(std::pair<int, int>(LINES / 2, COLS / 2 - 1));
  snake->push_back(std::pair<int, int>(LINES / 2, COLS / 2));
}

void Game::UpdateGame(int choice) {
  auto snake_header = snake->front();
  // Modify head to body
  ::mvprintw(snake_header.first, snake_header.second, "%c", kSnakeBodyIcon);

  auto snake_second = ++snake->begin();
  switch (choice) {
    case 'k':
    case KEY_UP: {
      if (snake_header.first == snake_second->first + 1) {
        goto default_run;
      }
      --snake_header.first;
    } break;
    case 'j':
    case KEY_DOWN: {
      if (snake_header.first == snake_second->first - 1) {
        goto default_run;
      }
      ++snake_header.first;
    } break;
    case 'h':
    case KEY_LEFT: {
      if (snake_header.second == snake_second->second + 1) {
        goto default_run;
      }
      --snake_header.second;
    } break;
    case 'l':
    case KEY_RIGHT: {
      if (snake_header.second == snake_second->second - 1) {
        goto default_run;
      }
      ++snake_header.second;
    } break;
    default_run:
    default: {
      snake_header.first = 2 * snake_header.first - snake_second->first;
      snake_header.second = 2 * snake_header.second - snake_second->second;
    }
  }

  // Delete tail.
  // Note: This operation must precede the following add header operation
  auto tail = snake->back();
  ::mvprintw(tail.first, tail.second, "%c", ' ');
  snake->pop_back();

  // Add head
  snake->push_front(snake_header);
  ::mvprintw(snake_header.first, snake_header.second, "%c", kSnakeHeadIcon);

  // Crash check
  if (BodyCrashCheck() || EdgeCrashCheck()) {
    status = GameStatus::kOver;
    return;
  }

  if (snake_header.first == apple->first &&
      snake_header.second == apple->second) {
    ++score;
    DrawScore();

    // Checking for broken records
    if (score > best_score) {
      best_score = score;
      DrawBest();
    }

    UpdateSpeed();

    // Add tail
    ::mvprintw(tail.first, tail.second, "%c", kSnakeBodyIcon);
    snake->push_back(tail);

    GenApple();
    DrawApple();
  }
}

void Game::Reset() {
  ResetSnake();
  score = 0;
  GenApple();
}

void Game::Exit() { ::endwin(); }

void Game::DrawSnake() {
  // Print head
  ::mvprintw(snake->front().first, snake->front().second, "%c", kSnakeHeadIcon);
  // Print body
  std::for_each(++snake->begin(), snake->end(), [this](auto item) {
    ::mvprintw(item.first, item.second, "%c", kSnakeBodyIcon);
  });
}

void Game::DrawScene() {
  box(stdscr, 0, 0);
  mvhline(kBorder + kHeadHeight, kBorder, '-', COLS - 2 * kBorder);
  DrawApple();
  DrawBest();
  DrawScore();
  DrawTime(0);
  DrawSnake();
}

void Game::DrawOverPanel() {
  static char msg[64] = {0};
  std::snprintf(msg, sizeof(msg), "Game Over! Score is %d.", score);
  msg_panel->Show(msg);
}

void Game::SceneClear() {
  ::wclear(stdscr);
  ::wrefresh(stdscr);
}

void Game::Run() {
  while (true) {
    switch (status) {
      case GameStatus::kGaming: {
        // Setting the keypad event blocking time
        ::timeout(get_delay().count());

        DrawScene();

        using std::chrono::duration_cast;
        using std::chrono::high_resolution_clock;
        using std::chrono::milliseconds;

        int choice;
        while (status == GameStatus::kGaming) {
          auto const frame_start = high_resolution_clock::now();

          choice = getch();

          const int KEY_ESC = 27;
          if (choice == KEY_ESC) {
            status = GameStatus::kStop;
            break;
          }

          UpdateGame(choice);

          // Flush keypad event cache
          ::flushinp();

          auto const frame_end = high_resolution_clock::now();

          DrawTime(duration_cast<milliseconds>(frame_end - game_start).count() /
                   1000);

          // Calculate the remaining time of the frame, then wait
          auto duration = duration_cast<milliseconds>(frame_end - frame_start);
          auto sleep =
              duration > get_delay() ? get_delay() : (get_delay() - duration);
          std::this_thread::sleep_for(sleep);
        }
      }; break;
      case GameStatus::kMain: {
        main_menu->ShowWaitClose();
      } break;
      case GameStatus::kStop: {
        using std::chrono::system_clock;
        auto const t1 = system_clock::now();
        stop_menu->ShowWaitClose();
        auto const t2 = system_clock::now();

        // Game start time needs to be recalculated when pausing
        game_start += (t2 - t1);
      } break;
      case GameStatus::kOver: {
        DrawOverPanel();
        Reset();

        // The game is over and you need to go back to the main menu, then you
        // need to clear the screen scene
        SceneClear();
        status = GameStatus::kMain;
      } break;
      case GameStatus::kExit:
        Exit();
        return;
    }
  }
}

// TODO(twowind) efficiency can be optimized
void Game::GenApple() {
  std::list<std::pair<int, int>>::const_iterator i;
  do {
    apple->first =
        utils::Random(kBorder * 2 + kHeadHeight, LINES - 1 - kBorder);
    apple->second = utils::Random(kBorder, COLS - 1 - kBorder);

    i = std::find_if(snake->cbegin(), snake->cend(), [this](auto const item) {
      return item.first == apple->first && item.second == apple->second;
    });
  } while (i != snake->cend());
}

void Game::DrawApple() {
  ::mvprintw(apple->first, apple->second, "%c", kAppleIcon);
}

void Game::DrawScore() {
  static char str[64] = {0};
  std::snprintf(str, sizeof(str), "score: %d", score);
  ::mvprintw(kBorder, COLS - kBorder - strlen(str), "%s", str);
}

void Game::DrawTime(int64_t time) {
  static char str[64] = {0};
  auto const hour = time / 3600;
  auto const minutes = time % 3600 / 60;
  auto const second = time % 60;
  std::snprintf(str, sizeof(str), "Time: %04ld:%02ld:%02ld", hour, minutes,
                second);
  ::mvprintw(kBorder, kBorder, "%s", str);
}
