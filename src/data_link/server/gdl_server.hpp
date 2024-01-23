/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   gdl_server.hpp
 * @brief  Data Link Server for Giraffe
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-23
 * @copyright  2024 (license to be defined)
 */

#ifndef GDL_SERVER_HPP_
#define GDL_SERVER_HPP_

#include "nlohmann/json.hpp"
#include <socket.hpp>
#include <unordered_map>

using json = nlohmann::json;

#include "common_logger.hpp"
#include "protocol.hpp"

#include "gdl_server_database.hpp"
#include "giraffe_data_link.hpp"

namespace giraffe::gdl {

static const std::unordered_map<DataLink::Status, std::string> GDL_STATUS_MAP{
    {DataLink::Status::DISABLED, "DISABLED"},
    {DataLink::Status::DISCONNECTED, "DISCONNECTED"},
    {DataLink::Status::CONNECTED, "CONNECTED"},
};

/**
 * @brief The Giraffe Data Link Terminal
 * @details This contains a simple terminal for interacting with the GDL (or at
 * least it did). However, not it is primarily interfaced with the GGS via a TCP
 * socket.
 */
class GdlServer {
public:
  GdlServer() = default;
  ~GdlServer() = default;

  int run();

private:
  int loadConfig();
  void saveConfig();

  json getConfigJson();

  /**
   * @brief Set the configuration from a JSON object. Will use default values
   * for any missing fields.
   *
   * @param config - The JSON object to use for configuration.
   * @return true - The JSON was fully parsed and valid. Configuration was set.
   * @return false - The JSON was invalid. Configuration may be partially set.
   */
  bool setConfigFromJson(const json &config);

  // ---- RESPONSE SENDERS ----
  void sendResponseData(const json &data);
  void sendResponseError(const std::string &error);
  void sendResponseSuccess();

  // ---- REQUESTS ----
  void routeRequest(const std::string &rsc);
  void handleRequestConfig();
  void handleRequestStatus();
  void handleRequestReceivedMessages();

  // ---- SETS ----
  void routeSet(const protocol::Message &received_msg);
  void handleSetNewBroadcast(const json &received_data);
  void handleSetConfig(const json &received_data);

  json aprsPositionalPacketToJson(signal_easel::aprs::PositionPacket &packet) {
    auto info = packet.frame.getInformation();
    std::string raw(info.begin(), info.end());

    json res = {{"src", packet.source_address},
                {"src_ssid", packet.source_ssid},
                {"dst", packet.destination_address},
                {"dst_ssid", packet.destination_ssid},
                {"latitude", packet.latitude},
                {"longitude", packet.longitude},
                {"altitude", packet.altitude},
                {"course", packet.course},
                {"speed", packet.speed},
                {"symbol_table", packet.symbol_table},
                {"symbol", packet.symbol},
                {"comment", packet.comment},
                {"gps_time", packet.time_code},
                {"info", raw},
                {"utc_timestamp", packet.decoded_timestamp.toString()}};
    return res;
  }

  uint32_t getNewBroadcastId() {
    last_broadcast_id_++;
    return last_broadcast_id_;
  }

  bool stop_flag_{false};
  sock::TcpSocketServer server_socket_{};
  sock::TcpSocketServer client_socket_{};

  /**
   * @brief Set in sendResponseData() and sendResponseError() to indicate that
   * the client is not connected.
   */
  bool send_result_{false};

  /**
   * @brief Used to check if a response has been sent to the client.
   */
  bool response_sent_{false};

  Config gdl_config_ = Config(true);
  DataLink gdl_{gdl_config_};

  GdlServerDatabase db_{};

  uint32_t last_broadcast_id_{1};

  uint32_t protocol_id_{1};
  std::string getNextResponseId() {
    return std::to_string(protocol_id_++);
  }

  CommonLogger<100> log_{LoggerLevel::DEBUG, true};
};

} // namespace giraffe::gdl

#endif /* GDL_SERVER_HPP_ */
