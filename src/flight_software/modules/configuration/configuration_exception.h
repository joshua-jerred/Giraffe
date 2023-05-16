/**
 * @file configuration_exception.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Simple configuration exception.
 * @date 2023-05-15
 * @copyright Copyright (c) 2023
 */

#ifndef CONFIGURATION_EXCEPTION_H_
#define CONFIGURATION_EXCEPTION_H_

#include <exception>
#include <string>

class ConfigurationException : public std::exception {
 public:
  ConfigurationException(std::string message) : message_(message) {
  }
  const char* what() const throw() {
    return message_.c_str();
  }

 private:
  std::string message_;
};

#endif