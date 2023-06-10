/**
 * @file command.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Data Structures for commands.
 * @date 2023-05-16
 * @copyright Copyright (c) 2023
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include <mutex>
#include <queue>

#include "node.h"

namespace command {
struct Command {
  node::Identification destination = node::Identification::UNKNOWN;
};

class CommandQueue {
public:
  CommandQueue(node::Identification this_destination)
      : this_destination_(this_destination) {
  }

  bool addCommand(command::Command command);
  bool getCommand(command::Command &command);

private:
  node::Identification this_destination_;

  std::mutex lock_ = std::mutex();
  std::queue<command::Command> queue_ = std::queue<command::Command>();
};

}  // namespace command

#endif