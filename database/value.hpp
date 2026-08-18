#pragma once
#include <chrono>
#include <string>

using namespace std;
using namespace std::chrono;

struct Value{
  string val;
  system_clock::time_point expiry_time;
};
