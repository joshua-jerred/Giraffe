#include "influxdb.hpp"
#include <curl_utility.hpp>
#include <iostream>

#include "blocks.hpp"
#include "to_string.hpp"

using namespace data_middleware;

inline std::string qte(const std::string input_str) {
  return "\"" + input_str + "\"";
}
inline std::string getUnixTime(std::time_t time) {
  return std::to_string(time);
}

void InfluxDb::logDataPacket(const data::DataPacket &packet) {
  if (packet.type == data::DataPacket::Type::STATUS) {
    /**
     * @todo Implement status packet logging to InfluxDB
     */
    return;
  }

  std::cout << "NOT STATUS" << std::endl;

  // Generate the body
  InfluxLine line;
  line.measurement = "DataPacket";
  line.tags["source"] = util::to_string(packet.source);
  line.tags["secondary_identifier"] = packet.secondary_identifier;
  line.fields["value"] = qte(packet.value);

  std::cout << "influx log data packet\n" << std::endl;
  std::cout << line.getString() << std::endl;

  influx_lines_.push_back(line);
}

void InfluxDb::logLogPacket(const data::LogPacket &packet) {

  InfluxLine line;
  line.measurement = "LogPacket";

  // tags
  line.tags["source"] = qte(util::to_string(packet.source));
  line.tags["sid"] = qte(packet.secondary_identifier);
  line.tags["lvl"] = qte(util::to_string(packet.level));

  // fields
  line.fields["diag_id"] = qte(util::to_string(packet.id));
  line.fields["msg"] = qte(packet.info);

  // timestamp
  line.timestamp = std::chrono::system_clock::to_time_t(packet.created_time);

  influx_lines_.push_back(line);
}

void InfluxDb::writeFrames() {
  if (!frame_timer_.isDone()) {
    return;
  }
  frame_timer_.setTimeout(config_.data_module_influxdb.getDataDumpInterval());
  frame_timer_.reset();

  auto stream_stats_json = shared_data_.blocks.stream_stats.get().toJson();
  for (auto &[key, value] : stream_stats_json.items()) {
    InfluxLine line;
    line.measurement = "StreamStats";
    line.fields[key] = value.dump();
    influx_lines_.push_back(line);
  }

  std::string all_lines = "";
  for (auto &line : influx_lines_) {
    all_lines += line.getString() + "\n";
  }
  influx_lines_.clear();

  const std::string full_path = url_ + "api/v2/write?org=" + org_ +
                                "&bucket=" + data_bucket_ + "&precision=ns";
  std::vector<std::string> headers{"Authorization: Token " + auth_token_,
                                   "Content-Type: text/plain; charset=utf-8",
                                   "Accept: application/json"};

  CurlRequest request{full_path};
  request.setMethod(CurlRequest::Method::POST);
  request.setHeaders(headers);
  request.setBody(all_lines);
  request.perform();
}