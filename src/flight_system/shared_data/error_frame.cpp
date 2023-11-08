/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   error_frame.cpp
 * @brief  The implementation of the error frame.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-30
 * @copyright  2023 (license to be defined)
 */

#include "error_frame.hpp"
#include "giraffe_assert.hpp"

namespace data {

void ErrorFrame::addError(const data::LogPacket &packet) {
  giraffe_assert(packet.level == data::LogPacket::Level::ERROR);

  std::lock_guard<std::mutex> lock(error_frame_mutex_);
  total_errors_++;
  last_error_reported_ = BoosterSeat::clck::now();

  ErrorFrameItem current_in_frame{};

  // No error with this id in the frame, add it.
  if (!Frame::get(packet.id, current_in_frame)) {
    ErrorFrameItem new_item{};
    new_item.last_reported = packet.created_time;
    new_item.first_reported = packet.created_time;
    new_item.occurrences = 1;
    new_item.packet = packet;
    Frame::insert(packet.id, new_item);
    return;
  }

  // Error with this id in the frame, update it.
  current_in_frame.last_reported = packet.created_time;
  current_in_frame.occurrences++;
  current_in_frame.packet = packet;
  Frame::insert(packet.id, current_in_frame);
}

void ErrorFrame::clearError(DiagnosticId id) {
  std::lock_guard<std::mutex> lock(error_frame_mutex_);
  Frame::remove(id);
}

void ErrorFrame::clearAllErrors() {
  std::lock_guard<std::mutex> lock(error_frame_mutex_);
  Frame::clear();
}

int ErrorFrame::numTotalErrors() const {
  std::lock_guard<std::mutex> lock(error_frame_mutex_);
  return total_errors_;
}

int ErrorFrame::numActiveErrors() const {
  std::lock_guard<std::mutex> lock(error_frame_mutex_);
  return Frame::size();
}

bool ErrorFrame::getError(DiagnosticId id, ErrorFrameItem &item) const {
  std::lock_guard<std::mutex> lock(error_frame_mutex_);
  return Frame::get(id, item);
}

std::vector<DiagnosticId> ErrorFrame::getActiveErrorIds() const {
  std::lock_guard<std::mutex> lock(error_frame_mutex_);
  std::vector<DiagnosticId> error_ids{};

  for (auto &item : Frame::frame_) {
    error_ids.push_back(item.first);
  }

  return error_ids;
}

bool ErrorFrame::isActive(DiagnosticId id) const {
  std::lock_guard<std::mutex> lock(error_frame_mutex_);
  return Frame::in(id);
}

int ErrorFrame::numOccurrences(DiagnosticId id) const {
  std::lock_guard<std::mutex> lock(error_frame_mutex_);
  ErrorFrameItem item{};
  if (!Frame::get(id, item)) {
    return 0;
  }
  return item.occurrences;
}

BoosterSeat::clck::TimePoint ErrorFrame::timeOfLastErrorReported() const {
  std::lock_guard<std::mutex> lock(error_frame_mutex_);
  return last_error_reported_;
}

bool ErrorFrame::lastOccurrence(DiagnosticId id,
                                BoosterSeat::clck::TimePoint &time) const {
  std::lock_guard<std::mutex> lock(error_frame_mutex_);
  ErrorFrameItem item{};
  if (!Frame::get(id, item)) {
    return false;
  }
  time = item.last_reported;
  return true;
}

std::unordered_map<DiagnosticId, ErrorFrameItem>
ErrorFrame::getFullFrame() const {
  std::lock_guard<std::mutex> lock(error_frame_mutex_);
  return Frame::frame_;
}

} // namespace data