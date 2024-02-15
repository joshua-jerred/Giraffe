/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   giraffe_exception.cpp
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-01
 * @copyright  2023 (license to be defined)
 */

#include "giraffe_diagnostics.hpp"

template <typename IntType>
std::string hexString(IntType w, size_t hex_length = sizeof(IntType) << 1) {
  static const char *digits = "0123456789ABCDEF";
  std::string out(hex_length, '0');
  for (size_t i = 0, j = (hex_length - 1) * 4; i < hex_length; ++i, j -= 4)
    out[i] = digits[(w >> j) & 0x0f];
  return out;
}

GiraffeException::GiraffeException(const DiagnosticId diagnostic_id,
                                   const std::string &info)
    : diagnostic_id_(diagnostic_id), info_(info) {
}

DiagnosticId GiraffeException::id() const throw() {
  return diagnostic_id_;
}

std::string GiraffeException::info() const throw() {
  return info_.c_str();
}

const char *GiraffeException::what() const throw() {
  static const std::string what =
      hexString(static_cast<uint16_t>(diagnostic_id_), 4) + ":" +
      K_DIAGNOSTIC_ID_STRING_MAP.at(diagnostic_id_) +
      (info_.empty() ? "" : " - ") + info_;
  return what.c_str();
}