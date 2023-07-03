/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   error_frame.hpp
 * @brief  The error frame.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-30
 * @copyright  2023 (license to be defined)
 */

#ifndef ERROR_FRAME_HPP_
#define ERROR_FRAME_HPP_

#include <BoosterSeat/clock.hpp>

#include "frame.hpp"
#include "log_ids.hpp"
#include "streams.hpp"

namespace data {

/**
 * @brief A struct to store information about an error in the error frame.
 */
struct ErrorFrameItem {
  BoosterSeat::clck::TimePoint last_reported{};
  BoosterSeat::clck::TimePoint first_reported{};
  int occurrences = 0;

  data::LogPacket packet{};
};

/**
 * @brief The error frame.
 * @details Used to keep track of errors that have occurred in the system.
 * Specifically, the number of occurrences of each error along with the first
 * and last time the error was reported.
 */
class ErrorFrame : private Frame<DiagnosticId, ErrorFrameItem> {
public:
  ErrorFrame() = default;
  ~ErrorFrame() = default;

  /**
   * @brief Add an error (LogPacket) to the error frame.
   * @details If the error already exists, it will be updated, otherwise it will
   * be added to the frame.
   * @param packet - The LogPacket to add.
   */
  void addError(const data::LogPacket &packet);

  /**
   * @brief Clear an individual error from the frame by id.
   * @param id - The id of the error to clear.
   */
  void clearError(DiagnosticId id);

  /**
   * @brief Clear all errors from the error frame.
   */
  void clearAllErrors();

  /**
   * @brief The total number of errors that have been added to the frame.
   * This is not the number of active errors, but the total number of errors
   * that have been added to the frame.
   * @return int - The total number of errors that have been added to the frame.
   */
  int numTotalErrors() const;

  /**
   * @brief The number of active errors in the frame.
   * @return int - The number of active errors in the frame.
   */
  int numActiveErrors() const;

  /**
   * @brief Returns the error with the given id if it exists in the frame.
   * @details If the error does not exist in the frame, the item will be
   * left unmodified and false will be returned. Otherwise, the item will be
   * populated with the error info and true will be returned.
   * @param id - The id of the error to get.
   * @param item (out) - The item to populate with the error info.
   * @return true - The error was found in the frame.
   * @return false - The error was not found in the frame.
   */
  bool getError(DiagnosticId id, ErrorFrameItem &item) const;

  /**
   * @brief Get all active error ids that are currently in the frame.
   * @return std::vector<DiagnosticId> - A vector of all active error ids.
   */
  std::vector<DiagnosticId> getActiveErrorIds() const;

  /**
   * @brief Returns true if the error is active in the frame.
   * @param id - The id of the error to check.
   * @return true - The error is active in the frame.
   * @return false - The error is not active in the frame.
   */
  bool isActive(DiagnosticId id) const;

  /**
   * @brief Returns the number of occurrences of an error. If the error is not
   * in the frame, 0 is returned.
   * @param id - The id of the error to check.
   * @return int - The number of occurrences of the error.
   */
  int numOccurrences(DiagnosticId id) const;

  /**
   * @brief Returns a time point representing when the last time was that an
   * error was added to the frame.
   *
   * @return BoosterSeat::clck::TimePoint - The time the last error was added.
   */
  BoosterSeat::clck::TimePoint timeOfLastErrorReported() const;

  /**
   * @brief Returns a time point of when the last time that a specified error
   * was reported. If the error is not in the frame, false is returned,
   * otherwise true.
   * @param id - The id of the error to check.
   * @param time (out) - The time point of when the error was last reported.
   * @return true - The error was found in the frame.
   * @return false - The error was not found in the frame.
   */
  bool lastOccurrence(DiagnosticId id,
                      BoosterSeat::clck::TimePoint &time) const;

  /**
   * @brief Get the full error frame.
   * @return std::unordered_map<DiagnosticId, ErrorFrameItem> - The error frame.
   */
  std::unordered_map<DiagnosticId, ErrorFrameItem> getFullFrame() const;

private:
  /**
   * @brief Mutex for the error frame specifically.
   */
  mutable std::mutex error_frame_mutex_{};

  /**
   * @brief The total number of errors in the frame.
   */
  int total_errors_ = 0;

  /**
   * @brief The time of the last error reported.
   */
  BoosterSeat::clck::TimePoint last_error_reported_{};
};

} // namespace data

#endif /* ERROR_FRAME_HPP_ */