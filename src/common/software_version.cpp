/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   software_version.cpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2024-12-20
/// @copyright  2024 (license to be defined)

#include <regex>

#include "software_version.hpp"

/// @brief Regular expression for a semantic version *number* string of the
/// format "<major>.<minor>.<patch>".
static const std::string SEMANTIC_VERSION_NUMBER_REGEX =
    "^(\\d+)\\.(\\d+)\\.(\\d+)$";

namespace giraffe {

void SoftwareVersion::setNumbersFromString(const std::string &version_string) {
  auto zeroValues = [&]() {
    this->major = 0;
    this->minor = 0;
    this->patch = 0;
  };

  bool matched = false;
  try {
    // Parse with regex
    std::regex version_pattern(SEMANTIC_VERSION_NUMBER_REGEX);
    std::smatch version_match;
    matched = std::regex_match(version_string, version_match, version_pattern);

    // Set the numbers
    this->major = std::stoi(version_match[1]);
    this->minor = std::stoi(version_match[2]);
    this->patch = std::stoi(version_match[3]);
  } catch (const std::exception &e) {
    zeroValues();
    throw std::runtime_error(": " + std::string(e.what()));
  }

  if (!matched) {
    zeroValues();
    throw std::runtime_error("Invalid version string: " + version_string);
  }
}

std::string SoftwareVersion::getNumbersAsString() const {
  return std::to_string(this->major) + "." + std::to_string(this->minor) + "." +
         std::to_string(this->patch);
}

void SoftwareVersion::setStageFromString(const std::string &stage_string) {
  static_assert(static_cast<uint8_t>(Stage::_NUM_STAGES_) == 3,
                "Stage enum has changed, update this method");
  if (stage_string == "development") {
    this->stage = Stage::DEVELOPMENT;
  } else if (stage_string == "release") {
    this->stage = Stage::RELEASE;
  } else {
    this->stage = Stage::UNKNOWN;
    throw std::runtime_error("Invalid stage string: " + stage_string);
  }
}

std::string SoftwareVersion::getStageAsString() const {
  static_assert(static_cast<uint8_t>(Stage::_NUM_STAGES_) == 3,
                "Stage enum has changed, update this method");
  switch (this->stage) {
  case Stage::UNKNOWN:
    return "unknown";
  case Stage::DEVELOPMENT:
    return "development";
  case Stage::RELEASE:
    return "release";
  default:
    return "unknown";
  }
}

} // namespace giraffe