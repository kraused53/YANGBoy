#include "Logger.h"

#include <iostream>
#include <chrono>

std::string get_date_time(void) {
  std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  std::string time_string(30, '\0');
  std::strftime(&time_string[0], time_string.size(), "%a %b %d, %Y at %I:%M:%S", std::localtime(&time));

  return time_string;
}

// Informational log - prints in green text
void Logger::Log( const std::string& message ) {
  return;
  std::string log = " INFO: [ " + get_date_time() + " ] - " + message;

  // Print log message to terminal
  std::cout << "\x1b[32m" << log << "\033[0m" << std::endl;
}

// Error log - prints in green text
void Logger::Err( const std::string& message ) {
  return;
  // Create new log entry
  std::string err = "ERROR: [ " + get_date_time() + " ] - " + message;

  std::cout << "\x1b[31m" << err << "\033[0m" << std::endl;
}