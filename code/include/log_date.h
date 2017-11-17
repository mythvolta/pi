// Include the date and time when outputting a string

#ifndef LOG_DATE_H
#define LOG_DATE_H

#include <iostream>
#include <iomanip>
#include <string>

// Get the date and time
std::string date_time() {
  char buffer[32];
  auto t = std::time(nullptr);
  auto foo = *std::localtime(&t);
  std::strftime(buffer, 32, "[%Y-%m-%d %H:%M:%S] ", &foo);
  return (std::string)buffer;
}

#endif // LOG_DATE_H
