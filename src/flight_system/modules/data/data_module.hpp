/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   data_module.hpp
 * @brief  The data module header file.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-30
 * @copyright  2023 (license to be defined)
 */

#ifndef DATA_MODULE_HPP_
#define DATA_MODULE_HPP_

#include <functional>

#include "data_log.hpp"
#include "influxdb.hpp"
#include "module.hpp"

namespace modules {

/**
 * @brief  A major module of the Giraffe Flight System that handles data flow
 * through the system.
 */
class DataModule : public Module {
public:
  /**
   * @brief Constructor for the data module.
   * @param shared_data - System wide shared data object.
   * @param configuration - System wide configuration object.
   */
  DataModule(data::SharedData &shared_data, cfg::Configuration &configuration);
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
   * @brief Processes all of the data streams and updates the data block that
   * contains the stream stats.
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
   * @defgroup DataStreamParsing Data Stream Parsing
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
   * @brief Process a data packet with ID data::DataId::CAMERA_newImagePath
   * @details Validates that the packet contains a file path that exists and
   * updates the data block.
   */
  void parseCameraNewImageDataPacket(const data::DataPacket &packet);

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

  /**
   * @brief The data log object, used to log data and errors to files.
   */
  data_middleware::DataLog data_log_;

  /**
   * @brief Used to access the influxdb database.
   */
  data_middleware::InfluxDb influxdb_;

  /**
   * @brief True if data file logging is enabled.
   * @details This is updated every loop based on the configuration.
   */
  bool data_file_enabled_ = false;

  /**
   * @brief True if error file logging is enabled.
   * @details This is updated every loop based on the configuration.
   */
  bool log_file_enabled_ = false;

  /**
   * @brief True if influxdb logging is enabled.
   * @details This is updated every loop based on the configuration.
   */
  bool influxdb_enabled_ = false;

  int influxdb_write_rate_ms_ = 1000;

  /**
   * @brief User to determine if shared 'log_container' needs to be updated so
   * the console module can print the log.
   */
  bool console_module_enabled_ = false;

  /**
   * @brief The current data file logging strategy.
   * @details This is updated every loop based on the configuration.
   */
  cfg::gEnum::LogStrategy data_file_logging_strategy_ =
      cfg::gEnum::LogStrategy::INTERVAL;

  /**
   * @brief The current error file logging strategy.
   * @details This is updated every loop based on the configuration.
   */
  cfg::gEnum::ErrorLogStrategy error_file_logging_strategy_ =
      cfg::gEnum::ErrorLogStrategy::ALL;
};

} // namespace modules

#endif