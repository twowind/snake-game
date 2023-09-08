// Copyright 2023 twowind
#include "utils.hpp"
#include <random>
#include <thread>

namespace utils {
int Random(const int min, const int max) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution dis(min, max);
  return dis(gen);
}
}  // namespace utils
