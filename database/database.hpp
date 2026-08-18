#pragma once
#include <iostream>
#include <unordered_map>
#include <string>


class Redis{
private:
  std::unordered_map<std::string,std::string> db;
public:
  Redis(){
    std::cout<<"Initializing redis database ...\n";
  }
  std::string get(std::string key){
    if(db.count(key)==0){
      return "Key not found";
    }
    return db[key];
  }
  void set(std::string key, std::string val){
    db[key]=val;
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