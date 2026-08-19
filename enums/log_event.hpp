#pragma once
#include <chrono>
#include <string>

struct Event {
  std::chrono::system_clock::time_point sequence;
  std::string payload;
};