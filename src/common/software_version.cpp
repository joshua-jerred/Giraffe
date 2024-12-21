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

#include <BoosterSeat/string_utils.hpp>

#include "software_version.hpp"

/// @brief Regular expression for a semantic version *number* string of the
/// format "<major>.<minor>.<patch>[-<stage>]", where "-<stage>" is optional.
static const std::string SEMANTIC_VERSION_NUMBER_REGEX =
    "^(\\d+)\\.(\\d+)\\.(\\d+)(-[a-z]{1,10})?(-dev)?$";

namespace giraffe {

void SoftwareVersion::setVersionFromString(const std::string &version_string) {
  static_assert(static_cast<uint8_t>(Stage::_NUM_STAGES_) == 5,
                "Stage enum has changed, update this method");

  auto zeroValues = [&]() {
    this->major = 0;
    this->minor = 0;
    this->patch = 0;
    this->stage = Stage::UNKNOWN;
    this->clean = false;
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

    std::string stage_string_a = version_match[4].str();
    std::string stage_string_b = version_match[5].str();

    if (stage_string_a == "-dev" || stage_string_b == "-dev") {
      this->clean = false;
    } else {
      this->clean = true;
    }

    if (stage_string_a.empty() && stage_string_b.empty()) {
      this->stage = Stage::RELEASE;
    } else if (stage_string_a == "-alpha") {
      this->stage = Stage::ALPHA;
    } else if (stage_string_a == "-beta") {
      this->stage = Stage::BETA;
    } else if (stage_string_a == "-rc") {
      this->stage = Stage::RELEASE_CANDIDATE;
    } else {
      this->stage = Stage::UNKNOWN;
    }

  } catch (const std::exception &e) {
    zeroValues();
    throw std::runtime_error("except during regex or stoi with semver: " +
                             std::string(e.what()));
  }

  if (!matched) {
    zeroValues();
    throw std::runtime_error("Invalid version string: " + version_string);
  }
}

std::string SoftwareVersion::getVersionString() const {
  return std::to_string(this->major) + "." + std::to_string(this->minor) + "." +
         std::to_string(this->patch) +
         (stage == Stage::RELEASE ? "" : "-" + getStageAsString());
}

std::string SoftwareVersion::getNumbersAsString() const {
  return std::to_string(this->major) + "." + std::to_string(this->minor) + "." +
         std::to_string(this->patch);
}

std::string SoftwareVersion::getStageAsString() const {
  static_assert(static_cast<uint8_t>(Stage::_NUM_STAGES_) == 5,
                "Stage enum has changed, update this method");
  std::string stage_string;
  switch (this->stage) {
  case Stage::UNKNOWN:
    stage_string = "unknown";
    break;
  case Stage::ALPHA:
    stage_string = "alpha";
    break;
  case Stage::BETA:
    stage_string = "beta";
    break;
  case Stage::RELEASE_CANDIDATE:
    stage_string = "rc";
    break;
  case Stage::RELEASE:
    stage_string = "";
    break;
  default:
    stage_string = "error";
    break;
  }

  return stage_string + (this->clean ? "" : "-dev");
}

} // namespace giraffe