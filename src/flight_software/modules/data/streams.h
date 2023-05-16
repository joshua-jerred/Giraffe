/**
 * @file stream.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Generics for the stream.
 * @date 2023-05-15
 * @copyright Copyright (c) 2023
 */

#ifndef STREAM_H_
#define STREAM_H_

#include <mutex>
#include <queue>
#include <string>

#include "time_types.h"

namespace data {
namespace streams {

enum class Source { NONE, CONFIGURATION_MODULE, DATA_MODULE };

struct BaseStreamPacket {
  data::streams::Source source = data::streams::Source::NONE;
  giraffe_time::TimePoint created_time = giraffe_time::TimePoint();
};

template <class T>
class Stream {
 public:
  void addPacket(T packet) {
    std::lock_guard<std::mutex> lock(stream_mutex_);

    packet_queue_.push(packet);
    current_packets_++;
    total_packets_++;
  }

  bool getPacket(T &packet) {
    std::lock_guard<std::mutex> lock(stream_mutex_);

    if (packet_queue_.empty()) {
      return false;
    }

    packet = packet_queue_.front();
    packet_queue_.pop();
    current_packets_--;

    return true;
  }

  int getNumPackets() {
    std::lock_guard<std::mutex> lock(stream_mutex_);
    return current_packets_;
  }

  int getTotalPackets() {
    std::lock_guard<std::mutex> lock(stream_mutex_);
    return total_packets_;
  }

 private:
  std::mutex stream_mutex_;
  std::queue<T> packet_queue_;

  int current_packets_ = 0;
  int total_packets_ = 0;
};

struct ErrorStreamPacket : public BaseStreamPacket {
  std::string code;
  std::string info;
};

typedef Stream<ErrorStreamPacket> ErrorStream;

}  // namespace streams
}  // namespace data

#endif