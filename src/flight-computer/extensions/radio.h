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
  Radio(DataStream &data_stream, const RadioMetadata &radio_metadata)
      : data_stream_(data_stream), radio_metadata_(radio_metadata) {}
  Radio(const Radio &other) = delete;             // No copy constructor
  Radio &operator=(const Radio &other) = delete;  // No copy assignment
  virtual ~Radio();

  RadioMetadata GetRadioMetadata() const { return radio_metadata_; }
  radio::Status GetRadioStatus() const { return radio_status_; }
  std::string GetTXFrequency() const { return tx_frequency_; }
  std::string GetRXFrequency() const { return rx_frequency_; }
  radio::TxPowerLevel GetTXPowerLevel() const { return tx_power_level_; }
  radio::Bandwidth GetBandwidth() const { return bandwidth_; }

  virtual float GetRSSI();

  virtual void Initialize();
  virtual void PowerOn();
  virtual void PowerOff();

  virtual void PTTOn();
  virtual void PTTOff();

  virtual void SetTXFrequency(std::string frequency);
  virtual void SetRXFrequency(std::string frequency);
  virtual void SetTXPowerLevel(TxPowerLevel power_level);
  virtual void SetBandwidth(Bandwidth bandwidth);

  /**
   * @brief Set the Squelch of the radio
   * @param squelch A value between 0 and 10, where 0 is "monitor"
   */
  virtual void SetSquelch(int squelch);

 private:
  radio::Status radio_status_ = radio::Status::OFF;
  std::string tx_frequency_ = "";
  std::string rx_frequency_ = "";
  radio::TxPowerLevel tx_power_level_ = radio::TxPowerLevel::HIGH;
  radio::Bandwidth bandwidth_ = radio::Bandwidth::HIGH;

  DataStream &data_stream_;
  const RadioMetadata radio_metadata_;
};

class SA868 : public Radio {
 public:
  SA868(DataStream &data_stream, const RadioMetadata &radio_metadata)
      : Radio(data_stream, radio_metadata) {}
  ~SA868();

 private:
};

}  // namespace radio

#endif  // RADIO_H_
