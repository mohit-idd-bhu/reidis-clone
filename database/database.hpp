#pragma once
#include <iostream>
#include <unordered_map>
#include <string>
#include <chrono>
#include "value.hpp"

using namespace std::chrono;

class Redis{
private:
  std::unordered_map<std::string,Value> db;
  size_t ttl;
public:
  Redis(size_t ttl_):ttl(ttl_){
    std::cout<<"Initializing redis database with ttl "<<ttl<<'\n';
  }
  std::string get(std::string key){
    if(db.count(key)==0){
      return "Key not found";
    }
    auto now_time = system_clock::now();
    Value val = db[key];
    if(val.expiry_time < now_time){
      return "Key not found";
    }
    return db[key].val;
  }
  void set(std::string key, std::string val){
    auto expiry_time = system_clock::now()+seconds(ttl);
    db[key] = Value{val,expiry_time};
  }
  void del(std::string key){
    db.erase(key);
  }
  int size(){
    return db.size();
  }
  void clear(){
    db.clear();
  }
};