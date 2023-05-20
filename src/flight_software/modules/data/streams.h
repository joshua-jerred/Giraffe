/**
 * @file stream.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Generics for the stream.
 * @date 2023-05-15
 * @copyright Copyright (c) 2023
 */

#ifndef STREAM_H_
#define STREAM_H_

#include <iostream>
#include <mutex>
#include <queue>
#include <string>

#include "time_types.h"
#include "node.h"

namespace data {

struct BaseStreamPacket {
  node::Identification source = node::Identification::UNKNOWN;
  giraffe_time::TimePoint created_time = giraffe_time::TimePoint();

  void resetTime() {
    created_time = giraffe_time::TimePoint();
  }
};

template <class T>
class Stream {
 public:
  Stream() : packet_queue_() {
  }

  void addPacket(T packet) {
    stream_mutex_.lock();

    packet_queue_.push(packet);
    current_packets_++;
    total_packets_++;
    stream_mutex_.unlock();
  }

  bool getPacket(T& packet) {
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

  void reset() {
    std::lock_guard<std::mutex> lock(stream_mutex_);
    current_packets_ = 0;
    total_packets_ = 0;
    packet_queue_ = std::queue<T>();
  }

 private:
  std::mutex stream_mutex_ = std::mutex();
  std::queue<T> packet_queue_;

  int current_packets_ = 0;
  int total_packets_ = 0;
};

struct ErrorStreamPacket : public BaseStreamPacket {
  std::string code = "";
  std::string info = "";
};

class ErrorStream : public Stream<ErrorStreamPacket> {
 public:
  void addError(node::Identification source, std::string code, std::string info = "") {
    ErrorStreamPacket pkt;
    pkt.source = source;
    pkt.code = code;
    pkt.info = info;
    pkt.resetTime();

    addPacket(pkt);
  }
};

struct DataStreamPacket : public BaseStreamPacket {
  std::string identifier = "";
  std::string value = "";
};

class DataStream : public Stream<DataStreamPacket> {
 public:
  void addData(node::Identification source, std::string identifier, std::string value) {
    DataStreamPacket pkt;
    pkt.source = source;
    pkt.identifier = identifier;
    pkt.value = value;
    pkt.resetTime();

    addPacket(pkt);
  }
};

struct Streams {
  data::DataStream data_stream = data::DataStream();
  data::ErrorStream error_stream = data::ErrorStream();
};

}  // namespace data

/**
 * @todo print source
 */
std::ostream& operator<<(std::ostream& o, const data::ErrorStreamPacket& e);

#endif