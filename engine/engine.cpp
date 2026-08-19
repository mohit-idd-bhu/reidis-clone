#include<string>
#include<iostream>
#include<vector>
#include "engine.hpp"
#include "../utils/utils.hpp"
#include "../enums/command.hpp"
#include "../enums/commnd_result.hpp"
#include "../logs/aof_logger.cpp"
#include <chrono>


using namespace std;

CommandResult DatabaseEngine::execute_command(string& cmd_str){
  auto arg_pair = splitter_utils::get_args(cmd_str);
  auto pos_args = arg_pair.first;
  auto flag_args = arg_pair.second;
  CommandResult result;
  result.status=Status::OK;
  auto seq_time = chrono::system_clock::now();

  AOFLogger::instance().push(Event{seq_time,cmd_str});
  if(pos_args.size()==0){
    result.message="Unkown Command given\ntry help";
    result.status=Status::ERROR;
  }

  COMMAND cmd = command_utils::string_to_cmd(pos_args[0]);
  if(cmd==COMMAND::HELP){
    result.message="Redis usage help, type help to show again";
  }
  else if(cmd==COMMAND::PING){
    result.message="PONG";
  }
  else if(cmd==COMMAND::EXIT){
    result.status=Status::EXIT;
  }
  else if(cmd==COMMAND::GET && flag_args.size()==0 && pos_args.size()==2){
    string key = pos_args[1];
    result.message=DatabaseEngine::redis.get(key);
  }
  else if(cmd==COMMAND::SET && flag_args.size()==0 && pos_args.size()==3){
    string key = pos_args[1],value = pos_args[2];
    DatabaseEngine::redis.set(key,value);
  }
  else if(cmd==COMMAND::SIZE && flag_args.size()==0 && pos_args.size()==1){
    result.message=to_string(DatabaseEngine::redis.size());
  }
  else{
    result.message="Unkown command given\ntry help\n";
    result.status=Status::ERROR;
  }
  return result;
}