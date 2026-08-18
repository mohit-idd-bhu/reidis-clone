#include<iostream>
#include<vector>
#include<string>
#include "utils/utils.hpp"
#include "engine/engine.hpp"
#include "servers/cliserver/cliserver.hpp"
#include "servers/tcpserver/tcpserver.hpp"

using namespace std;

int main(){
  DatabaseEngine dbEngine;
  TcpServer tcpserver(dbEngine);
  tcpserver.start();
}