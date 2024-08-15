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

#include <socket.hpp>
#include <unordered_map>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include "common_logger.hpp"
#include "protocol.hpp"

#include "giraffe_data_link.hpp"
#include "software_physical_layer.hpp"

namespace giraffe::gdl {

static const std::unordered_map<DataLink::Status, std::string> GDL_STATUS_MAP{
    {DataLink::Status::ERROR, "ERROR"},
    {DataLink::Status::DISABLED, "DISABLED"},
    {DataLink::Status::STARTING, "STARTING"},
    {DataLink::Status::RUNNING, "RUNNING"},
    {DataLink::Status::STOPPING, "STOPPING"}};

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
  void stop() {
    log_.info("Stop Signal Received (keyboard interrupt)");
    stop_flag_ = true;
    client_socket_.close();
    server_socket_.close();
  }

private:
  int loadConfig();

  /**
   * @brief Attempts to save the current configuration to the config file.
   * @details Only need to check the return value for reporting reasons.
   * (request)
   * @return true - The configuration was saved successfully.
   * @return false - The configuration was not saved successfully. Error in log.
   */
  bool saveConfig();

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
  void sendResponseData(const json &data, const std::string &resource);
  void sendResponseError(const std::string &error);
  void sendResponseSuccess();

  // ---- REQUESTS ----
  void routeRequest(const std::string &rsc);
  void handleRequestConfig();
  void handleRequestStatus();
  void handleRequestReceivedMessages();
  void handleRequestSentMessages();
  void handleRequestLog();

  // ---- SETS ----
  void routeSet(const protocol::Message &received_msg);
  void handleSetNewBroadcast(const json &request_data);
  void handleSetNewExchange(const json &request_data);
  void handleSetDisableReceiver();
  void handleSetEnableReceiver();
  void handleSetResetConfig();
  void handleSetRestart();
  void handleSetConfig(const json &request_data);

  uint32_t getNewBroadcastId() {
    last_broadcast_id_++;
    return last_broadcast_id_;
  }

  bool sendMessage(const gdl::Message &msg) {
    if (sent_messages_.size() > MAX_SENT_MESSAGES) {
      sent_messages_.pop();
      log_.warn("Sent Messages Queue Full");
    }
    sent_messages_.push(msg);
    return gdl_.sendMessage(msg);
  }

  bool stop_flag_{false};
  int server_port_{9557};
  bool data_link_enabled_{true};
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
  std::shared_ptr<SoftwarePhysicalLayer> physical_layer_ =
      std::make_shared<SoftwarePhysicalLayer>(gdl_config_);
  DataLink gdl_{gdl_config_, physical_layer_};

  uint32_t last_broadcast_id_{1};

  uint32_t protocol_id_{1};
  std::string getNextResponseId() {
    return std::to_string(protocol_id_++);
  }

  CommonLogger<100> log_{LoggerLevel::INFO, true};

  static const size_t MAX_SENT_MESSAGES{50};
  std::queue<gdl::Message> sent_messages_{};
};

} // namespace giraffe::gdl

#endif /* GDL_SERVER_HPP_ */
