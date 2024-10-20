/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   gdl_server.cpp
 * @brief  Data Link Server for Giraffe
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-23
 * @copyright  2024 (license to be defined)
 */

#include "BoosterSeat/filesystem.hpp"
namespace bsfs = bst::filesystem;

#include "gdl_server.hpp"

#include "giraffe_file_paths.hpp"

namespace giraffe::gdl {

int GdlServer::run() {

  auto res = loadConfig(); // NO DEBUG PRINTS BEFORE THIS.
  if (res != 0) {
    return res;
  }

  std::string received_string;
  protocol::Message received_msg;

  if (!server_socket_.init(server_port_)) {
    log_.error("Failed to initialize server socket");
    return 1;
  }

  log_.info("GDL Server Started on port " + std::to_string(server_port_));

  if (data_link_enabled_) {
    gdl_.enable();
    log_.info("Data Link Enabled");
  } else {
    log_.info("Data Link Disabled");
  }

  while (!stop_flag_) {
    if (server_socket_.accept(client_socket_)) {
      log_.info("Client Connected: " + client_socket_.getClientAddress());
      while (!stop_flag_) {
        if (!client_socket_.receive(received_string)) {
          log_.info("Client Disconnected");
          break;
        }
        if (!protocol::parseMessage(received_string, received_msg)) {
          log_.warn("Failed to parse message: " + received_string);
          break;
        }

        if (received_msg.typ == protocol::MessageType::REQ) {
          log_.debug("REQ: " + received_msg.rsc);
          routeRequest(received_msg.rsc);
        } else if (received_msg.typ == protocol::MessageType::SET) {
          log_.debug("SET: " + received_msg.rsc);
          routeSet(received_msg);
        }

        if (!send_result_) {
          log_.info("Client Disconnected");
          break;
        }

        if (!response_sent_) {
          log_.warn("Response not sent");
        }
      }
    }

    /// @todo There might be a better way to do this.
    bst::sleep(2); // prevent 100% CPU usage
  }

  if (gdl_.isEnabled()) {
    gdl_.disable();
    log_.info("Data Link Disabled");
  }

  return 0;
}

int GdlServer::loadConfig() {
  if (file_paths::createGiraffeDirIfNotExists()) {
    log_.info("Created Giraffe Directory");
  }
  std::string config_file_path = file_paths::getGdlServerConfigFilePath();

  if (!bsfs::doesFileExist(config_file_path)) {
    log_.warn("Config file does not exist. Creating...");
    bsfs::createFile(config_file_path);
    saveConfig();
    return 0;
  }

  json config;
  try {
    std::ifstream config_file(config_file_path);
    config = json::parse(config_file);
  } catch (const std::exception &e) {
    log_.error("Failed to open and parse config file: " +
               std::string(e.what()));
    return -1;
  }

  // attempt to load config from file. If it fails, use defaults. (already set)
  setConfigFromJson(config);
  // debug prints are allowed now
  log_.debug("GDL server config loaded from: " + config_file_path);

  saveConfig();

  return 0;
}

bool GdlServer::saveConfig() {
  log_.debug("Saving GDL Server Config");

  json config = getConfigJson();

  std::string config_file_path = file_paths::getGdlServerConfigFilePath();

  try {
    std::ofstream config_file(config_file_path);
    config_file << config.dump(2);
    config_file.close();
  } catch (const std::exception &e) {
    log_.error("Failed to save config file: " + std::string(e.what()));
    return false;
  }

  log_.debug("GDL Server Config Saved");
  return true;
}

json GdlServer::getConfigJson() {
  json config = {
      {"server_port", server_port_},
      {"proactive_keep_alive", gdl_config_.getProactiveKeepAlive()},
      {"logging_level", to_string(log_.getLevel())},
      {"logging_print_to_stdout", log_.getPrintToStdout()},
      {"source_callsign", gdl_config_.getCallSign()},
      {"source_ssid", gdl_config_.getSSID()},
      {"remote_callsign", gdl_config_.getRemoteCallSign()},
      {"remote_ssid", gdl_config_.getRemoteSSID()},
  };
  return config;
}

bool GdlServer::setConfigFromJson(const json &config) {
  // NO DEBUG LOGS BEFORE THIS
  bool valid = true;

  // lambda functions just to be consistent with checking for valid keys
  auto is_valid_string = [&](const std::string &key) {
    return config.contains(key) && config[key].is_string();
  };
  auto is_valid_bool = [&](const std::string &key) {
    return config.contains(key) && config[key].is_boolean();
  };
  auto is_valid_number = [&](const std::string &key) {
    return config.contains(key) && config[key].is_number();
  };

  if (is_valid_bool("logging_print_to_stdout")) {
    log_.setPrintToStdout(config["logging_print_to_stdout"]);
  } else {
    valid = false;
    log_.warn("Failed to load logging_print_to_stdout from json. "
              "Defaulting to true.");
    log_.setPrintToStdout(true);
  }

  if (is_valid_string("logging_level")) {
    std::string level = config["logging_level"];
    if (level == "DEBUG") {
      log_.setLevel(LoggerLevel::DEBUG);
    } else if (level == "INFO") {
      log_.setLevel(LoggerLevel::INFO);
    } else if (level == "WARN") {
      log_.setLevel(LoggerLevel::WARN);
    } else if (level == "ERROR") {
      log_.setLevel(LoggerLevel::ERROR);
    } else {
      valid = false;
      log_.warn("Invalid logging level specified in json. "
                "Defaulting to INFO.");
      log_.setLevel(LoggerLevel::INFO);
    }
  } else {
    valid = false;
    log_.warn("Failed to load logging_level from json. "
              "Defaulting to INFO.");
    log_.setLevel(LoggerLevel::INFO);
  }
  // DEBUG LOGS ALLOWED NOW

  if (is_valid_string("source_callsign") &&
      config["source_callsign"] != "NOCALL") {
    gdl_config_.setCallSign(config["source_callsign"]);
  } else {
    valid = false;
    log_.warn("Failed to load source_callsign from json or is set to default");
  }

  if (is_valid_number("source_ssid")) {
    gdl_config_.setSSID(config["source_ssid"]);
  } else {
    valid = false;
    log_.warn("Failed to load source_ssid from json");
  }

  if (is_valid_string("remote_callsign") &&
      config["remote_callsign"] != "NOCALL") {
    gdl_config_.setRemoteCallSign(config["remote_callsign"]);
  } else {
    valid = false;
    log_.warn("Failed to load remote_callsign from json or is set to default");
  }

  if (is_valid_number("remote_ssid")) {
    gdl_config_.setRemoteSSID(config["remote_ssid"]);
  } else {
    valid = false;
    log_.warn("Failed to load remote_ssid from json");
  }

  if (is_valid_bool("proactive_keep_alive")) {
    gdl_config_.setProactiveKeepAlive(config["proactive_keep_alive"]);
  } else {
    valid = false;
    log_.warn("Failed to load proactive_keep_alive from json. "
              "Defaulting to false.");
    gdl_config_.setProactiveKeepAlive(false);
  }

  if (is_valid_number("server_port")) {
    server_port_ = config["server_port"];
    if (server_port_ < 1 || server_port_ > 65535) {
      valid = false;
      log_.warn("Invalid server_port specified. "
                "Defaulting to 9557.");
      server_port_ = 9557;
    }
  } else {
    valid = false;
    log_.warn("Failed to load server_port from json. "
              "Defaulting to 9557.");
    server_port_ = 9557;
  }

  return valid;
}

void GdlServer::sendResponseData(const json &data,
                                 const std::string &resource) {
  protocol::Message msg;
  protocol::createResponseMessage(msg, protocol::Endpoint::GDL,
                                  protocol::Endpoint::GGS, getNextResponseId(),
                                  protocol::ResponseCode::GOOD, data, resource);
  send_result_ = client_socket_.send(msg.getJsonString());
  response_sent_ = true;
}

void GdlServer::sendResponseError(const std::string &error) {
  protocol::Message msg;
  protocol::createResponseMessage(
      msg, protocol::Endpoint::GDL, protocol::Endpoint::GGS,
      getNextResponseId(), protocol::ResponseCode::ERROR, {{"err", error}});
  send_result_ = client_socket_.send(msg.getJsonString());
  response_sent_ = true;
}

void GdlServer::sendResponseSuccess() {
  protocol::Message msg;
  protocol::createResponseMessage(
      msg, protocol::Endpoint::GDL, protocol::Endpoint::GGS,
      getNextResponseId(), protocol::ResponseCode::GOOD, {{"msg", "success"}});
  send_result_ = client_socket_.send(msg.getJsonString());
  response_sent_ = true;
}

void GdlServer::routeRequest(const std::string &rsc) {
  if (rsc == "status") {
    handleRequestStatus();
  } else if (rsc == "config") {
    handleRequestConfig();
  } else if (rsc == "received_messages") {
    handleRequestReceivedMessages();
  } else if (rsc == "sent_messages") {
    handleRequestSentMessages();
  } else if (rsc == "log") {
    handleRequestLog();
  } else {
    sendResponseError("unknown_resource");
  }
}

void GdlServer::routeSet(const protocol::Message &received_msg) {
  const std::string &rsc = received_msg.rsc;
  if (rsc == "config") {
    handleSetConfig(received_msg.dat);
  } else if (rsc == "new_broadcast") {
    handleSetNewBroadcast(received_msg.dat);
  } else if (rsc == "new_exchange") {
    handleSetNewExchange(received_msg.dat);
  } else if (rsc == "disable_receiver") {
    handleSetDisableReceiver();
  } else if (rsc == "enable_receiver") {
    handleSetEnableReceiver();
  } else if (rsc == "reset_config") {
    handleSetResetConfig();
  } else if (rsc == "restart") {
    handleSetRestart();
  } else {
    sendResponseError("unknown_resource");
  }
}

} // namespace giraffe::gdl