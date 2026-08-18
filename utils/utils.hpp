#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "../enums/command.hpp"
#include "../enums/commnd_result.hpp"

using namespace std;

namespace splitter_utils{
  vector<string> splitter(string& str,const char& delimeter);
  pair<vector<string>,vector<string>> get_args(string& str);
}

namespace command_utils{
  string cmd_to_string(COMMAND& cmd);
  COMMAND string_to_cmd(string& str);
}

namespace serealize_utils{
  string response_buffer(CommandResult& cmd_result);
}