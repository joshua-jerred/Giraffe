/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   software_version.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2024-12-20
/// @copyright  2024 (license to be defined)

#pragma once

#include <string>

namespace giraffe {

/// @brief Giraffe Software Version Structure
/// @details Semantic Versioning 2.0.0
struct SoftwareVersion {
  /// @brief Major version
  uint32_t major = 0;
  /// @brief Minor version
  uint32_t minor = 0;
  /// @brief Patch version
  uint32_t patch = 0;

  /// @brief Version stage, uses semantic versioning 2.0.0 point.
  /// @warning Due to now knowing what they all will be, the order of this enum
  /// is not guaranteed to be logical. _NUM_STAGES_ must be last.
  enum class Stage : uint8_t {
    UNKNOWN = 0,
    DEVELOPMENT, // development
    RELEASE,     // release
    _NUM_STAGES_ // The number of stages, must be last
  };

  /// @brief Stage of the software version
  Stage stage = Stage::UNKNOWN;

  /// @brief Indicates if the software was built within a clean git repository.
  bool clean = false;

  /// @brief Set the major, minor, and patch numbers from a string. Does not
  /// change the stage or clean values. This will throw a std::runtime_error if
  /// the version string is invalid, the major, minor, and patch values will be
  /// set to 0.
  /// @param version_string - The version string to parse.
  /// @exception std::runtime_error - Thrown if the version string is invalid.
  void setNumbersFromString(const std::string &version_string);

  /// @brief Get the major, minor, and patch numbers as a string in the format
  /// "major.minor.patch".
  /// @return The version string.
  std::string getNumbersAsString() const;

  /// @brief Set the stage from a string, ex: `development` or `release`. If the
  /// string is invalid, the stage will be set to `UNKNOWN` and no exception
  /// will be thrown.
  /// @param stage_string - The stage string to parse and set.
  /// @exception std::runtime_error - Thrown if the stage string is invalid.
  void setStageFromString(const std::string &stage_string);

  /// @brief Get the stage as a string, ex: `development`, `release`, or
  /// `unknown`.
  /// @return The stage string.
  std::string getStageAsString() const;

  /// @brief Set the major, minor, and patch numbers to 0, the stage to
  /// `UNKNOWN`, and clean to false.
  void zero() {
    major = 0;
    minor = 0;
    patch = 0;
    stage = Stage::UNKNOWN;
    clean = false;
  }
};

} // namespace giraffe