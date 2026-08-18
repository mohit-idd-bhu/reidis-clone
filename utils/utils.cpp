#include<iostream>
#include<vector>
#include "utils.hpp"
#include <string>

using namespace std;

namespace splitter_utils{
  vector<string> splitter(string& str,const char& delimeter){
    string search = str+delimeter;
    vector<string> tokens;
    size_t start = 0,end;
    while((end = search.find(delimeter,start))!= string::npos){
      tokens.push_back(search.substr(start,end-start));
      start=end+1;
    }
    return tokens;
  }

  pair<vector<string>,vector<string>> get_args(string& str){
    vector<string> tokens = splitter(str,' ');
    string arg;
    vector<string> pos_arg,flag_arg;
    for(auto token : tokens){
      if(token.size()==0) continue;
      if(token[0]=='-'){
        if(token.size()>1 && token[1]=='-')
          arg = token.substr(2);
        else
          arg = token.substr(1);
        flag_arg.push_back(arg);
      }
      else{
        pos_arg.push_back(token);
      }
    }
    return {pos_arg,flag_arg};
  }
}

namespace command_utils{
  string cmd_to_string(COMMAND& cmd){
    if(cmd == COMMAND::GET) return "get";
    if(cmd == COMMAND::SET) return "set";
    if(cmd == COMMAND::CLEAR) return "clear";
    if(cmd == COMMAND::SIZE) return "size";
    if(cmd == COMMAND::HELP) return "help";
    if(cmd == COMMAND::EXIT) return "exit";
    return "unknown";
  }

  COMMAND string_to_cmd(string& cmd){
    transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c) {
      return tolower(c);
    });

    if(cmd == "get") return COMMAND::GET;
    if(cmd == "set") return COMMAND::SET;
    if(cmd == "clear") return COMMAND::CLEAR;
    if(cmd == "size") return COMMAND::SIZE;
    if(cmd == "help") return COMMAND::HELP;
    if(cmd == "exit") return COMMAND::EXIT;
    return COMMAND::UNKOWN;
  }
}

namespace serealize_utils{
  string response_buffer(CommandResult& cmd_result){
    return status_to_string(cmd_result.status) + "||" + cmd_result.message;
  }
}