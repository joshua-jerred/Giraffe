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

#include "giraffe_diagnostic_ids.hpp"

class GiraffeException : public std::exception {
public:
  GiraffeException(const DiagnosticId diagnostic_id,
                   const std::string &info = "");

  DiagnosticId id() const throw();

  std::string info() const throw();

  const char *what() const throw() override;

private:
  DiagnosticId diagnostic_id_;
  std::string info_;
};

#endif // GIRAFFE_EXCEPTION_HPP_