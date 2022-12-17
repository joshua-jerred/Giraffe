/**
 * @file utility-one-wire.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief Implementation for the one wire utility class.
 * @date 2022-11-02
 * @copyright Copyright (c) 2022
 * @version 0.1.0
 */

#include "utility-one-wire.h"

OneWire::OneWire(std::string device_id) {
	std::string onewire_path = ONE_WIRE_LOCATION;
	std::string onewire_file = ONE_WIRE_FILE;
    path_ = onewire_path + device_id + onewire_file;
}

OneWire::~OneWire() {
}

std::string OneWire::read() {
	std::string line = "";

	file_.open(path_);
	if (file_.is_open()) {
    	std::getline(file_, line);
		return line;
	} else {
		return line;
	}
}

int OneWire::checkDevice() {
	bool res = std::filesystem::exists(path_);

	if (res == true) {
		return 0;
	} else {
		return -1;
	}
}