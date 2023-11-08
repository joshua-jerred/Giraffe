/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   test_command_processor.cpp
 * @brief  Test of the command processor.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-11-03
 * @copyright  2023 (license to be defined)
 */

#include "unit_test.hpp"

#include "command.hpp"

TEST(commands_DataModule, clearAllErrors) {
  cmd::Command command{};
  std::string command_string = "cmd/dat/cae/"; // Clear all errors command
  EXPECT_TRUE(cmd::parseCommandString(command_string, command));
  EXPECT_EQ(command.destination, node::Identification::DATA_MODULE);
  EXPECT_EQ(command.command_id, cmd::CommandId::DATA_MODULE_clearAllErrors);
  EXPECT_EQ(command.int_arg, 0);
  EXPECT_EQ(command.str_arg, "");
}

TEST(commands_DataModule, clearSpecificError) {
  cmd::Command command{};
  std::string command_string =
      "cmd/dat/cse/bbbb"; // Clear specific error command
  EXPECT_TRUE(cmd::parseCommandString(command_string, command));
  EXPECT_EQ(command.destination, node::Identification::DATA_MODULE);
  EXPECT_EQ(command.command_id, cmd::CommandId::DATA_MODULE_clearSpecificError);
  EXPECT_EQ(command.int_arg, 0xBBBB);
  EXPECT_EQ(command.str_arg, "");
}

TEST(commands_FlightRunner, shutdownFlightSystem) {
  cmd::Command command{};
  std::string command_string = "cmd/flr/sdn/";
  EXPECT_TRUE(cmd::parseCommandString(command_string, command));
  EXPECT_EQ(command.destination, node::Identification::FLIGHT_RUNNER);
  EXPECT_EQ(command.command_id, cmd::CommandId::FLIGHT_RUNNER_shutdownSystem);
  EXPECT_EQ(command.int_arg, 0);
  EXPECT_EQ(command.str_arg, "");
}