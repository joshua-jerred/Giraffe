#ifndef DATA_MODULE_H_
#define DATA_MODULE_H_

#include <functional>

#include "data_log.hpp"
#include "influxdb.hpp"
#include "module.hpp"

namespace modules {

class DataModule : public Module {
public:
  DataModule(data::SharedData &, cfg::Configuration &);
  ~DataModule() override = default;

private:
  /**
   * @brief Startup override for the data module.
   * @details Currently doesn't do anything unique.
   */
  void startup() override;

  /**
   * @brief Loop override for the data module.
   * @details Calls all stream parsing, file/db logging functions.
   */
  void loop() override;

  /**
   * @brief Shutdown override for the data module.
   * @details As this is one of the last modules to shutdown, it
   * will do a final call to parsing functions.
   */
  void shutdown() override;

  /**
   * @brief Used to process and commands sent to the data module.
   * @todo This function is not implemented yet.
   */
  void processCommand(const cmd::Command &);

  /**
   * @brief Used to process shared streams.
   *
   * @details This function will process all packets in a stream and
   * update the streams stats. If data packets are added to the stream
   * while this function is running, they will be processed on the next call.
   *
   * @tparam T - The type of stream packet.
   * @param stream - The stream to process.
   * @param stats - The stream stats to update
   * @param parse_function - The function to call on each packet.
   */
  template <typename T>
  void processStream(data::Stream<T> &stream,
                     data::blocks::StreamsStats::StreamStats &stats,
                     std::function<void(const T &)> parse_function) {
    // Get the number of packets currently in the stream and the total number
    // that have gone through the stream.
    int packet_count = stream.getNumPackets();
    int total_packets = stream.getTotalPackets();

    T packet;
    bool first = true;
    int delay_ms = 0; // used to calculate the processing delay
    for (int i = 0; i < packet_count; i++) {
      bool got_packet = stream.getPacket(packet);
      if (!got_packet) { // this should never happen
        return;
      }
      // Calculate the delay between now and when the oldest packet was created
      if (first) {
        delay_ms = BoosterSeat::clck::millisecondsElapsed(packet.created_time);
        first = false;
      }

      // Pass to the packet parsing function
      parse_function(packet);
    }

    // Update the stream stats for this stream
    stats.current_packets = packet_count;
    stats.total_packets = total_packets;
    stats.processing_delay_ms = delay_ms;
  }

  /**
   * @brief Process all shared streams.
   */
  void processAllStreams();

  /**
   * @brief Process all data stream packets and log them (if enabled)
   *
   * @details This function is called for each packet in the data stream, it
   * will route them to the appropriate parsing function. This includes
   * influxdb/datalog middleware if enabled. After all packets are processed,
   * the stream stats will be updated.
   *
   * @defgroup DataStreamParsing
   * @{
   */
  void processDataPacket(const data::DataPacket &packet);

  /**
   * @todo This function is not implemented yet.
   * @param packet
   */
  void parseGeneralDataPacket(const data::DataPacket &packet);

  /**
   * @brief Parses the status of modules and updates the data block.
   * @param packet - The status data packet to parse.
   */
  void parseStatusDataPacket(const data::DataPacket &packet);

  /**
   * @brief Parses all data from extensions and updates the data frame.
   * @param packet
   */
  void parseExtensionDataPacket(const data::DataPacket &packet);
  /**
   * @}
   */

  /**
   * @brief Process all packets from the log stream.
   */
  void processLogPacket(const data::LogPacket &packet);

  /**
   * @brief Process all packets from the GPSFrame stream.
   */
  void processGpsFramePacket(const data::GpsFramePacket &packet);

  /**
   * @brief Process all packets from the IMUFrame stream.
   */
  void processImuFramePacket(const data::ImuFramePacket &packet);

  data_middleware::DataLog data_log_;
  data_middleware::InfluxDb influxdb_;

  bool data_file_enabled_ = false;
  bool log_file_enabled_ = false;
  bool influxdb_enabled_ = false;

  cfg::gEnum::LogStrategy data_file_logging_strategy_ =
      cfg::gEnum::LogStrategy::INTERVAL;
};

} // namespace modules

#endif