/**
 * @file module-telemetry.cpp
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief This fle implements the class TelemetryModule.
 *
 * @version 0.1.0
 * @date 2022-10-11
 * @copyright Copyright (c) 2022
 * 
 * @todo Documentation
 * @todo Unit Tests
 */

#include <atomic>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include "modules.h"
#include "radio.h"
#include "unit-conversions.h"

using namespace modules;

#include "mwav.h"

bool FileExists(const std::string &name) {
  std::ifstream f(name.c_str());
  return f.good();
}

/**
 * @brief Construct a new TelemetryModule object.
 *
 * @param config_data All configuration data.
 * @param data_stream A pointer to the data stream.
 */
TelemetryModule::TelemetryModule(ConfigData config_data,
                                 DataStream &data_stream)
    : Module(data_stream, configurables::prefix::kTelemetryModule, "telemetry"),
      config_data_(config_data) {
  tx_number_ = 0;  // First tx id will be 1
  std::vector<RadioMetadata> &radio_metadata_ = config_data_.telemetry.radios;
  if (config_data_.telemetry.radios.size() == 0) {
    error("NO_RADIOS");
  } else {
    data("RADIOS", std::to_string(radio_metadata_.size()));
  }

  call_sign_ = config_data_.telemetry.call_sign;

  aprs_generic_.source_address = config_data_.telemetry.call_sign;
  aprs_generic_.source_ssid = config_data_.telemetry.aprs_ssid;

  aprs_generic_.destination_address =
      config_data_.telemetry.aprs_destination_address;
  aprs_generic_.destination_ssid = config_data_.telemetry.aprs_destination_ssid;

  aprs_generic_.symbol = config_data_.telemetry.aprs_symbol;

  if (config_data_.telemetry.aprs_alternate_symbol_table) {
    aprs_generic_.symbol_table = mwav::AprsSymbolTable::SECONDARY;
  } else {
    aprs_generic_.symbol_table = mwav::AprsSymbolTable::PRIMARY;
  }
}

/**
 * @brief Destroy the Telemetry Module:: Telemetry Module object
 * @todo implement this in a safe way.
 */
TelemetryModule::~TelemetryModule() { stop(); }

/**
 * @brief This starts the telemetry module thread.
 * @todo not implemented yet.
 */
void TelemetryModule::start() {
  if (status() != ModuleStatus::STOPPED) {
    return;
  }

  updateStatus(ModuleStatus::STARTING);
  std::vector<RadioMetadata> &radio_metadata_ = config_data_.telemetry.radios;
  for (RadioMetadata &radio_metadata : radio_metadata_) {
    if (radio_metadata.radio_type == "dra-sa") {
      radios_.push_back(new radio::DraSaRadio(data_stream_, radio_metadata));
    } else {
      error("UNKNOWN_RADIO_TYPE", radio_metadata.radio_type);
    }
  }
  ChooseRadio();

  data("TX_Q_SZ", "0");
  data("ACTIVE_TX", "NONE");
  stop_flag_ = 0;
  tx_thread_ = std::thread(&TelemetryModule::runner, this);
}

void TelemetryModule::stop() {
  if (status() == ModuleStatus::STOPPED) {
    return;
  }
  stop_flag_ = 1;
  if (tx_thread_.joinable()) {
    tx_thread_.join();
    updateStatus(ModuleStatus::STOPPED);
  }
  for (radio::Radio *radio : radios_) {
    delete radio;
    radio = nullptr;
  }
  radios_.clear();
}

/**
 * @brief Currently implemented with PSK
 * @param None
 * @return Void
 */
void TelemetryModule::sendDataPacket() {
  int tx_num = getNextTXNumber();

  DataFrame data = data_stream_.getDataFrameCopy();
  std::string message;
  message += "\n\n" + call_sign_ + "\n";
  message += "automated message - data to follow\n";
  message += "pkt_id:" + std::to_string(tx_num) + "\n";

  // Add data (specified in config) to the message that will be sent.
  std::string key = "";
  for (ConfigData::DataTypes::DataType type : config_data_.data_types.types) {
    if (type.include_in_telemetry) {
      key = type.source + ":" + type.unit;
      if (data.contains(key)) {
        message += type.telemetry_name + ":" + data[key].value + "\n";
      }
    }
  }

  // for (const auto & [ unit, packet ] : data) {
  //     message += unit + ":" + packet.value + "\n";
  // }

  message += call_sign_ + "\n";
  message.push_back((char)4);  // End of Transmission character

  std::string message_lower;
  for (auto c : message) message_lower.push_back(std::tolower(c));

  Transmission newTX;
  newTX.type = Transmission::Type::DATA;
  newTX.tx_num = tx_num;
  newTX.wav_location = generatePSK(message_lower, newTX.tx_num);
  newTX.message = message;
  newTX.length = psk_length_;
  addToTXQueue(newTX);  // If the TX failed to generate the error must be
                        // handled somewhere else. Thread Safe.
}

