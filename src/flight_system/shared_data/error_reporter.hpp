/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   error_reporter.hpp
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-09-17
 * @copyright  2024 (license to be defined)
 */

#pragma once

#include "shared_data.hpp"

namespace data {

/// @brief A simple class to unique errors once.
/// @details Allows for an error to be reported once unless it is cleared.
class ErrorReporter {
public:
  /// @brief Constructor.
  /// @param shared_data - Reference to shared data.
  /// @param source - The source of the error.
  /// @param error_id - The error id to report.
  ErrorReporter(SharedData &shared_data, node::Identification source,
                DiagnosticId error_id)
      : shared_data_(shared_data), source_id_{source}, error_id_(error_id) {
  }

  void reportError(std::string extra_info = "") {
    bool is_duplicate = extra_info == previous_extra_info_;

    // If the current error and extra info have already been reported, don't
    // report again. However, the error may have been cleared from the error
    // frame. If it's set but not active in the error frame, we should report
    // it again.
    if (is_set_ && is_duplicate &&
        shared_data_.frames.error_frame.isActive(error_id_)) {
      return;
    }

    shared_data_.streams.log.error(source_id_, error_id_, extra_info);
    previous_extra_info_ = extra_info;
    is_set_ = true;
  }

  void clearError() {
    is_set_ = false;
  }

private:
  bool is_set_ = false;

  SharedData &shared_data_;
  const node::Identification source_id_;
  const DiagnosticId error_id_;
  std::string previous_extra_info_ = "";
};

} // namespace data