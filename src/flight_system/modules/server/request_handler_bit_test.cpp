/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   request_handler_image.cpp
 * @brief  The image request handler implementation.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-02-20
 * @copyright  2024 (license to be defined)
 */

#include "request_handler.hpp"

void RequestRouter::handleBitTestSet(sock::TcpSocketServer &client,
                                     const std::string &msg_rsc) {
  try {
    if (msg_rsc == "bit_test/start") {

      cmd::Command command{};
      command.destination = node::Identification::FLIGHT_RUNNER;
      command.command_id = cmd::CommandId::FLIGHT_RUNNER_startBitTest;
      shared_data_.streams.command.addCommand(
          node::Identification::SERVER_MODULE, command);

      sendSuccessResponse(client);
      return;
    } else if (msg_rsc == "bit_test/stop") {

      cmd::Command command{};
      command.destination = node::Identification::FLIGHT_RUNNER;
      command.command_id = cmd::CommandId::FLIGHT_RUNNER_stopBitTest;
      shared_data_.streams.command.addCommand(
          node::Identification::SERVER_MODULE, command);

      sendSuccessResponse(client);
      return;
    } else if (msg_rsc == "bit_test/reset") {
      cmd::Command command{};
      command.destination = node::Identification::FLIGHT_RUNNER;
      command.command_id = cmd::CommandId::FLIGHT_RUNNER_resetBitTest;
      shared_data_.streams.command.addCommand(
          node::Identification::SERVER_MODULE, command);

      sendSuccessResponse(client);
      return;
    } else {
      sendErrorPacket(client, "invalid bit test set request");
      return;
    }

    sendErrorPacket(client, "invalid bit test set request");
    return;
  } catch (const std::exception &e) {
    sendErrorPacket(client, "error handling bit test set request");
    return;
  }
}