/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   giraffe_exception.hpp
 * @brief  Common exception for Giraffe.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-10
 * @copyright  2023 (license to be defined)
 */

#ifndef GIRAFFE_EXCEPTION_HPP_
#define GIRAFFE_EXCEPTION_HPP_

#include <exception>
#include <string>

namespace common {

class Exception : public std::exception {
public:
  Exception(const std::string &error_code,
            const std::string &info = "")
      : error_code_(error_code), info_(info),
        message_(error_code + " : " + info) {
  }

  const char *what() const throw() {
    return message_.c_str();
  }

  std::string error_code() const throw() {
    return error_code_;
  }

  std::string info() const throw() {
    return info_.c_str();
  }

private:
  std::string error_code_;
  std::string info_;
  std::string message_;
};

} // namespace common

#endif // GIRAFFE_EXCEPTION_HPP_