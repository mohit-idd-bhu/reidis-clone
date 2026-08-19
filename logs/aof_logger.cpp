#include "log_buffer.hpp"
#include "../enums/log_event.hpp"
#include <iostream>
#include <fstream>
#include <thread>

class AOFLogger
{
public:
  static AOFLogger &instance(){
    static AOFLogger logger;
    return logger;
  }

  void push(Event e){
    log_buff.try_push(e);
  }

  void consume(){
    while (running_){
      std::this_thread::sleep_for(std::chrono::seconds(1));
      Event e;
      while (log_buff.try_pop(e)){
        write(e);
      }
      log_file_writer.flush();
    }

    // Drain anything remaining.
    Event e;
    while (log_buff.try_pop(e)){
      write(e);
    }
    log_file_writer.flush();
  }

  void write(Event e){
    log_file_writer << e.payload << '\n';
  }
private:
  AOFLogger() : log_file_writer("aof.log", std::ios::app), worker(&AOFLogger::consume,this){}
  ~AOFLogger(){
    running_=false;
    if(worker.joinable()){
      worker.join();
    }
    log_file_writer.flush();
  }
  LogBuffer<Event, 4096> log_buff;
  std::ofstream log_file_writer;
  std::atomic<bool> running_{true};
  std::thread worker;
};