/**
 * @file configuration.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief
 * @date 2023-05-13
 * @copyright Copyright (c) 2023
 */

#include "configuration.h"

#include "configuration_internal.h"

cfg::Configuration::Configuration() {
  config_lock_.lock();

  general_.project_name = cfg::general::defaults::project_name;
  general_.main_board_type = cfg::general::defaults::main_board;
  general_.starting_procedure = cfg::general::defaults::starting_procedure;

  debug_.print_errors = cfg::debug::defaults::print_errors;
  debug_.console_update_interval = cfg::debug::defaults::console_update_interval;
  debug_.console_enabled = cfg::debug::defaults::console_enabled;

  server_.tcp_socket_port = cfg::server::defaults::tcp_socket_port;

  config_lock_.unlock();
}

cfg::Configuration::~Configuration() {
  config_lock_.lock();
  config_lock_.unlock();
}

void cfg::Configuration::setGeneral(cfg::General &general) {
  config_lock_.lock();
  general_ = general;
  config_lock_.unlock();
}

cfg::General cfg::Configuration::getGeneral() {
  config_lock_.lock();
  cfg::General ret = general_;
  config_lock_.unlock();
  return ret;
}

void cfg::Configuration::setDebug(cfg::Debug &debug) {
  config_lock_.lock();
  debug_ = debug;
  config_lock_.unlock();
}

cfg::Debug cfg::Configuration::getDebug() {
  config_lock_.lock();
  cfg::Debug ret = debug_;
  config_lock_.unlock();
  return ret;
}

void cfg::Configuration::setServer(cfg::Server &server) {
  config_lock_.lock();
  server_ = server;
  config_lock_.unlock();
}

cfg::Server cfg::Configuration::getServer() {
  config_lock_.lock();
  cfg::Server ret = server_;
  config_lock_.unlock();
  return ret;
}
