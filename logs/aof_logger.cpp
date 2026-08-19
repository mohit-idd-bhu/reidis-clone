#include "log_buffer.hpp"
#include "../enums/log_event.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <atomic>

class AOFLogger{
public:
  static AOFLogger& instance(){
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
    const auto timestamp = std::chrono::system_clock::to_time_t(e.sequence);
    std::tm local_timestamp{};
    #ifdef _WIN32
      localtime_s(&local_timestamp, &timestamp);
    #else
      localtime_r(&timestamp, &local_timestamp);
    #endif
      log_file_writer << '[' << std::put_time(&local_timestamp, "%Y-%m-%d %H:%M:%S")<< "] " << e.payload << '\n';
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