/**
 * @brief Pulls data from the data snapshot and then
 * uses the AFSK utility to create an APRS packet.
 * @todo currently not implemented
 */
void TelemetryModule::sendAPRS() {
  Transmission newTX;
  newTX.type = Transmission::Type::APRS;
  newTX.tx_num = getNextTXNumber();
  newTX.wav_location = GenerateAprsLocation(newTX.tx_num);
  newTX.length = 10; /** @todo implement this*/
  addToTXQueue(newTX);
}

void TelemetryModule::doCommand(GFSCommand command) {
  std::cout << "Telemetry Module is doing the command:" << command.id
            << std::endl;
  if (command.id ==
      "rtx") {  // Resend a transmission from the transmission log (by id)
    int tx_id = 0;
    try {
      tx_id = std::stoi(command.arg);
    } catch (std::invalid_argument &e) {
      error("RTX_INV", command.arg);
      return;
    }

    Transmission newTX;
    if (data_stream_.requestTXFromLog(tx_id, newTX)) {
      std::cout << "Re-sending transmission " << tx_id << std::endl;
      if (newTX.type == Transmission::Type::DATA) {
        std::string old_message = newTX.message;
        int old_tx_num = newTX.tx_num;
        newTX.tx_num = getNextTXNumber();
        std::string new_message =
            "ReTx of #" + std::to_string(old_tx_num) + "\n";
        new_message += old_message;

        newTX.message = "";

        for (auto c : new_message) newTX.message.push_back(std::tolower(c));

        // newTX.message += old_message;
        newTX.wav_location = generatePSK(newTX.message, newTX.tx_num);

        if (newTX.wav_location != "") {
          addToTXQueue(newTX);
        } else {
          error("RTX_MF", command.arg);
        }
      }
    } else {
      error("RTX_NF", command.arg);
    }

  } else {
    error("INV_CMD", command.id);
  }
  /*
   */
}

/**
 * @brief Adds an SSTV image to the transmit queue.
 * It will use the path of the most recent picture from
 * the data snapshot, along with environmental data.
 */
void TelemetryModule::sendSSTVImage() {
  Transmission newTX;
  newTX.type = Transmission::Type::SSTV;
  newTX.wav_location = GenerateSSTV(getNextTXNumber());
  if (newTX.wav_location == "") {
    error("SSTV_MF", "");
    return;
  }
  addToTXQueue(newTX);
}

/**
 * @brief Incremented the tx number and returns the new value.
 * @return int
 */
int TelemetryModule::getNextTXNumber() { return tx_number_++; }

void TelemetryModule::ChooseRadio() {
  std::vector<RadioMetadata> &radio_metadata_ = config_data_.telemetry.radios;
  if (radio_metadata_.size() == 0) {
    error("NO_RADIOS", "");
  }

  int lowest_priority = radio_metadata_[0].priority;
  primary_radio_index_ = 0;
  int num_enabled = 0;
  for (unsigned int i = 0; i < radio_metadata_.size(); i++) {
    if (!radio_metadata_[i].enabled) {
      continue;
    }
    num_enabled++;
    int priority = radio_metadata_[i].priority;
    if (priority < lowest_priority) {
      lowest_priority = priority;
      primary_radio_index_ = i;
    }
  }

  if (num_enabled == 0) {
    error("NO_EN_RAD", "");
    primary_radio_ = nullptr;
  } else {
    primary_radio_ = radios_[primary_radio_index_];
  }
  data("PRI_RAD", radio_metadata_[primary_radio_index_].radio_name);
}

/**
 * @brief First checks the transmission for type, and verifies
 * that the wav file actually exists. If both are good,
 * it will acquire a lock on the transmission queue, add the
 * data, then unlock it.
 * @param transmission The transmission to add to the queue.
 * @return void
 * @see Transmission
 * @todo Fully implement the queue
 */
