/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   telemetry_module.hpp
 * @brief  The telemetry module.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-10-26
 * @copyright  2023 (license to be defined)
 */

#ifndef TELEMETRY_MODULE_HPP_
#define TELEMETRY_MODULE_HPP_

#include <optional>

#include <BoosterSeat/timer.hpp>

#include <giraffe_data_link.hpp>
#include <software_physical_layer.hpp>

#include "module.hpp"
#include "radios/sa868.hpp"

namespace modules {

static modules::MetaData metadata("telemetry_module",
                                  node::Identification::TELEMETRY_MODULE);

class TelemetryModule : public Module {
public:
  TelemetryModule(data::SharedData &shared_data, cfg::Configuration &config)
      : Module(metadata, shared_data, config) {
  }
  ~TelemetryModule() override = default;

private:
  void startup() override;
  void loop() override;
  void shutdown() override;

  /**
   * @brief Called if there is a message in the receive queue. Pops one message
   * and processes it.
   */
  void processTelemetryMessageQueue();

  /**
   * @brief This function will only send a packet if it is time to send one. If
   * not, it will just return immediately.
   */
  void sendAprsPositionPacket();

  /**
   * @brief Send an SSTV image.
   * @details Only functions if SSTV is configured. Requires a camera to be
   * connected.
   */
  void sendSstvImage();

  /**
   * @brief Called when the flight computer detects that it is descending. If
   * the data link is connected, it will send a message to the ground station.
   */
  void reportDescent();

  /**
   * @brief Process commands that are received from the flight runner.
   * @param command - The command to process.
   */
  void processCommand(const cmd::Command &command);

  uint32_t getNextMessageId() {
    return message_id_++;
  }

  int total_messages_sent_ = 0;
  int total_messages_received_ = 0;
  std::string last_received_message_ = "";

  bst::Timer aprs_position_packet_timer_{};

  uint32_t message_id_ = 0;

  /// @brief The configuration for the data link.
  giraffe::gdl::Config gdl_config_{false};

  /// @brief A software radio for the data link that can be used in place of a
  /// physical radio. It is mostly just stubbed out.
  // giraffe::gdl::SoftwarePhysicalLayer software_radio_{gdl_config_};

  /// @brief An optional SA868 radio that can be used in place of the software
  /// radio. This will be populated if it's selected in the configuration.
  // std::optional<radios::Sa868> sa868_ = std::nullopt;

  std::shared_ptr<giraffe::gdl::PhysicalLayer> physical_layer_ = nullptr;

  /// @brief The Data Link used for telemetry.
  giraffe::gdl::DataLink gdl_{gdl_config_};
};

} // namespace modules

#endif /* TELEMETRY_MODULE_HPP_ */