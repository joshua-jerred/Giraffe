/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   file_formatting.hpp
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

private:
  /**
   * @brief Used to set up the frame json structure.
   * @param frame
   */
  void setupFrameStructure(json &frame);
  void addComponent(DataFrameComponent component, json &frame);

  cfg::Configuration &config_;
  data::SharedData &shared_data_;
};

}; // namespace data_middleware

#endif /* FILE_FORMATTING_HPP_ */
