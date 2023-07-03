/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   streams.hpp
 * @brief  The header file for the data streams.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-30
 * @copyright  2023 (license to be defined)
 */

#ifndef STREAMS_HPP_
#define STREAMS_HPP_

#include <atomic>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>

#include <BoosterSeat/clock.hpp>
#include <BoosterSeat/exception.hpp>

#include "data_ids.hpp"
#include "log_ids.hpp"
#include "node.hpp"
#include "positional.hpp"

namespace data {

struct BaseStreamPacket {
  node::Identification source = node::Identification::UNKNOWN;
  std::string secondary_identifier = "";
  BoosterSeat::clck::TimePoint created_time = BoosterSeat::clck::now();

  void resetTime() {
    created_time = BoosterSeat::clck::now();
  }
};

template <class T> class Stream {
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

  int getNumPackets() { // not atomic as this is used for stream processing
    std::lock_guard<std::mutex> lock(stream_mutex_);
    return current_packets_;
  }

  int getTotalPackets() { // atomic
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
  std::atomic<int> total_packets_ = 0;
};

struct LogPacket : public BaseStreamPacket {
  enum class Level : int { DEBUG = 0, INFO = 1, WARN = 2, ERROR = 3 };
  Level level = Level::INFO;
  DiagnosticId id = DiagnosticId::GENERIC_unknown;
  std::string info = "";
};

class LogStream : public Stream<LogPacket> {
public:
  void error(node::Identification source, DiagnosticId error_id,
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
            DiagnosticId log_id = DiagnosticId::GENERIC_info) {
    LogPacket pkt;
    pkt.source = source;
    pkt.level = LogPacket::Level::INFO;
    pkt.id = log_id;
    pkt.info = info;
    pkt.resetTime();

    addPacket(pkt);
  }

  void debug(node::Identification source, std::string info = "",
             DiagnosticId log_id = DiagnosticId::GENERIC_debug) {
    LogPacket pkt;
    pkt.source = source;
    pkt.level = LogPacket::Level::DEBUG;
    pkt.id = log_id;
    pkt.info = info;
    pkt.resetTime();

    addPacket(pkt);
  }

  void errorBoosterSeatException(const node::Identification source,
                                 const DiagnosticId error_id,
                                 const BoosterSeat::BoosterSeatException &e) {
    LogPacket pkt;
    pkt.source = source;
    pkt.level = LogPacket::Level::ERROR;
    pkt.id = error_id;
    pkt.info = "Errno: " + std::to_string(e.errorNumber());
    pkt.resetTime();

    addPacket(pkt);
  }

  void errorStdException(const node::Identification source,
                         const DiagnosticId error_id, const std::exception &e) {
    LogPacket pkt;
    pkt.source = source;
    pkt.level = LogPacket::Level::ERROR;
    pkt.id = error_id;
    pkt.info = "what: " + std::string(e.what());
    pkt.resetTime();

    addPacket(pkt);
  }
};

struct DataPacket : public BaseStreamPacket {
  enum class Type { GENERIC, STATUS };

  // Generics
  data::DataPacket::Type type = data::DataPacket::Type::GENERIC;
  data::DataId identifier = data::DataId::GENERIC_unknown;
  std::string value = ""; // generic data

  // Extra (Uses so little space, not really a problem to include it
  // in all packets)
  node::Status node_status = node::Status::UNKNOWN;
};

class DataStream : public Stream<DataPacket> {
public:
  void addData(node::Identification source, data::DataId data_id,
               std::string value) {
    DataPacket pkt;
    pkt.source = source;
    pkt.identifier = data_id;
    pkt.value = value;
    pkt.resetTime();

    addPacket(pkt);
  }
  void reportStatus(
      node::Identification status_to_update, node::Status status,
      data::DataId extra_identifier = data::DataId::MODULE_statusUpdate) {
    DataPacket pkt;
    pkt.source = status_to_update;
    pkt.type = DataPacket::Type::STATUS;
    pkt.node_status = status;
    pkt.identifier = extra_identifier;
    pkt.resetTime();

    addPacket(pkt);
  }
};

struct GpsFramePacket : public BaseStreamPacket {
  data::GpsFrame frame{};
};

class GpsFrameStream : public Stream<GpsFramePacket> {
public:
  void addFrame(node::Identification source, std::string secondary_id,
                data::GpsFrame frame) {
    GpsFramePacket pkt{source, secondary_id, BoosterSeat::clck::now(), frame};
    addPacket(pkt);
  }
};

struct ImuFramePacket : public BaseStreamPacket {
  data::ImuFrame frame{};
};

class ImuFrameStream : public Stream<ImuFramePacket> {
public:
  void addFrame(node::Identification source, std::string secondary_id,
                data::ImuFrame frame) {
    ImuFramePacket pkt{source, secondary_id, BoosterSeat::clck::now(), frame};
    addPacket(pkt);
  }
};

} // namespace data

#endif