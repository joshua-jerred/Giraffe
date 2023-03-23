/**
 * @file sa868.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Driver for the DraSaRadio
 * @date 2023-02-25
 * @copyright Copyright (c) 2023
 * @version 0.4
 */

#include <iostream>
#include <regex>
#include <thread>

#include "radio.h"

namespace radio {

DraSaRadio::DraSaRadio(DataStream &data_stream,
                       const RadioMetadata radio_metadata)
    : Radio(data_stream, radio_metadata),
      serial_(radio_metadata.extra_args.uart_device_path,
              radio_metadata.extra_args.uart_baud_rate),
      power_pin_(radio_metadata.gpio_power),
      ptt_pin_(radio_metadata.gpio_ptt),
      squelch_detect_pin_(radio_metadata.gpio_squelch_detect) {
  std::cout << "PTT Pin: " << ptt_pin_ << std::endl;
  try {
    serial_.Connect();
    if (!serial_.IsConnected()) {
      radio_status_ = Status::ERROR;
      error("UART_CONNECT");
    }
  } catch (interface::SerialException &e) {
    radio_status_ = Status::ERROR;
    error("UART_INIT", e.what());
  }
  if (!gpio.IsInitialized()) {
    radio_status_ = Status::ERROR;
    error("GPIO_INIT");
  }
  try {
    ptt_gpio = interface::Gpio::Pin(ptt_pin_, interface::Gpio::PinMode::OUTPUT);
    gpio.SetupPin(ptt_gpio);
  } catch (interface::Gpio::GpioException &e) {
    radio_status_ = Status::ERROR;
    error("GPIO_PTT");
  }
}

DraSaRadio::~DraSaRadio() {
  if (radio_status_ != Status::OFF) {
    PowerOff();
  }

  if (serial_.IsConnected()) {
    serial_.Disconnect();
  }
}

// Not yet implemented
float DraSaRadio::GetRSSI() {
  if (!radio_metadata_.rssi_capable) {
    return -1;
  }

  return 0;
}

// May be changed to void
bool DraSaRadio::Initialize(const std::string &init_frequency) {
  std::cout << "Initialize" << std::endl;
  SetFrequency(init_frequency);
  if (!Handshake()) {
    radio_status_ = Status::ERROR;
    error("INIT_HS");
  }
  if (!SetFilters()) {
    radio_status_ = Status::ERROR;
    error("INIT_SF");
  }
  if (!SetGroup()) {
    radio_status_ = Status::ERROR;
    error("INIT_SG");
  }
  radio_status_ = Status::NOT_CONFIGURED;
  return true;
}

bool DraSaRadio::PowerOn() {
  std::cout << "PowerOn" << std::endl;
  return true;
}

bool DraSaRadio::PowerOff() {
  std::cout << "PowerOff" << std::endl;
  return true;
}

bool DraSaRadio::PTTOn() {
  try {
    gpio.Write(ptt_gpio, true);
    std::this_thread::sleep_for(std::chrono::milliseconds(
        radio_metadata_.ptt_delay));  // Wait for radio to settle
  } catch (interface::Gpio::GpioException &e) {
    radio_status_ = Status::ERROR;
    error("GPIO_PTT_T");
  }
  std::cout << "PTTOn " << radio_metadata_.ptt_delay << std::endl;
  return true;
}

bool DraSaRadio::PTTOff() {
  try {
    gpio.Write(ptt_gpio, false);
  } catch (interface::Gpio::GpioException &e) {
    radio_status_ = Status::ERROR;
    error("GPIO_PTT_T");
  }
  std::cout << "PTTOff" << std::endl;
  return true;
}

bool DraSaRadio::SetFrequency(const std::string &frequency) {
  // This is an ugly way of doing this, it should be changed
  std::cout << "SetFrequency: " << frequency << std::endl;
  std::regex gfs_freq_format("^[0-9]{3}[.][0-9]{3}$");
  if (!std::regex_search(frequency, gfs_freq_format)) {
    error("SF_FMT", frequency);
    return false;
  }
  try {
    float freq = std::stof(frequency);
    bool FreqInRange = false;
    for (std::pair<std::string, std::string> pair :
         radio_metadata_.frequency_ranges) {
      if (freq >= std::stof(pair.first) || freq <= std::stof(pair.second)) {
        FreqInRange = true;
      }
    }
    if (!FreqInRange) {
      error("SF_FNIR", frequency);
      return false;
    }

    // Split frequency not yet implemented
    tx_frequency_ = frequency + "0";
    rx_frequency_ = frequency + "0";
  } catch (std::invalid_argument &e) {
    error("SF_FRFMT", frequency);
    return false;
  } catch (std::out_of_range &e) {
    error("SF_FRFMT", frequency);
    return false;
  }

  if (SetGroup()) {
    radio_status_ = Status::CONFIGURED;
  } else {
    radio_status_ = Status::ERROR;
  }
  return true;
}

// bool DraSaRadio::SetRXFrequency(const std::string &frequency) {
//   (void)frequency;
//   std::cout << "SetRXFrequency" << frequency << std::endl;
//   return true;
// }

// Not yet implemented
bool DraSaRadio::SetTXPowerLevel(TxPowerLevel power_level) {
  (void)power_level;
  std::cout << "SetTXPowerLevel" << std::endl;
  return true;
}

// Not yet implemented
bool DraSaRadio::SetBandwidth(Bandwidth bandwidth) {
  (void)bandwidth;
  std::cout << "SetBandwidth" << std::endl;
  return true;
}

// Not yet implemented
bool DraSaRadio::SetSquelch(int squelch) {
  (void)squelch;
  std::cout << "SetSquelch" << std::endl;
  return true;
}

bool DraSaRadio::Handshake() {
  const std::string command = "AT+DMOCONNECT\r\n";
  const std::string expected_response = "+DMOCONNECT:0\r\n";
  std::string response;

  for (int i = 0; i < 3; i++) {
    response = SerialWriteRead(command);
    if (response == expected_response) {
      return true;
    }
  }

  error("HS_FAIL", response);
  return false;
}

bool DraSaRadio::SetFilters() {
  // Not control of this is implemented
  // AT+SETFILTER=PRE/DE-EMPHASIS, HIGH PASS, LOW PASS
  // 0 = on, 1 = off
  const std::string command = "AT+SETFILTER=1,0,0\r\n";
  const std::string expected_response = "+DMOSETFILTER:0\r\n";
  std::string response = SerialWriteRead(command);
  if (response == expected_response) {
    return true;
  } else {
    error("SFIL_FAIL", response);
    return false;
  }
}

bool DraSaRadio::SetGroup() {
  // AT+DMOSETGROUP= Bandwidth，TFV，RFV，Tx_CXCSS，SQ，Rx_CXCSS

  // 0 = 25 kHz kHz, 1 = 12.5 kHz
  std::string bandwidth = bandwidth_ == Bandwidth::HIGH ? "0" : "1";

  // Frequency is in MHz, format of "XXX.XXXX"
  std::regex freq_format("^[0-9]{3}[.][0-9]{4}$");
  // TX Frequency
  if (!std::regex_search(tx_frequency_, freq_format)) {
    error("SG_TF", tx_frequency_);
    return false;
  }
  // RX Frequency
  if (!std::regex_search(rx_frequency_, freq_format)) {
    error("SG_RF", rx_frequency_);
    return false;
  }

  // CTCSS Format is "XXXX"
  // TX CTCSS (Not implemented)
  std::string tx_ctcss = "0000";
  // RX CTCSS (Not implemented)
  std::string rx_ctcss = "0000";

  // Squelch is 0-8, 0 is monitor mode, GFS implementation is 0-9
  // Should be normalized prior to this function call
  if (squelch_ < 0 || squelch_ > 8) {
    error("SG_SQF", std::to_string(squelch_));
    return false;
  }
  std::string sq = std::to_string(squelch_);

  // Put it all together
  std::string command = "AT+DMOSETGROUP=" + bandwidth + "," + tx_frequency_ +
                        "," + rx_frequency_ + "," + tx_ctcss + "," + sq + "," +
                        rx_ctcss + "\r\n";
  std::string expected_response = "+DMOSETGROUP:0\r\n";

  std::string response = SerialWriteRead(command);
  if (response != expected_response) {
    error("SG_SE", response);
    return false;
  }
  return true;
}

std::string DraSaRadio::SerialWriteRead(const std::string &command) {
  const int wait_for_response_time_ = 200;  // ms

  try {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(wait_for_response_time_));
    std::cout << command << std::endl;
    serial_.Write(command);
    std::this_thread::sleep_for(
        std::chrono::milliseconds(wait_for_response_time_));
    std::string response = serial_.ReadLine();
    std::cout << response << std::endl;
    return response;
  } catch (const interface::SerialException &e) {
    error("SE", e.what());
    return "SE";
  }
}

}  // namespace radio
