/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   command.hpp
 * @brief  The command/queue system for the flight system.
 * @addtogroup control
 * @{
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

/**
 * @brief A command object. Used for passing commands to their destinations.
 */
struct Command {
  node::Identification destination = node::Identification::UNKNOWN;
  cmd::CommandId command_id = cmd::CommandId::GENERAL_unknown;
  uint32_t int_arg = 0;
  std::string str_arg{};
};

/**
 * @brief Used within modules to queue commands to be processed.
 */
class CommandQueue {
public:
  CommandQueue(node::Identification this_destination)
      : this_destination_(this_destination) {
  }

  /**
   * @brief Adds a command to the queue.
   * @param command The command to add.
   * @return true if the command was added successfully (space in the queue).
   * False otherwise.
   */
  bool addCommand(Command command);

  /**
   * @brief Gets the next command in the queue.
   * @param command (out) The command to populate.
   * @return true if a command was retrieved. If the queue is empty, this will
   * return false.
   */
  bool getCommand(Command &command);

private:
  /**
   * @brief The destination of this command queue. Used to filter commands.
   */
  node::Identification this_destination_;

  /**
   * @brief Mutex lock for the queue as multiple threads may be accessing it.
   */
  std::mutex lock_ = std::mutex();

  /**
   * @brief The queue of commands.
   */
  std::queue<Command> queue_ = std::queue<Command>();
};

/**
 * @brief Parses a command string into a command object.
 * @param command_string The command string to parse.
 * @param command (out) The command object to populate.
 * @return true if the command string was parsed successfully. False otherwise.
 */
bool parseCommandString(const std::string &command_string, Command &command);

} // namespace cmd

/**
 * @}
 */

#endif /* COMMAND_HPP_ */
