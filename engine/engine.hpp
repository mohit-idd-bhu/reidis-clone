#pragma once
#include <string>
#include <unistd.h>
#include "../database/database.hpp"
#include "../enums/commnd_result.hpp"

class DatabaseEngine{
private:
  Redis redis;
public:
  DatabaseEngine(): redis(){
    std::cout<<"Initializing database with pid "<<getpid()<<'\n';
  }
  CommandResult execute_command(std::string& cmd_str);
};