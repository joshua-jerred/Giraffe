/**
 * @file configuration_exception.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Generic GFS Exception
 * @date 2023-05-15
 * @copyright Copyright (c) 2023
 */

#ifndef CONFIGURATION_EXCEPTION_H_
#define CONFIGURATION_EXCEPTION_H_

#include <exception>
#include <string>

class GfsException : public std::exception {
 public:
  GfsException(const std::string &source, const std::string &error_code,
               const std::string &info = "")
      : source_(source), error_code_(error_code), info_(info),
      message_(source + "_" + error_code + " : " + info) {
  }

  const char *what() const throw() {
    return message_.c_str();
  }

  std::string error_string() const throw() {
    return source_ + "_" + error_code_;
  }

  std::string info() const throw() {
    return info_.c_str();
  }

 private:
  std::string source_;
  std::string error_code_;
  std::string info_;
  std::string message_;
};

#endif