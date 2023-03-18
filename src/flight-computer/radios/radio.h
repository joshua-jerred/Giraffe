/**
 * @file radio.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief This file contains the class definition for the generic radio
 * extension.
 * @details A lesson in why OOP isn't always the best solution. Or maybe it's
 * the implementation... I blame the author (me).
 * @date 2023-03-17
 * @copyright Copyright (c) 2023
 * @version 0.1
 */

#ifndef RADIO_H_
#define RADIO_H_

#include "data-stream.h"
#include "interface.h"

namespace radio {

enum class Status {
  OFF,
  UNCONFIGURED,
  CONFIGURING,
  CONFIGURED,
  READY,
  TRANSMITTING,
  RECEIVING,
  PARAMETER_ERROR,
  UNKNOWN_ERROR
};

enum class TxPowerLevel { HIGH = 0, LOW = 1 };

enum class Bandwidth { HIGH = 0, LOW = 1 };

class Radio {
 public:
  Radio(DataStream &data_stream, const RadioMetadata &radio_metadata);
  Radio(const Radio &other) = delete;             // No copy constructor
  Radio &operator=(const Radio &other) = delete;  // No copy assignment
  virtual ~Radio();

  RadioMetadata GetRadioMetadata() const { return radio_metadata_; }
  radio::Status GetStatus() const { return radio_status_; }
  std::string GetTXFrequency() const { return tx_frequency_; }
  std::string GetRXFrequency() const { return rx_frequency_; }
  radio::TxPowerLevel GetTXPowerLevel() const { return tx_power_level_; }
  radio::Bandwidth GetBandwidth() const { return bandwidth_; }

  virtual float GetRSSI();

  virtual bool Initialize();
  virtual bool PowerOn();
  virtual bool PowerOff();

  virtual bool PTTOn();
  virtual bool PTTOff();

  virtual bool SetTXFrequency(const std::string &frequency);
  virtual bool SetRXFrequency(const std::string &frequency);
  virtual bool SetTXPowerLevel(TxPowerLevel power_level);
  virtual bool SetBandwidth(Bandwidth bandwidth);

  /**
   * @brief Set the Squelch of the radio
   * @param squelch A value between 0 and 10, where 0 is "monitor"
   */
  virtual bool SetSquelch(int squelch);

 protected:
  radio::Status radio_status_ = radio::Status::OFF;
  std::string tx_frequency_ = "";
  std::string rx_frequency_ = "";
  radio::TxPowerLevel tx_power_level_ = radio::TxPowerLevel::HIGH;
  radio::Bandwidth bandwidth_ = radio::Bandwidth::HIGH;
  int squelch_ = 5;

  DataStream &data_stream_;
  const RadioMetadata radio_metadata_;
};

class DraSaRadio : public Radio {
 public:
  DraSaRadio(DataStream &data_stream, const RadioMetadata &radio_metadata);
  ~DraSaRadio();

  float GetRSSI();

  bool Initialize();
  bool PowerOn();
  bool PowerOff();

  bool PTTOn();
  bool PTTOff();

  bool SetTXFrequency(const std::string &frequency);
  bool SetRXFrequency(const std::string &frequency);
  bool SetTXPowerLevel(TxPowerLevel power_level);
  bool SetBandwidth(Bandwidth bandwidth);
  bool SetSquelch(int squelch);

 private:
  interface::Serial serial_;

  int power_pin_;
  int ptt_pin_;
  int squelch_pin_;
};

}  // namespace radio

#endif  // RADIO_H_
