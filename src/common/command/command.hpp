/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   commands.hpp
 * @brief  The command/queue system for the flight system.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-13
 * @copyright  2023 (license to be defined)
 */

#ifndef COMMAND_HPP_
#define COMMAND_HPP_

#include <mutex>
#include <queue>

#include "command_ids.hpp"
#include "node.hpp"

namespace cmd {

struct Command {
  node::Identification destination = node::Identification::UNKNOWN;
  cmd::CommandId command_id = cmd::CommandId::GENERAL_unknown;
};

class CommandQueue {
public:
  CommandQueue(node::Identification this_destination)
      : this_destination_(this_destination) {
  }

  bool addCommand(Command command);
  bool getCommand(Command &command);

private:
  node::Identification this_destination_;

  std::mutex lock_ = std::mutex();
  std::queue<Command> queue_ = std::queue<Command>();
};

} // namespace cmd

#endif /* COMMAND_HPP_ */