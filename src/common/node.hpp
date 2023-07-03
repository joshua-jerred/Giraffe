/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   node.hpp
 * @brief  Contains structures that are used to identify components of the
 * flight system.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-13
 * @copyright  2023 (license to be defined)
 */

#ifndef NODE_HPP_
#define NODE_HPP_

#include <cstdint>
#include <map>

namespace node {

/**
 * @brief Node identification, used for logging, telemetry, command routing,
 * and more.
 */
enum class Identification {
  UNKNOWN,
  FLIGHT_RUNNER,
  CONFIGURATION,
  DATA_MODULE,
  CONSOLE_MODULE,
  SERVER_MODULE,
  SYSTEM_MODULE,
  EXTENSION_MODULE,
  EXTENSION
};

/**
 * @brief Map of node identification to string
 * @see node::Identification
 */
static const std::map<Identification, std::string> identification_to_string = {
    {Identification::UNKNOWN, "unknown"},
    {Identification::FLIGHT_RUNNER, "flight_runner"},
    {Identification::CONFIGURATION, "configuration"},
    {Identification::DATA_MODULE, "data_module"},
    {Identification::CONSOLE_MODULE, "console_module"},
    {Identification::SERVER_MODULE, "server_module"},
    {Identification::SYSTEM_MODULE, "system_module"},
    {Identification::EXTENSION_MODULE, "extension_module"},
    {Identification::EXTENSION, "extension"}};

/**
 * @brief Node status, used for modules and extensions.
 */
enum class Status : uint16_t {
  UNKNOWN = 0b00000001,
  DISABLED = 0b00000010,
  STOPPED = 0b00000100,
  STARTING = 0b00001000,
  RUNNING = 0b00010000,
  SLEEPING = 0b00100000,
  STOPPING = 0b01000000,
  ERROR = 0b10000000
};

inline constexpr uint16_t kNodeInactiveStatuses =
    static_cast<uint16_t>(Status::DISABLED) |
    static_cast<uint16_t>(Status::STOPPED) |
    static_cast<uint16_t>(Status::ERROR);

inline constexpr uint16_t kNodeActiveStatuses =
    static_cast<uint16_t>(Status::STARTING) |
    static_cast<uint16_t>(Status::RUNNING) |
    static_cast<uint16_t>(Status::SLEEPING) |
    static_cast<uint16_t>(Status::STOPPING);

/**
 * @brief Map of node status to string
 * @see node::Status
 */
static const std::map<Status, std::string> status_to_string = {
    {Status::UNKNOWN, "unknown"},   {Status::DISABLED, "disabled"},
    {Status::STOPPED, "stopped"},   {Status::STARTING, "starting"},
    {Status::RUNNING, "running"},   {Status::SLEEPING, "sleeping"},
    {Status::STOPPING, "stopping"}, {Status::ERROR, "error"}};

} // namespace node

#endif