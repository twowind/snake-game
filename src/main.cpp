#include "game.hpp"

int main() {
  auto& game = Game::GetInstance();
  game.Init();
  game.Run();
  return 0;
  return 0;
}
