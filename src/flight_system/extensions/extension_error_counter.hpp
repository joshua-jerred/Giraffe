/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   extension_error_counter.hpp
 * @brief  Error counter for extensions
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-02
 * @copyright  2023 (license to be defined)
 */

#ifndef EXTENSION_ERROR_COUNTER_HPP_
#define EXTENSION_ERROR_COUNTER_HPP_

#include "giraffe_diagnostic_ids.hpp"

namespace extension {

/**
 * @brief Helper for counting the number of errors that have occurred.
 * @details This is used to set a 'retry' limit while still being able to report
 * the error. One drawback is that only the last error is reported.
 */
class ErrorCounter {
public:
  /**
   * @brief Construct a new Error Counter
   *
   * @param error_limit
   */
  ErrorCounter(uint32_t error_limit);
  ~ErrorCounter() = default;

  /**
   * @brief Report and error and increment the error count.
   * @param error_id - The error id to report
   */
  void error(DiagnosticId error_id) {
    ++count_;
    error_id_ = error_id;
  }

  /**
   * @brief Reset the error counter.
   */
  void reset() {
    count_ = 0;
    error_id_ = DiagnosticId::GENERIC_unknown;
  }

  /**
   * @brief Return the current error count.
   * @return uint32_t - The current error count.
   */
  uint32_t count() const {
    return count_;
  }

  /**
   * @brief Return true if the error count is at the limit.
   *
   * @return true - Reached the limit.
   * @return false - Not at the limit.
   */
  bool atLimit() const {
    return count_ >= error_limit;
  }

  /**
   * @brief Return the last error id.
   * @return DiagnosticId - The last error id.
   */
  DiagnosticId getLastErrorId() const {
    return error_id_;
  }

private:
  const uint32_t error_limit;
  DiagnosticId error_id_ = DiagnosticId::GENERIC_unknown;
};

} // namespace extension
#endif /* EXTENSION_ERROR_COUNTER_HPP_ */