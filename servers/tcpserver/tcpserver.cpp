#include "tcpserver.hpp"
#include <unistd.h>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../../utils/utils.hpp"

void handle_client(int client_socket, DatabaseEngine& db_engine)
{
  std::cout << "Client connected on thread "<< std::this_thread::get_id() << '\n';
  char buffer[1024];
  while(1){
    ssize_t bytes = recv(client_socket, buffer, sizeof(buffer)-1, 0);

    if (bytes > 0){
      buffer[bytes]='\0';
      std::string message(buffer);
      CommandResult cmd_result = db_engine.execute_command(message);
      std::string response_message = serealize_utils::response_buffer(cmd_result);
      ::send(client_socket, response_message.c_str(), response_message.size(), 0);
    }
    else{
      std::cout << "Client disconnected\n";
      break;
    }
  }
  close(client_socket);
}

void TcpServer::start(){
  int server_socket = ::socket(AF_INET,SOCK_STREAM,0);
  if(server_socket < 0){
    std::cout<<"Server not connected\n";
    return;
  }
  sockaddr_in server{};

  server.sin_family = AF_INET;
  server.sin_port = htons(8080);
  
  inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

  if(::bind(server_socket, reinterpret_cast<sockaddr*>(&server), sizeof(server)) < 0){
    perror("bind");
    ::close(server_socket);
    return;
  }
  if(::listen(server_socket,5)<0){
    perror("listen");
    ::close(server_socket);
    return;
  }

  std::cout<<"Listening on port 8080\n";
  DatabaseEngine db_engine = IServer::db;

  while(true){
    int client_socket = ::accept(server_socket,nullptr,nullptr);
    if(client_socket < 0){
      continue;
    }
    std::thread(handle_client,client_socket,std::ref(db_engine)).detach();
  }
  ::close(server_socket);
}

void TcpServer::stop(){}