#pragma once
#include "../engine/engine.hpp"

class IServer{
protected:
  DatabaseEngine& db;
public:
  IServer(DatabaseEngine& db_):db(db_){};
  virtual void start()=0;
  virtual void stop()=0;
};