void TelemetryModule::addToTXQueue(Transmission transmission) {
  if (transmission.type == Transmission::Type::ERROR) {
    error("BAD_TX_TYPE", "");
    return;
  }
  // This will be enabled once file generation is implemented.
  std::ifstream fs(transmission.wav_location);
  if (!fs.good()) {
    data_stream_.addError("M_TEL", "BAD_TX_FILE", transmission.wav_location,
                          10);
    return;
  }

  data_stream_.addToTxQueue(transmission);
}

std::string TelemetryModule::generatePSK(const std::string &message,
                                         const int tx_number) {
  std::string file_path = configurables::file_paths::kTelemetryWavLocation +
                          (std::string) "psk-" + std::to_string(tx_number) +
                          ".wav";

  mwav::DataModulation mode = mwav::DataModulation::BPSK_125;  // Default

  std::string requested_mode = config_data_.telemetry.data_packets_mode;
  if (requested_mode == "bpsk125") {
    mode = mwav::DataModulation::BPSK_125;
  } else if (requested_mode == "bpsk250") {
    mode = mwav::DataModulation::BPSK_250;
  } else if (requested_mode == "bpsk500") {
    mode = mwav::DataModulation::BPSK_500;
  } else if (requested_mode == "bpsk1000") {
    mode = mwav::DataModulation::BPSK_1000;
  } else {
    error("PSK_INV", requested_mode);
    mode = mwav::DataModulation::BPSK_125;
  }

  bool res = false;

  try {
    if (config_data_.telemetry.data_packets_morse_callsign) {
      res = mwav::EncodeString(mode, message, file_path, call_sign_);
    } else {
      res = mwav::EncodeString(mode, message, file_path);
    }
  } catch (mwav::Exception &e) {
    error("PSK_EX", e.what());
    return "";
  }

  if (res) {
    psk_length_ = 10; /** @todo implement this */
    return file_path;
  } else {
    error("PSK_E", "Failed to encode PSK data.");
    return "";
  }
}

std::string TelemetryModule::GenerateAprsTelemetry(const int tx_number) {
  (void)tx_number;
  return "";
}

std::string TelemetryModule::GenerateAprsLocation(const int tx_number) {
  std::string file_path = configurables::file_paths::kTelemetryWavLocation +
                          (std::string) "aprs-" + std::to_string(tx_number) +
                          ".wav";
  CriticalData critical_data = data_stream_.getCriticalData();
  GPSFrame gps = critical_data.latest_valid_gps_data;
  bool gps_valid = critical_data.gps_data_valid;

  if (gps_valid) {
    mwav::aprs_packet::CompressedPositionReport aprs_loc;
    aprs_loc.time_code = "000000"; /** @todo implement this */
    aprs_loc.latitude = gps.latitude;
    aprs_loc.longitude = gps.longitude;
    aprs_loc.altitude = unit_conversions::MetersToFeet(gps.altitude);
    aprs_loc.speed = unit_conversions::MetersPerSecondToKnots(gps.ground_speed);
    aprs_loc.course = gps.heading_of_motion;
    aprs_loc.comment = config_data_.telemetry.aprs_comment;
    /** @todo verify that the restricting in
    MWAV match the restrictions in GFS for the memo length*/
    try {
      mwav::EncodeAprs(file_path, aprs_generic_, aprs_loc);
    } catch (mwav::Exception &e) {
      error("APRS_LOC_EX", e.what());
      return "";
    }
    return file_path;
  } else {
    error("APRS_LOC_E");
    mwav::aprs_packet::Invalid aprs;
    std::string tmp = "Failed to generate APRS location.";
    for (char c : tmp) {
      aprs.data.push_back(c);
    }
    try {
      mwav::EncodeAprs(file_path, aprs_generic_, aprs);
    } catch (mwav::Exception &e) {
      error("APRS_LOC_EX", e.what());
      return "";
    }
    return file_path;
  }
}

std::string TelemetryModule::GenerateSSTV(const int tx_number) {
  std::string image = data_stream_.GetLatestImage();
  std::string image_path = configurables::file_paths::kImagesLocation + image;
  if (image == "") {
    error("SSTV_NI");
    return "";
  } else if (!FileExists(image_path)) {
    error("SSTV_IDE", image_path);
    return "";
  }
  std::string file_path = configurables::file_paths::kTelemetryWavLocation +
                          (std::string) "sstv-" + std::to_string(tx_number) +
                          ".wav";

  std::vector<std::string> data = {"Giraffe SSTV Test"};

  try {
    mwav::EncodeSSTV(file_path, image_path, false,
                     config_data_.telemetry.call_sign,
                     mwav::Sstv_Mode::ROBOT_36, data);
  } catch (mwav::Exception &e) {
    error("SSTV_EX", e.what());
    return "";
  }

  return file_path;
}

