/**
 * @file configuration.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief
 * @date 2023-05-13
 * @copyright Copyright (c) 2023
 */

#include "configuration.h"

#include "configuration_internal.h"

cfg::Configuration::Configuration(data::ErrorStream &es): es_(es) {
  config_lock_.lock();

  general_.project_name = cfg::general::defaults::project_name;
  general_.main_board_type = cfg::general::defaults::main_board;
  general_.starting_procedure = cfg::general::defaults::starting_procedure;

  debug_.print_errors = cfg::debug::defaults::print_errors;
  debug_.console_update_interval = cfg::debug::defaults::console_update_interval;
  debug_.console_enabled = cfg::debug::defaults::console_enabled;

  server_.tcp_socket_port = cfg::server::defaults::tcp_socket_port;

  telemetry_.telemetry_enabled = cfg::telemetry::defaults::telemetry_enabled;
  telemetry_.call_sign = cfg::telemetry::defaults::call_sign;

  aprs_.telemetry_packets = cfg::aprs::defaults::telemetry_packets;
  aprs_.position_packets = cfg::aprs::defaults::position_packets;
  aprs_.frequency.setFrequency(cfg::aprs::defaults::frequency);
  aprs_.ssid = cfg::aprs::defaults::ssid;
  aprs_.destination_address = cfg::aprs::defaults::destination_address;
  aprs_.destination_ssid = cfg::aprs::defaults::destination_ssid;
  aprs_.symbol_table = cfg::aprs::defaults::symbol_table;
  aprs_.symbol = cfg::aprs::defaults::symbol;
  aprs_.comment = cfg::aprs::defaults::comment;

  sstv_.enabled = cfg::sstv::defaults::enabled;
  sstv_.frequency.setFrequency(cfg::sstv::defaults::frequency);
  sstv_.mode = cfg::sstv::defaults::mode;
  sstv_.overlay_data = cfg::sstv::defaults::overlay_data;

  data_packets_.enabled = cfg::data_packets::defaults::enabled;
  data_packets_.frequency.setFrequency(cfg::data_packets::defaults::frequency);
  data_packets_.mode = cfg::data_packets::defaults::mode;
  data_packets_.comment = cfg::data_packets::defaults::comment;
  data_packets_.morse_call_sign = cfg::data_packets::defaults::morse_call_sign;

  config_lock_.unlock();
}

cfg::Configuration::~Configuration() {
  config_lock_.lock();
  config_lock_.unlock();
}

bool cfg::Configuration::setGeneral(const cfg::General &general) {
  bool error_free = true;

  config_lock_.lock();

  std::string error;
  if (!cfg::general::validators::projectName(general.project_name, error)) {
    error_free = false;
    reportError("ST_GEN_PN", error);
  } else {
    general_.project_name = general.project_name;
  }

  general_.main_board_type = general.main_board_type;
  general_.starting_procedure = general.starting_procedure;
  
  config_lock_.unlock();

  return error_free;
}

cfg::General cfg::Configuration::getGeneral() {
  config_lock_.lock();
  cfg::General ret = general_;
  config_lock_.unlock();
  return ret;
}

bool cfg::Configuration::setDebug(const cfg::Debug &debug) {
  config_lock_.lock();
  debug_ = debug;
  config_lock_.unlock();
  return true;
}

cfg::Debug cfg::Configuration::getDebug() {
  config_lock_.lock();
  cfg::Debug ret = debug_;
  config_lock_.unlock();
  return ret;
}

bool cfg::Configuration::setServer(const cfg::Server &server) {
  std::string error;
  if (!cfg::server::validators::tcpSocketPort(server.tcp_socket_port, error)) {
    reportError("ST_SRV_TSP", error);
  }

  config_lock_.lock();
  server_ = server;
  config_lock_.unlock();
  return true;
}

cfg::Server cfg::Configuration::getServer() {
  config_lock_.lock();
  cfg::Server ret = server_;
  config_lock_.unlock();
  return ret;
}

bool cfg::Configuration::setTelemetry(const cfg::Telemetry &telemetry) {
  config_lock_.lock();
  telemetry_ = telemetry;
  config_lock_.unlock();
  return true;
}

cfg::Telemetry cfg::Configuration::getTelemetry() {
  config_lock_.lock();
  cfg::Telemetry ret = telemetry_;
  config_lock_.unlock();
  return ret;
}

bool cfg::Configuration::setAprs(const cfg::Aprs &aprs) {
  config_lock_.lock();
  aprs_ = aprs;
  config_lock_.unlock();
  return true;
}

cfg::Aprs cfg::Configuration::getAprs() {
  config_lock_.lock();
  cfg::Aprs ret = aprs_;
  config_lock_.unlock();
  return ret;
}

bool cfg::Configuration::setSstv(const cfg::Sstv &sstv) {
  config_lock_.lock();
  sstv_ = sstv;
  config_lock_.unlock();
  return true;
}

cfg::Sstv cfg::Configuration::getSstv() {
  config_lock_.lock();
  cfg::Sstv ret = sstv_;
  config_lock_.unlock();
  return ret;
}

bool cfg::Configuration::setDataPackets(const cfg::DataPackets &data_packets) {
  config_lock_.lock();
  data_packets_ = data_packets;
  config_lock_.unlock();
  return true;
}

cfg::DataPackets cfg::Configuration::getDataPackets() {
  config_lock_.lock();
  cfg::DataPackets ret = data_packets_;
  config_lock_.unlock();
  return ret;
}

void cfg::Configuration::reportError(std::string code, std::string info) {
  es_.addError(data::Source::CONFIGURATION_MODULE, code, info);
}