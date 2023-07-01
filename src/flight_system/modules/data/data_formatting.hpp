/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   data_formatting.hpp
 * @brief  This file contains the DataFormatter class, which is used to
 * format data for the data and log files.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-08
 * @copyright  2023 (license to be defined)
 */

#ifndef DATA_FORMATTING_HPP_
#define DATA_FORMATTING_HPP_

#include "configuration.hpp"
#include "shared_data.hpp"
#include <nlohmann/json.hpp>

namespace data_middleware {
typedef nlohmann::ordered_json json;

enum class DataFrameComponent {
  MODULES_STATUSES,
  STREAM_STATS,
  SERVER_STATS,
  SYSTEM_INFO,
  ENVIRONMENTAL_DATA
};

class DataFormatter {
public:
  DataFormatter(cfg::Configuration &config, data::SharedData &shared_data)
      : config_(config), shared_data_(shared_data) {
  }
  ~DataFormatter() = default;

  std::string fullFrame();
  std::string partialFrame(std::vector<DataFrameComponent> components);
  std::string dataPacketToJsonString(const data::DataPacket &packet) const;

  std::string fullErrorFrame();
  std::string logPacketToJsonString(const data::LogPacket &packet) const;

private:
  /**
   * @brief Used to set up the frame json structure.
   * @param frame - The json object to set up.
   * @param body_field - The name of the field that will contain the body.
   */
  void setupFrameStructure(json &frame, const std::string &body_field);
  void addComponent(DataFrameComponent component, json &frame);

  /**
   * @brief Generates a timestamp based off of the current configuration.
   * @details Uses either UTC or local time, either generates HH:MM:SS or
   * DD/MM/YYYY_HH:MM:SS. 24 hour time is used.
   *
   * @param time_point - The time point to generate the timestamp from (defaults
   * to now)
   * @return std::string - The generated timestamp
   */
  std::string generateTimestamp(const BoosterSeat::clck::TimePoint time_point =
                                    BoosterSeat::clck::now()) const;

  json fullFrameLogPacketToJson(const data::ErrorFrameItem &item) const;

  cfg::Configuration &config_;
  data::SharedData &shared_data_;
};

}; // namespace data_middleware

#endif /* FILE_FORMATTING_HPP_ */
