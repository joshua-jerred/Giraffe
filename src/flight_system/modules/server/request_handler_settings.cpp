/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   request_handler_settings.cpp
 * @brief  The setting request handler implementation.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-02-20
 * @copyright  2024 (license to be defined)
 */

#include "giraffe_file_paths.hpp"
#include "request_handler.hpp"

void RequestRouter::handleSettingRequest(sock::TcpSocketServer &client,
                                         protocol::Message &msg) {
  Json res_body;

  std::string req = msg.rsc.substr(msg.rsc.find('/') + 1);

  // it ain't pretty, but it works and it's faster than getting all the json at
  // once
  if (req == "general") {
    res_body = config_.general.getJson();
  } else if (req == "data_module_data") {
    res_body = config_.data_module_data.getJson();
  } else if (req == "data_module_influxdb") {
    res_body = config_.data_module_influxdb.getJson();
  } else if (req == "data_module_log") {
    res_body = config_.data_module_log.getJson();
  } else if (req == "console_module") {
    res_body = config_.console_module.getJson();
  } else if (req == "server_module") {
    res_body = config_.server_module.getJson();
  } else if (req == "system_module") {
    res_body = config_.system_module.getJson();
  } else if (req == "telemetry") {
    res_body = config_.telemetry.getJson();
  } else if (req == "telemetry_aprs") {
    res_body = config_.telemetry_aprs.getJson();
  } else if (req == "telemetry_sstv") {
    res_body = config_.telemetry_sstv.getJson();
  } else if (req == "telemetry_data_packets") {
    res_body = config_.telemetry_data_packets.getJson();
  } else if (req == "extensions") {
    res_body = config_.extensions.getJson();
  } else if (req == "extension_module") {
    res_body = config_.extension_module.getJson();
  } else if (req == "hardware_interface") {
    res_body = config_.hardware_interface.getJson();
  } else if (req == "adc_mappings") {
    res_body = config_.adc_mappings.getJson();
  } else if (req == "battery") {
    res_body = config_.battery.getJson();
  } else {
    sendErrorPacket(client, "setting section not found");
    return;
  }

  protocol::Message response_message;
  protocol::createResponseMessage(response_message, protocol::Endpoint::GFS,
                                  msg.src, msg.id, protocol::ResponseCode::GOOD,
                                  res_body);

  sendMessage(response_message, client);
}

void RequestRouter::handleSettingSet(sock::TcpSocketServer &client,
                                     protocol::Message &msg) {

  if (!msg.getBodyJson().contains("dat")) {
    sendErrorPacket(client, "no data provided");
    return;
  }

  std::string req = msg.rsc.substr(msg.rsc.find('/') + 1);
  if (req == "extensions") {
    handleExtensionSettingUpdate(client, msg);
    return;
  } else if (req == "adc_mappings") {
    handleAdcMappingSettingUpdate(client, msg);
    return;
  }

  Json new_cfg_data = msg.getBodyJson()["dat"];

  Json res_body = {{"success", true}};
  if (req == "general") {
    config_.general.setFromJson(new_cfg_data);
  } else if (req == "data_module_data") {
    config_.data_module_data.setFromJson(new_cfg_data);
  } else if (req == "data_module_influxdb") {
    config_.data_module_influxdb.setFromJson(new_cfg_data);
  } else if (req == "data_module_log") {
    config_.data_module_log.setFromJson(new_cfg_data);
  } else if (req == "console_module") {
    config_.console_module.setFromJson(new_cfg_data);
  } else if (req == "server_module") {
    config_.server_module.setFromJson(new_cfg_data);
  } else if (req == "system_module") {
    config_.system_module.setFromJson(new_cfg_data);
  } else if (req == "telemetry") {
    config_.telemetry.setFromJson(new_cfg_data);
  } else if (req == "telemetry_aprs") {
    config_.telemetry_aprs.setFromJson(new_cfg_data);
  } else if (req == "telemetry_sstv") {
    config_.telemetry_sstv.setFromJson(new_cfg_data);
  } else if (req == "telemetry_data_packets") {
    config_.telemetry_data_packets.setFromJson(new_cfg_data);
  } else if (req == "extension_module") {
    config_.extension_module.setFromJson(new_cfg_data);
  } else if (req == "hardware_interface") {
    config_.hardware_interface.setFromJson(new_cfg_data);
  } else if (req == "battery") {
    config_.battery.setFromJson(new_cfg_data);
  } else {
    sendErrorPacket(client, "setting section not found");
    return;
  }

  config_.save(giraffe::file_paths::getGfsConfigFilePath());

  protocol::Message response_message;
  protocol::createResponseMessage(response_message, protocol::Endpoint::GFS,
                                  msg.src, msg.id, protocol::ResponseCode::GOOD,
                                  res_body);
  sendMessage(response_message, client);
}

