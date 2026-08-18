#pragma once
#include "../server.hpp"

class CliServer: public IServer{
public:
  CliServer(DatabaseEngine& db_):IServer(db_){}
  void start() override;
  void stop() override;
};