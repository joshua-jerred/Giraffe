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
namespace bsfs = BoosterSeat::filesystem;

#include "gdl_server.hpp"

#include "giraffe_file_paths.hpp"

namespace giraffe::gdl {

int GdlServer::run() {

  auto res = loadConfig(); // NO DEBUG PRINTS BEFORE THIS.
  if (res != 0) {
    return res;
  }

  log_.debug("Starting GDL Server");

  std::string received_string;
  protocol::Message received_msg;

  gdl_.enable();
  if (!server_socket_.init(9557)) {
    log_.error("Failed to initialize server socket");
    return 1;
  }
  log_.info("GDL Server Started");

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
          routeRequest(received_msg.rsc);
        } else if (received_msg.typ == protocol::MessageType::SET) {
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

    if (gdl_.messageAvailable()) {
      gdl::Message msg;
      if (gdl_.receiveMessage(msg)) {
        db_.insertMessage(msg, false);
      }
    }
  }

  gdl_.disable();

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

void GdlServer::saveConfig() {
  log_.debug("Saving GDL Server Config");

  json config = getConfigJson();

  std::string config_file_path = file_paths::getGdlServerConfigFilePath();

  try {
    std::ofstream config_file(config_file_path);
    config_file << config.dump(2);
    config_file.close();
  } catch (const std::exception &e) {
    log_.error("Failed to save config file: " + std::string(e.what()));
    return;
  }

  log_.debug("GDL Server Config Saved");
}

json GdlServer::getConfigJson() {
  json config = {
      {"logging_level", to_string(log_.getLevel())},
      {"proactive_keep_alive", gdl_config_.getProactiveKeepAlive()},
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
    log_.warn("Failed to load logging_print_to_stdout from config file. "
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
      log_.warn("Invalid logging level specified. "
                "Defaulting to INFO.");
      log_.setLevel(LoggerLevel::INFO);
    }
  } else {
    valid = false;
    log_.warn("Failed to load logging_level from config file. "
              "Defaulting to INFO.");
    log_.setLevel(LoggerLevel::INFO);
  }
  // DEBUG LOGS ALLOWED NOW

  if (is_valid_string("source_callsign") &&
      config["source_callsign"] != "NOCALL") {
    gdl_config_.setCallSign(config["source_callsign"]);
  } else {
    valid = false;
    log_.warn(
        "Failed to load source_callsign from config file or is set to default");
  }

  if (is_valid_number("source_ssid") && config["source_ssid"] != 0) {
    gdl_config_.setSSID(config["source_ssid"]);
  } else {
    valid = false;
    log_.warn(
        "Failed to load source_ssid from config file or is set to default");
  }

  if (is_valid_string("remote_callsign") &&
      config["remote_callsign"] != "NOCALL") {
    gdl_config_.setRemoteCallSign(config["remote_callsign"]);
  } else {
    valid = false;
    log_.warn(
        "Failed to load remote_callsign from config file or is set to default");
  }

  if (is_valid_number("remote_ssid") && config["remote_ssid"].is_number() &&
      config["remote_ssid"] != 0) {
    gdl_config_.setRemoteSSID(config["remote_ssid"]);
  } else {
    valid = false;
    log_.warn(
        "Failed to load remote_ssid from config file or is set to default");
  }

  if (is_valid_bool("proactive_keep_alive")) {
    gdl_config_.setProactiveKeepAlive(config["proactive_keep_alive"]);
  } else {
    valid = false;
    log_.warn("Failed to load proactive_keep_alive from config file. "
              "Defaulting to false.");
    gdl_config_.setProactiveKeepAlive(false);
  }

  return valid;
}

void GdlServer::sendResponseData(const json &data) {
  protocol::Message msg;
  protocol::createResponseMessage(msg, protocol::Endpoint::GDL,
                                  protocol::Endpoint::GGS, getNextResponseId(),
                                  protocol::ResponseCode::GOOD, data);
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
  protocol::createResponseMessage(msg, protocol::Endpoint::GDL,
                                  protocol::Endpoint::GGS, getNextResponseId(),
                                  protocol::ResponseCode::GOOD, {});
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
  } else {
    sendResponseError("unknown_resource");
  }
}

void GdlServer::routeSet(const protocol::Message &received_msg) {
  if (received_msg.rsc == "new_broadcast") {
    handleSetNewBroadcast(received_msg.dat);
  } else {
    sendResponseError("unknown_resource");
  }
}

} // namespace giraffe::gdl