void TelemetryModule::runner() {
  updateStatus(ModuleStatus::RUNNING);

  if (primary_radio_ == nullptr) {
    error("NO_PRIM_RADIO");
    updateStatus(ModuleStatus::STOPPED);
    return;
  }

  while (!stop_flag_) {
    if (primary_radio_->GetStatus() != radio::Status::CONFIGURED) {
      if (primary_radio_->GetStatus() == radio::Status::OFF) {
        primary_radio_->PowerOn();
        primary_radio_->Initialize(config_data_.telemetry.data_packets_freq);
      }
      if (primary_radio_->GetStatus() == radio::Status::ERROR) {
        error("PRIM_RADIO_ERR");
        updateStatus(ModuleStatus::STOPPED);
        primary_radio_->Initialize(config_data_.telemetry.data_packets_freq);
      }
    }

    // Check for commands
    parseCommands();

    int queue_size = data_stream_.getTXQueueSize();
    data("TX_Q_SZ", queue_size);

    // Report the info about the TX log
    DataStream::TXLogInfo lg = data_stream_.getTXLogInfo();
    data("TX_LG_SZ", lg.tx_log_size);
    data("TX_LG_FRST", lg.first_tx_in_log);
    data("TX_LG_LAST", lg.last_tx_in_log);

    // Manage the TX queue
    if (queue_size > 0) {
      Transmission tx = data_stream_.getNextTX();
      std::string tx_length = std::to_string(tx.length);

      switch (tx.type) {
        case Transmission::Type::APRS:
          primary_radio_->SetFrequency(config_data_.telemetry.aprs_freq);
          Transmit(tx.wav_location, "APRS", tx.length);
          break;
        case Transmission::Type::SSTV:
          primary_radio_->SetFrequency(config_data_.telemetry.sstv_freq);
          Transmit(tx.wav_location, "SSTV", tx.length);
          break;
        case Transmission::Type::DATA:
          primary_radio_->SetFrequency(
              config_data_.telemetry.data_packets_freq);
          Transmit(tx.wav_location, "PSK", tx.length);
          break;
        default:
          error("BAD_TX_TYPE", std::to_string((int)tx.type));
          break;
      }
      data("ACTIVE_TX", "NONE");
    }
    std::this_thread::sleep_for(
        std::chrono::milliseconds(TELEMETRY_INTERVAL_MILI_SECONDS));
  }
}

void TelemetryModule::Transmit(std::string wav_location, std::string tx_type,
                               int tx_length) {
  if (primary_radio_->GetStatus() != radio::Status::CONFIGURED) {
    error("RAD_NCFG");
    return;
  }
  if (!primary_radio_->PTTOn()) {
    primary_radio_->PTTOff();
    error("RAD_PTT_ERR");
    return;
  }
  std::string command =
      "aplay " + wav_location +
      " >nul 2>nul";  // command to play with aplay suppress output

  data("ACTIVE_TX", tx_type + std::to_string(tx_length));

  /**
   * @details aplay seems to return 0 when it's done playing, 256 when
   * the file doesn't exist, and 2 when it was interrupted (ctrl+c).
   *
   * This allows me to stop a transmission early if needed and provides
   * time to exit GFS with another ctrl+c.
   *
   * This is definitely a bit of a hack but it gives the functionality
   * that I'm looking for. Eventually this could be replaced with popen
   * or a built in library for playing the wav files. I have had no issues
   * with this *so far*.
   */
  int exit_code = system(command.c_str());
  if (exit_code == 2) {
    std::cout << "Stopping Transmission Early - CTRL+C to exit GFS"
              << std::endl;
    std::this_thread::sleep_for(
        std::chrono::milliseconds(1000)  // Delay for 1 second
    );
  } else if (exit_code != 0) {
    error("APLAY_EXIT_CODE_" + std::to_string(exit_code), wav_location);
  }

  // Delete the wav file, it's no longer needed as the data is in the log
  if (!std::filesystem::remove(wav_location)) {
    error("DEL_WAV", wav_location);
  }
  primary_radio_->PTTOff();
}