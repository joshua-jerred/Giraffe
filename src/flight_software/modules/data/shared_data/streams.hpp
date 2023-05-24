/**
 * @file stream.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Generics for the stream.
 * @date 2023-05-15
 * @copyright Copyright (c) 2023
 */

#ifndef STREAMS_HPP_
#define STREAMS_HPP_

#include <iostream>
#include <mutex>
#include <queue>
#include <string>

#include "data_ids.hpp"
#include "log_ids.hpp"
#include "node.h"
#include "time_types.h"

namespace data {

struct BaseStreamPacket {
  node::Identification source = node::Identification::UNKNOWN;
  giraffe_time::TimePoint created_time = giraffe_time::now();

  void resetTime() {
    created_time = giraffe_time::now();
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

struct LogPacket : public BaseStreamPacket {
  enum class Level { DEBUG, INFO, WARN, ERROR };
  Level level = Level::INFO;
  data::logId id = data::logId::Generic_unknown;
  std::string info = "";
};

class LogStream : public Stream<LogPacket> {
 public:
  void error(node::Identification source, data::logId error_id,
             std::string info = "") {
    LogPacket pkt;
    pkt.source = source;
    pkt.level = LogPacket::Level::ERROR;
    pkt.id = error_id;
    pkt.info = info;
    pkt.resetTime();

    addPacket(pkt);
  }

  void info(node::Identification source, std::string info = "",
            data::logId log_id = data::logId::Generic_info) {
    LogPacket pkt;
    pkt.source = source;
    pkt.level = LogPacket::Level::INFO;
    pkt.id = log_id;
    pkt.info = info;
    pkt.resetTime();

    addPacket(pkt);
  }

  void debug(node::Identification source, std::string info = "",
             data::logId log_id) {
    LogPacket pkt;
    pkt.source = source;
    pkt.level = LogPacket::Level::DEBUG;
    pkt.id = log_id;
    pkt.info = info;
    pkt.resetTime();

    addPacket(pkt);
  }
};

struct DataPacket : public BaseStreamPacket {
  // Generics
  data::dataId identifier = data::dataId::Generic_unknown;
  std::string value = "";

  // Extra (Uses so little space, not really a problem to include it
  // in all packets)
  node::Status node_status = node::Status::UNKNOWN;
};

class DataStream : public Stream<DataPacket> {
 public:
  void addData(node::Identification source, data::dataId data_id,
               std::string value) {
    DataPacket pkt;
    pkt.source = source;
    pkt.identifier = data_id;
    pkt.value = value;
    pkt.resetTime();

    addPacket(pkt);
  }
};
}  // namespace data

#endif