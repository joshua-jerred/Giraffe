/**
 * @file sa868.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Driver for the DraSaRadio
 * @date 2023-02-25
 * @copyright Copyright (c) 2023
 * @version 0.4
 */

#include "radio.h"

namespace radio {

DraSaRadio::DraSaRadio(DataStream &data_stream,
                       const RadioMetadata &radio_metadata)
    : Radio(data_stream, radio_metadata),
      serial_(radio_metadata.extra_args.uart_device_path,
              radio_metadata.extra_args.uart_baud_rate),
      power_pin_(radio_metadata.gpio_power),
      ptt_pin_(radio_metadata.gpio_ptt),
      squelch_pin_(radio_metadata.gpio_squelch_detect) {}

DraSaRadio::~DraSaRadio() {
  if (radio_status_ != Status::OFF) {
    PowerOff();
  }

  if (serial_.IsConnected()) {
    serial_.Disconnect();
  }
}

float DraSaRadio::GetRSSI() { return 0; }

bool DraSaRadio::Initialize() { return true; }

bool DraSaRadio::PowerOn() { return true; }

bool DraSaRadio::PowerOff() { return true; }

bool DraSaRadio::PTTOn() { return true; }

bool DraSaRadio::PTTOff() { return true; }

bool DraSaRadio::SetTXFrequency(const std::string &frequency) {
  (void)frequency;
  return true;
}

bool DraSaRadio::SetRXFrequency(const std::string &frequency) {
  (void)frequency;
  return true;
}

bool DraSaRadio::SetTXPowerLevel(TxPowerLevel power_level) {
  (void)power_level;
  return true;
}

bool DraSaRadio::SetBandwidth(Bandwidth bandwidth) {
  (void)bandwidth;
  return true;
}

bool DraSaRadio::SetSquelch(int squelch) {
  (void)squelch;
  return true;
}

}  // namespace radio