void RequestRouter::handleExtensionSettingUpdate(sock::TcpSocketServer &client,
                                                 protocol::Message &msg) {
  std::string action;
  try {
    auto bdy = msg.getBodyJson();
    auto dat = bdy.at("dat");
    action = dat.at("action");

    if (action == "add") {
      auto ext_data = dat.at("ext_data");
      cfg::ExtensionMetadata ext_meta;
      ext_meta.setFromJson(ext_data, shared_data_.streams.log, "0");
      if (config_.extensions.addExtension(ext_meta)) {
        sendSuccessResponse(client);
      } else {
        sendErrorPacket(client, "Failed to add, check logs");
      }
      return;
    } else if (action == "remove") {
      auto ext_name = dat.at("ext_name");
      if (config_.extensions.removeExtension(ext_name)) {
        sendSuccessResponse(client);
      } else {
        sendErrorPacket(client, "Failed to remove, check logs");
      }
      return;
    } else if (action == "update") {
      auto ext_name = dat.at("ext_name");
      auto ext_data = dat.at("ext_data");
      cfg::ExtensionMetadata ext_meta;
      ext_meta.setFromJson(ext_data, shared_data_.streams.log, "0");
      if (config_.extensions.updateExtension(ext_name, ext_meta)) {
        sendSuccessResponse(client);
      } else {
        sendErrorPacket(client, "Failed to update, check logs");
      }
      return;
    }

    sendErrorPacket(client, "Invalid action provided");
    return;
    // auto ext_data = dat.at("extension_data");
  } catch (std::exception &e) {
    sendErrorPacket(client, "Failed to parse extension request");
    return;
  }

  sendErrorPacket(client, "Internal logic error");
}

void RequestRouter::handleAdcMappingSettingUpdate(sock::TcpSocketServer &client,
                                                  protocol::Message &msg) {
  std::string action;
  try {
    auto bdy = msg.getBodyJson();
    auto dat = bdy.at("dat");
    action = dat.at("action");

    if (action == "add") {
      auto map_data = dat.at("map_data");
      cfg::AdcConfig adc_map;
      adc_map.fromJson(map_data, shared_data_.streams.log, "0");
      if (config_.adc_mappings.addAdcConfig(adc_map)) {
        sendSuccessResponse(client);
      } else {
        sendErrorPacket(client, "Failed to add, check logs");
      }
      return;
    } else if (action == "remove") {
      auto map_label = dat.at("map_label");
      if (config_.adc_mappings.removeAdcConfig(map_label)) {
        sendSuccessResponse(client);
      } else {
        sendErrorPacket(client, "Failed to remove, check logs");
      }
      return;
    } else if (action == "update") {
      auto map_label = dat.at("map_label");
      auto map_data = dat.at("map_data");
      cfg::AdcConfig adc_map;
      adc_map.fromJson(map_data, shared_data_.streams.log, "0");
      if (config_.adc_mappings.updateAdcConfig(map_label, adc_map)) {
        sendSuccessResponse(
            client); // auto ext_data = dat.at("extension_data");
      } else {
        sendErrorPacket(client, "Failed to update, check logs");
      }
      return;
    }

    sendErrorPacket(client, "Invalid action provided");
    return;
  } catch (std::exception &e) {
    sendErrorPacket(client, "Failed to parse adc config request");
    return;
  }

  sendErrorPacket(client, "Internal logic error");
}