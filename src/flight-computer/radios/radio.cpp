/**
 * @file radio.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Implementation of the base radio class
 * @date 2023-03-17
 * @copyright Copyright (c) 2023
 * @version 0.4
 */

#include "radio.h"

namespace radio {

Radio::Radio(DataStream &data_stream, const RadioMetadata radio_metadata)
    : data_stream_(data_stream), radio_metadata_(radio_metadata) {}

Radio::~Radio() {}

float Radio::GetRSSI() {
  // radio_status_ = Status::UNCONFIGURED;
  return 0;
}

bool Radio::Initialize(const std::string &init_frequency) {
  (void)init_frequency;
  radio_status_ = Status::ERROR;
  return false;
}

bool Radio::PowerOn() {
  // radio_status_ = Status::UNCONFIGURED;
  return false;
}

bool Radio::PowerOff() {
  // radio_status_ = Status::UNCONFIGURED;
  return false;
}

bool Radio::PTTOn() {
  radio_status_ = Status::ERROR;
  return false;
}

bool Radio::PTTOff() {
  radio_status_ = Status::ERROR;
  return false;
}

bool Radio::SetFrequency(const std::string &frequency) {
  (void)frequency;
  radio_status_ = Status::ERROR;
  return false;
}

// bool Radio::SetRXFrequency(const std::string &frequency) {
//  (void)frequency;
//  radio_status_ = Status::UNCONFIGURED;
//  return false;
//}

bool Radio::SetTXPowerLevel(TxPowerLevel power_level) {
  (void)power_level;
  radio_status_ = Status::ERROR;
  return false;
}

bool Radio::SetBandwidth(Bandwidth bandwidth) {
  (void)bandwidth;
  radio_status_ = Status::ERROR;
  return false;
}

bool Radio::SetSquelch(int squelch) {
  (void)squelch;
  radio_status_ = Status::ERROR;
  return false;
}

void Radio::error(const std::string &error_code,
                  const std::string &error_message) {
  data_stream_.addError("RAD" + std::to_string(radio_metadata_.radio_id),
                        error_code, error_message);
}

}  // namespace radio
