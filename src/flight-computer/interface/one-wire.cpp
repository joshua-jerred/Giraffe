/**
 * @file utility-one-wire.cpp
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief Implementation for the one wire utility class.
 * 
 * @cite http://blog.foool.net/wp-content/uploads/linuxdocs/w1.pdf
 * 
 * @date 2022-11-02
 * @copyright Copyright (c) 2023
 * @version 0.1.0
 */

#include <fstream>
#include <string>
#include <filesystem>
#include <sstream>

#include "interface.h"

interface::OneWire::OneWire(std::string device_id):
	path_(ONE_WIRE_LOCATION + device_id),
	status_(ONEWIRE_STATUS::NOT_FOUND) {
	// Check to see if the device exists
	if (checkDevice() == 0) { // Device exists
		status_ = ONEWIRE_STATUS::OK;
	} else { // Device does not exist
		status_ = ONEWIRE_STATUS::NOT_FOUND;
	}
}

interface::OneWire::~OneWire() {
}

interface::ONEWIRE_STATUS interface::OneWire::status() {
	return status_;
}

/**
 * @brief Read the temperature file
 * @details This will still attempt to read even if the status is not OK.
 * The connection is handled by the kernel, so you might as well try.
 * @return std::string the contents of the w1_slave file
 */
std::string interface::OneWire::read_w1_slave() { // Read the w1_slave file
	std::string line = "";
	std::ifstream file_;
	file_.open(path_ + "/w1_slave");
	if (file_.is_open()) {
    	std::stringstream ss;
		ss << file_.rdbuf();
		return ss.str();
	} else {
		status_ = interface::ONEWIRE_STATUS::READ_ERROR;
		return "";
	}
}


std::string interface::OneWire::read_temperature() { // Read the temperature file
	std::string line = "";
	std::ifstream file_;
	file_.open(path_ + "/temperature");
	if (file_.is_open()) {
    	std::getline(file_, line);
		return line;
	} else {
		status_ = interface::ONEWIRE_STATUS::READ_ERROR;
		return "";
	}
}

int interface::OneWire::checkDevice() {
	bool res = std::filesystem::is_directory(path_);
	if (res == true) {
		return 0;
	} else {
		return -1;
	}
}