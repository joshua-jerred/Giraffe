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
    ALPHA,             // alpha
    BETA,              // beta
    RELEASE_CANDIDATE, // rc
    RELEASE,           // release
    _NUM_STAGES_       // The number of stages, must be last
  };

  /// @brief Stage of the software version
  Stage stage = Stage::UNKNOWN;

  /// @brief Indicates if the software was built within a clean git repository.
  /// Labeled by the `-dev` suffix.
  bool clean = false;

  /// @brief Set the major, minor, patch, and stage numbers from a string in the
  /// format `major.minor.patch[-<stage>]`, where `-<stage>` is optional.
  /// @param version_string - The version string to parse.
  /// @exception std::runtime_error - Thrown if the version string is invalid.
  void setVersionFromString(const std::string &version_string);

  /// @brief Get the version as a string in the format
  /// `major.minor.patch[-<stage>]`, where `-<stage>` is optional.
  /// @return The version string.
  std::string getSemanticVersionString() const;

  /// @brief Get the major, minor, and patch numbers as a string in the format
  /// "major.minor.patch".
  /// @return The version string.
  std::string getNumbersAsString() const;

  /// @brief Set the stage from a string in the format `alpha`, `beta`, `rc`, or
  /// empty for release.
  /// @param stage_string - The stage string to parse.
  void setStageFromString(const std::string &stage_string);

  /// @brief Get the stage as a string, per versioning.md
  /// @return The stage string.
  std::string getStageAsString() const;

  /// @brief Get get the development build status (branch is dirty).
  /// @return \c true if the build is a development build, \c false otherwise.
  bool isDevelopmentBuild() const {
    if (stage == Stage::UNKNOWN) {
      return false;
    }

    return clean == false;
  }

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