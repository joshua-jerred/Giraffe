/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   one_wire_interface.cpp
 * @brief  1-Wire interface (linux)
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-03
 * @copyright  2023 (license to be defined)
 */

#include <filesystem>
#include <fstream>
#include <sstream>

#include "one_wire_interface.hpp"

static const std::string kOneWireLocation = "/sys/bus/w1/devices/";

void OneWireInterface::setDeviceId(std::string device_id) {
  path_ = kOneWireLocation + device_id;
}

bool OneWireInterface::readW1SlaveFile(std::string &data) {
  std::ifstream file_;
  file_.open(path_ + "/w1_slave");
  if (!file_.is_open()) {
    return false;
  }
  std::stringstream ss;
  ss << file_.rdbuf();
  data = ss.str();
  return true;
}

bool OneWireInterface::readTemperatureFile(std::string &data) {
  std::ifstream file_;
  file_.open(path_ + "/temperature");
  if (!file_.is_open()) {
    return false;
  }
  std::getline(file_, data);
  return true;
}

bool OneWireInterface::deviceExists() {
  if (path_.empty()) {
    return false;
  }
  return std::filesystem::is_directory(path_);
}