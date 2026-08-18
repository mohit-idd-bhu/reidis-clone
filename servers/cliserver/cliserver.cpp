#include "cliserver.hpp"

void CliServer::start(){
  std::string cmd;
  while(true){
    std::cout<<"root@redis# ";
    std::getline(std::cin,cmd);
    CommandResult cmd_result = IServer::db.execute_command(cmd);
    if(cmd_result.status == Status::EXIT)
      std::exit(0);
    if(cmd_result.message.size()>0)
      std::cout<<cmd_result.message<<'\n';
  }
}

void CliServer::stop(){}