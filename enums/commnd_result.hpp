#pragma once
#include <string>

enum class Status{
  OK,
  ERROR,
  EXIT
};

inline std::string status_to_string(Status status) {
  switch (status) {
    case Status::OK:
      return "OK";
    case Status::ERROR:
      return "ERROR";
    case Status::EXIT:
      return "EXIT";
    default:
      return "UNKNOWN";
  }
}

struct CommandResult{

  Status status;
  std::string message;

  CommandResult(){}
  CommandResult(Status status_, std::string message_):status(status_),message(message_){ }

  std::string status_to_string() const {
    return ::status_to_string(status);
  }
};

