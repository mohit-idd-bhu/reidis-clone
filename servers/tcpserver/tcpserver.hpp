#pragma once
#include "../server.hpp"

class TcpServer: public IServer{
public:
  TcpServer(DatabaseEngine& db_):IServer(db_){}
  void start() override;
  void stop() override;
};